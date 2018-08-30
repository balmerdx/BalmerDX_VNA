#include "plot.h"
#include "ili/UTFT.h"
#include "ili/DefaultFonts.h"
#include "progress_bar.h"
#include "colors.h"

#include <math.h>

typedef struct Line
{
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
} Line;

typedef struct GraphData
{
    uint16_t count;
    uint16_t colorW;
    Line lines[PLOT_MAX_POINTS];
} GraphData;


static int g_full_x;
static int g_full_y;
static int g_full_dx;
static int g_full_dy;

static int g_plot_x;
static int g_plot_y;
static int g_plot_dx;
static int g_plot_dy;

static uint16_t g_background_color = VGA_BLACK;
static uint16_t g_outline_color = COLOR_FOREGROUND_DARK_GRAY;

static uint16_t g_left_axe_chars = 5;

static float g_xmin = 0;
static float g_xmax = 1;
static float g_ymin = 0;
static float g_ymax = 1;

static float g_scale_x = 1; //Конвертируем float -> pixel
static float g_scale_y = 1;

static GraphData g_graph[PLOT_MAX_GRAPH];

static bool g_line_visible = false;
static int g_line_pos = 0;


#define PLOT_MAX_TICKS 60
static Line g_ticks[PLOT_MAX_TICKS];
static int g_ticks_count = 0;

static void PlotAddTicks(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    if(g_ticks_count>=PLOT_MAX_TICKS)
        return;

    Line* l = g_ticks + g_ticks_count;
    if(x0<x1)
    {
        l->x0 = x0;
        l->y0 = y0;
        l->x1 = x1;
        l->y1 = y1;
    } else
    {
        l->x1 = x0;
        l->y1 = y0;
        l->x0 = x1;
        l->y0 = y1;
    }
    g_ticks_count++;
}


void PlotHideLine();
void PlotDrawLine();

void PlotSetFont()
{
    UTFT_setFont(FONT8x15);
}

void PlotClear()
{
    g_ticks_count = 0;

    UTFT_setColorW(g_background_color);
    //UTFT_fillRect(g_full_x, g_full_y, g_full_x+g_full_dx, g_full_y+g_full_dy);
    //left
    UTFT_fillRect(g_full_x, g_full_y, g_plot_x-1, g_full_y+g_full_dy-1);
    //bottom
    UTFT_fillRect(g_plot_x, g_plot_y+g_plot_dy+1, g_full_x+g_full_dx-1, g_full_y+g_full_dy-1);
    //top
    UTFT_fillRect(g_plot_x, g_full_y, g_full_x+g_full_dx, g_plot_y-1);


    UTFT_setColorW(g_outline_color);
    UTFT_drawRect(g_plot_x, g_plot_y, g_plot_x+g_plot_dx, g_plot_y+g_plot_dy);
    PlotAddTicks(g_plot_x, g_plot_y, g_plot_x, g_plot_y+g_plot_dy);
    PlotAddTicks(g_plot_x+g_plot_dx, g_plot_y, g_plot_x+g_plot_dx, g_plot_y+g_plot_dy);

    UTFT_setColorW(VGA_BLACK);
    UTFT_fillRect(g_plot_x+1, g_plot_y+1, g_plot_x+g_plot_dx-1, g_plot_y+g_plot_dy-1);

    for(int i=0; i<PLOT_MAX_GRAPH; i++)
    {
        g_graph[i].count = 0;
    }

    g_line_visible = false;
}


void PlotInit(int x, int y, int dx, int dy)
{
    PlotSetFont();
    g_full_x = x;
    g_full_y = y;
    g_full_dx = dx;
    g_full_dy = dy;


    uint8_t cx = UTFT_getFontXsize();
    uint8_t cy = UTFT_getFontYsize();
    g_plot_x = x + cx*g_left_axe_chars+2;
    g_plot_y = y + cy/2+1;
    g_plot_dx = g_full_x+g_full_dx-g_plot_x-1;
    //g_plot_dy = g_full_y+g_full_dy-g_plot_y-1-cy/2;
    g_plot_dy = g_full_y+g_full_dy-g_plot_y-2-cy;

    ProgressInit(g_plot_x, g_full_y, g_plot_dx, g_plot_y-g_full_y);

    PlotClear();
}

//Предполагаем, что самое большое, что может встретиться -9999
//Самое маленькое, что может встретиться -0.01
void PlotCalcTicks(float xmin, float xmax,
                   int* ptick_xmin, int* ptick_xmax, int* ptick_dx, float* ptick_mul, int* float_places)
{
    *ptick_xmin = 0;
    *ptick_xmax = 10;
    *ptick_dx = 1;
    *ptick_mul = 1;
    *float_places = 0;

    float dx = xmax-xmin;
    if(dx<0)
        dx = -dx;

    float ftick = 1;

    float c = 3.f;
    for(int i=-2; i<8; i++)
    {
        ftick = 1;
        if(i<0)
        {
            for(int j=0; j<-i; j++)
                ftick *= 0.1f;
            *float_places = -i;
        } else
        {
            for(int j=0; j<i; j++)
                ftick *= 10.f;
            *float_places = 0;
        }

        //if(dx>=c*ftick && dx<c*10*ftick)
        if(dx<c*10*ftick)
            break;
    }

    uint32_t count = lround(dx/ftick);

    *ptick_dx = 1;
    if(count>=c*5)
        *ptick_dx = 5;
    else
    if(count>=c*2)
        *ptick_dx = 2;

    *ptick_xmin = floor(xmin/(ftick* *ptick_dx))* *ptick_dx;
    *ptick_xmax = ceil(xmax/(ftick* *ptick_dx))* *ptick_dx;
    *ptick_mul = ftick;
}

int PlotFloatToScreenX(float value)
{
    return g_plot_x + lround(g_scale_x*(value-g_xmin));
}

int PlotFloatToScreenY(float value)
{
    int bottom = g_plot_y + g_plot_dy;
    return bottom - lround(g_scale_y*(value-g_ymin));
}

float PlotScreenToFloatX(int x)
{
    return (x - g_plot_x)/g_scale_x + g_xmin;
}

void PlotSetAxis(float xmin, float xmax, float ymin, float ymax)
{
    g_xmin = xmin;
    g_xmax = xmax;
    g_ymin = ymin;
    g_ymax = ymax;

    PlotClear();

    UTFT_setBackColorW(g_background_color);
    PlotSetFont();
    uint8_t fontx = UTFT_getFontXsize();
    uint8_t fonty = UTFT_getFontYsize();

    const int tick_len = 5;
    int g_plot_xmax = g_plot_x + g_plot_dx;
    int g_plot_ymax = g_plot_y + g_plot_dy;

    {//Make labels Y
        int tick_ymin = 0;
        int tick_ymax = 10;
        int tick_delta = 2;
        float tick_mul = 0.1f;
        int float_places = 1;

        PlotCalcTicks(ymin, ymax, &tick_ymin, &tick_ymax,
                      &tick_delta, &tick_mul, &float_places);

        g_ymin = tick_ymin*tick_mul;
        g_ymax = tick_ymax*tick_mul;

        g_scale_y = g_plot_dy/(g_ymax-g_ymin);

        for(int t=tick_ymin; t<=tick_ymax; t+=tick_delta)
        {
            float value = t*tick_mul;
            int y = PlotFloatToScreenY(value);

            UTFT_setColorW(VGA_WHITE);
            UTFT_printNumF(value, g_full_x, y-fonty/2, float_places, g_left_axe_chars, true);

            PlotAddTicks(g_plot_x+1, y, g_plot_x+tick_len, y);
            PlotAddTicks(g_plot_xmax-1, y, g_plot_xmax-tick_len, y);
        }
    }

    {//make labels X
        int tick_xmin = 0;
        int tick_xmax = 10;
        int tick_delta = 2;
        float tick_mul = 0.1f;
        int float_places = 1;

        g_scale_x = g_plot_dx/(g_xmax-g_xmin);

        PlotCalcTicks(xmin, xmax, &tick_xmin, &tick_xmax,
                      &tick_delta, &tick_mul, &float_places);

        float t_mul = 1;
        float_places = 0;
        if(tick_mul>99e3)
        {
            t_mul = tick_mul/1e6;
            float_places = 1;
            if(tick_mul>999e3)
                float_places = 0;
        }

        for(int t=tick_xmin+tick_delta; t<=tick_xmax-tick_delta; t+=tick_delta)
        {
            float value = t*tick_mul;
            int x = PlotFloatToScreenX(value);
            int num_len = 2;
            int start_label_x = x-fontx*3/2;
            if(start_label_x+num_len*fontx > g_plot_xmax)
                continue;

            UTFT_setColorW(VGA_WHITE);
            UTFT_printNumF(t*t_mul, start_label_x, g_plot_ymax+2, float_places, 3, true);

            PlotAddTicks(x, g_plot_ymax-1, x, g_plot_ymax-tick_len);
            PlotAddTicks(x, g_plot_y+1, x, g_plot_y+tick_len);
        }
    }

    UTFT_setColorW(g_outline_color);
    for(int i=0; i<g_ticks_count; i++)
    {
        Line* l = g_ticks+i;
        UTFT_drawLine(l->x0, l->y0, l->x1, l->y1);
    }

}


//Обрезаем линию так, чтобы она вмещалась в прямоугольник g_xmin, g_xmax,g_ymin, g_ymax
//return false если не получилось обрезать
bool PlotClampLine(float* x0, float* y0, float* x1, float* y1)
{
    float dx = *x1-*x0;
    float dy = *y1-*y0;
    float xstart = *x0;
    float ystart = *y0;

    bool zero_dy = fabsf(dy)<1e-6;
    bool zero_dx = fabsf(dx)<1e-6;
    /*
    x = xstart + t*dx;
    y = ystart + t*dy;

    t = (y-ystart)/dy;
    x = xstart + (y-ystart)*dx/dy;

    */

    //Обрезаем по X
    if(*x0>g_xmax && *x1>g_xmax)
        return false;
    if(*x0<g_xmin && *x1<g_xmin)
        return false;

    if(*x0>g_xmax)
    {
        if(zero_dx)
            return false;

        *x0 = g_xmax;
        *y0 = ystart + (*x0-xstart)*dy/dx;
    }

    if(*x1>g_xmax)
    {
        if(zero_dx)
            return false;

        *x1 = g_xmax;
        *y1 = ystart + (*x1-xstart)*dy/dx;
    }

    if(*x0<g_xmin)
    {
        if(zero_dx)
            return false;

        *x0 = g_xmin;
        *y0 = ystart + (*x0-xstart)*dy/dx;
    }

    if(*x1<g_xmin)
    {
        if(zero_dx)
            return false;

        *x1 = g_xmin;
        *y1 = ystart + (*x1-xstart)*dy/dx;
    }

    //Обрезаем по Y
    if(*y0>g_ymax && *y1>g_ymax)
        return false;
    if(*y0<g_ymin && *y1<g_ymin)
        return false;

    if(*y0>g_ymax)
    {
        if(zero_dy)
            return false;

        *y0 = g_ymax;
        *x0 = xstart + (*y0-ystart)*dx/dy;
    }

    if(*y1>g_ymax)
    {
        if(zero_dy)
            return false;

        *y1 = g_ymax;
        *x1 = xstart + (*y1-ystart)*dx/dy;
    }

    if(*y0<g_ymin)
    {
        if(zero_dy)
            return false;

        *y0 = g_ymin;
        *x0 = xstart + (*y0-ystart)*dx/dy;
    }

    if(*y1<g_ymin)
    {
        if(zero_dy)
            return false;

        *y1 = g_ymin;
        *x1 = xstart + (*y1-ystart)*dx/dy;
    }

    return true;
}

void PlotDrawGraph(int graph_index, Point* points, int count, uint16_t colorW)
{
    if(count<2)
        return;
    if(count>PLOT_MAX_POINTS)
        return;
    if(graph_index<0 || graph_index>=PLOT_MAX_GRAPH)
        return;

    GraphData* d = g_graph + graph_index;
    d->count = count-1;
    d->colorW = colorW;

    for(int i=1; i<count; i++)
    {
        Point* p0 = points+(i-1);
        Point* p1 = points+i;

        float px0 = p0->x, py0 = p0->y;
        float px1 = p1->x, py1 = p1->y;

        if(!PlotClampLine(&px0, &py0, &px1, &py1))
            continue;

        int x0 = PlotFloatToScreenX(px0);
        int y0 = PlotFloatToScreenY(py0);
        int x1 = PlotFloatToScreenX(px1);
        int y1 = PlotFloatToScreenY(py1);

        Line* l = d->lines+(i-1);
        l->x0 = x0;
        l->y0 = y0;
        l->x1 = x1;
        l->y1 = y1;
    }

    UTFT_setColorW(d->colorW);
    for(int i=0; i<d->count; i++)
    {
        Line* l = d->lines+i;
        UTFT_drawLine(l->x0, l->y0, l->x1, l->y1);
    }
}

static void PlotUpdateLines(int xmin, int xmax, Line* lines, uint16_t count)
{
    for(int i=0; i<count; i++)
    {
        Line* l = lines+i;
        if(l->x1+1 < xmin)
            continue;
        if(l->x0-1 > xmax)
            continue;

        UTFT_drawLine(l->x0, l->y0, l->x1, l->y1);
    }

}

static void PlotUpdateGraphs(int xmin, int xmax)
{
    for(int graph_index=0; graph_index<PLOT_MAX_GRAPH; graph_index++)
    {
        GraphData* d = g_graph + graph_index;
        UTFT_setColorW(d->colorW);
        PlotUpdateLines(xmin, xmax, d->lines, d->count);
    }

    UTFT_setColorW(g_outline_color);
    PlotUpdateLines(xmin, xmax, g_ticks, g_ticks_count);
}

void PlotLineSetPos(int x)
{
    if(x<0)
        x = 0;
    if(x>=g_plot_dx)
        x = g_plot_dx;

    bool changed = g_line_pos != x;
    if(changed && g_line_visible)
    {
        PlotHideLine();
    }

    g_line_pos = x;

    if(changed && g_line_visible)
    {
        PlotDrawLine();
    }
}

bool PlotLineVisible()
{
    return g_line_visible;
}

void PlotLineSetVisible(bool show)
{
    if(g_line_visible==show)
        return;

    if(g_line_visible)
    {
        PlotHideLine();
    }

    if(show)
    {
        PlotDrawLine();
    }

    g_line_visible = show;
}

int PlotLinePos()
{
    return g_line_pos;
}

void PlotHideLine()
{
    int x = g_plot_x + g_line_pos;
    PlotUpdateGraphs(x, x);
    UTFT_setColorW(VGA_BLACK);
    UTFT_drawLine(x, g_plot_y+1, x, g_plot_y+g_plot_dy-1);
}

void PlotDrawLine()
{
    UTFT_setColorW(VGA_WHITE);
    int x = g_plot_x + g_line_pos;
    UTFT_drawLine(x, g_plot_y+1, x, g_plot_y+g_plot_dy-1);
}

float PlotLineX()
{
    int x = g_plot_x + g_line_pos;
    return PlotScreenToFloatX(x);
}

uint64_t PlotLineXround()
{
    int x = g_plot_x + g_line_pos;
    return llroundf(PlotScreenToFloatX(x));
}
