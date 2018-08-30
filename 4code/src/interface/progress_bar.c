#include "progress_bar.h"
#include "ili/UTFT.h"
#include <math.h>

static uint16_t g_background_color = VGA_BLACK;
static uint16_t g_progress_color = UTFT_COLOR(128, 255, 0);
static uint16_t g_background_progress = UTFT_COLOR(60, 60, 60);

static bool g_visible = false;
static float g_pos = 0;

static int g_x = 0;
static int g_dx = 0;
static int g_ymin = 0;
static int g_ymax = 0;

void ProgressInit(int x, int y, int dx, int dy)
{
    g_visible = false;
    g_pos = 0;

    g_x = x;
    g_dx = dx;
    g_ymin = y;
    g_ymax = y+dy-1;
}

void ProgressSetVisible(bool show)
{
    if(g_visible == show)
        return;
    g_visible = show;
    g_pos = 0;
    UTFT_setColorW(show?g_background_progress:g_background_color);
    //UTFT_fillRect(g_plot_x, g_full_y, g_plot_x+g_plot_dx, g_plot_y-1);
    UTFT_fillRect(g_x, g_ymin, g_x+g_dx-1, g_ymax);
}

bool ProgressVisible()
{
    return g_visible;
}

void ProgressSetPos(float pos)
{
    if(pos<0)
        pos=0;
    if(pos>1)
        pos = 1;

    int x_last = lround(g_pos*g_dx);
    int x_cur = lround(pos*g_dx);

    g_pos = pos;

    if(x_last==x_cur)
        return;
    if(!g_visible)
        return;

    if(x_last<x_cur)
    {
        UTFT_setColorW(g_progress_color);
        UTFT_fillRect(g_x+x_last, g_ymin, g_x+x_cur, g_ymax);
    } else
    {
        UTFT_setColorW(g_background_progress);
        UTFT_fillRect(g_x+x_cur+1, g_ymin, g_x+x_last, g_ymax);
    }

}

float ProgressPos()
{
    return g_pos;
}
