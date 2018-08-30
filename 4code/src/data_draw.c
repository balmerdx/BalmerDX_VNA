#include <stdint.h>
#include "data.h"
#include "ili/UTFT.h"
#include "hardware/store_to_stm32_flash.h"
#include "calibration_solt.h"

#include <math.h>

Point plot_data[PLOT_MAX_POINTS];
Point plot_data2[PLOT_MAX_POINTS];

const float amplitude_mul = 1e-7/27;
const float mul_raw_s21 = 0.2f;
const float mul_raw_s11 = 1.0f/5.62f;

float dB(float y)
{
    if(y<1e-6)
        y = 1e-6;

    return 20*log(y)/log(10);
}

void CalcMinMax(Point* points, int points_count, bool reset, Point* pmin, Point* pmax)
{
    if(reset)
    {
        *pmin = *points;
        *pmax = *points;
    }

    for(int i=0; i<points_count; i++)
    {
        Point p = points[i];
        if(pmin->x > p.x)
            pmin->x = p.x;
        if(pmax->x < p.x)
            pmax->x = p.x;
        if(pmin->y > p.y)
            pmin->y = p.y;
        if(pmax->y < p.y)
            pmax->y = p.y;
    }
}

void DrawIQBuffer(bool drawI, bool drawQ)
{
    if(!drawI && !drawQ)
        return;

    float mul = 1e-7f;
    float xmin = 0, xmax = 1, ymin = 0, ymax = 1;
    xmin = 0;
    xmax = SAMPLE_BUFFER_SIZE;

    int add = SAMPLE_BUFFER_SIZE/PLOT_MAX_POINTS;

    for(int i=0; i<PLOT_MAX_POINTS; i++)
    {
        float yQ = g_samplesQ[i*add]*mul;
        float yI = g_samplesI[i*add]*mul;

        if(i==0)
        {
            ymin = ymax = drawQ?yQ:yI;
        }

        if(drawI)
        {
            if(yI<ymin)
                ymin = yI;
            if(yI>ymax)
                ymax = yI;
        }


        if(drawQ)
        {
            if(yQ<ymin)
                ymin = yQ;
            if(yQ>ymax)
                ymax = yQ;
        }

    }

    PlotSetAxis(xmin, xmax, ymin, ymax);

    if(drawQ)
    {
        for(int i=0; i<PLOT_MAX_POINTS; i++)
        {
            float x = i*add;
            float y = g_samplesQ[i*add]*mul;
            plot_data[i].x = x;
            plot_data[i].y = y;
        }

        PlotDrawGraph(0, plot_data, PLOT_MAX_POINTS, VGA_GREEN);
    }

    if(drawI)
    {
        for(int i=0; i<PLOT_MAX_POINTS; i++)
        {
            float x = i*add;
            float y = g_samplesI[i*add]*mul;

            plot_data[i].x = x;
            plot_data[i].y = y;
        }

        PlotDrawGraph(1, plot_data, PLOT_MAX_POINTS, VGA_RED);
    }

}

void DrawRefAmplitudeDB()
{
    float xmin = 0, xmax = 1, ymin = 0, ymax = 1;
    for(int i=0; i<raw.s_param_count; i++)
    {
        SParam* p = raw.s_param + i;

        float x = p->freq;
        float y = dB(p->ref_amplitude*amplitude_mul);

        if(i==0)
        {
            xmin = xmax = x;
            ymin = ymax = y;
        } else
        {
            if(x<xmin)
                xmin = x;
            if(x>xmax)
                xmax = x;
            if(y<ymin)
                ymin = y;
            if(y>ymax)
                ymax = y;
        }

        plot_data[i].x = x;
        plot_data[i].y = y;
    }
    PlotSetAxis(xmin, xmax, ymin, ymax);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_GREEN);
}

void DrawS21ErrorDB()
{
    Point pmin,  pmax;
    for(int i=0; i<raw.s_param_count; i++)
    {
        SParam* p = raw.s_param + i;

        float x = p->freq;
        float y = dB(p->S21err*mul_raw_s21);

        plot_data[i].x = x;
        plot_data[i].y = y;
    }

    for(int i=0; i<raw.s_param_count; i++)
    {
        SParam* p = raw.s_param + i;

        float x = p->freq;
        float y = dB(cabsf(p->S21)*mul_raw_s21);

        plot_data2[i].x = x;
        plot_data2[i].y = y;
    }

    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);
    CalcMinMax(plot_data2, raw.s_param_count, false, &pmin, &pmax);
    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_RED);
    PlotDrawGraph(1, plot_data2, raw.s_param_count, VGA_GREEN);
}

void DrawS11()
{
    Point pmin,  pmax;
    for(int i=0; i<raw.s_param_count; i++)
    {
        complexf S11, S21;
        CalibrationCalculateI(i, &S11, &S21);
        plot_data[i].x = raw.s_param[i].freq;
        plot_data[i].y = crealf(S11);
        plot_data2[i].x = raw.s_param[i].freq;
        plot_data2[i].y = cimagf(S11);
    }

    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);
    CalcMinMax(plot_data2, raw.s_param_count, false, &pmin, &pmax);

    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_RED);
    PlotDrawGraph(1, plot_data2, raw.s_param_count, VGA_BLUE);
}

void DrawAbsS11()
{
    Point pmin,  pmax;
    for(int i=0; i<raw.s_param_count; i++)
    {
        complexf S11, S21;
        CalibrationCalculateI(i, &S11, &S21);
        plot_data[i].x = raw.s_param[i].freq;
        plot_data[i].y = cabsf(S11);
    }
    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);

    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_GREEN);
}


void DrawS21()
{
    Point pmin,  pmax;
    for(int i=0; i<raw.s_param_count; i++)
    {
        complexf S11, S21;
        CalibrationCalculateI(i, &S11, &S21);
        plot_data[i].x = raw.s_param[i].freq;
        plot_data[i].y = crealf(S21);
        plot_data2[i].x = raw.s_param[i].freq;
        plot_data2[i].y = cimagf(S21);
    }

    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);
    CalcMinMax(plot_data2, raw.s_param_count, false, &pmin, &pmax);

    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_RED);
    PlotDrawGraph(1, plot_data2, raw.s_param_count, VGA_BLUE);
}

void Draw_dB_S21()
{
    Point pmin,  pmax;
    for(int i=0; i<raw.s_param_count; i++)
    {
        complexf S11, S21;
        CalibrationCalculateI(i, &S11, &S21);
        plot_data[i].x = raw.s_param[i].freq;
        plot_data[i].y = dB(cabsf(S21));
    }
    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);

    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_GREEN);
}

void DrawAbsS21()
{
    Point pmin,  pmax;
    for(int i=0; i<raw.s_param_count; i++)
    {
        complexf S11, S21;
        CalibrationCalculateI(i, &S11, &S21);
        plot_data[i].x = raw.s_param[i].freq;
        plot_data[i].y = cabsf(S21);
    }
    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);

    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_GREEN);
}


void Draw_e30DB()
{
    Point pmin,  pmax;
    for(int i=0; i<g_calibration.count; i++)
    {
        freq_type freq = g_calibration.elems[i].freq;
        EParams p = calcEParams(freq);
        plot_data[i].x = freq;
        plot_data[i].y = dB(cabsf(p.e30)*mul_raw_s21);
    }
    CalcMinMax(plot_data, raw.s_param_count, true, &pmin, &pmax);

    PlotSetAxis(pmin.x, pmax.x, pmin.y, pmax.y);
    PlotDrawGraph(0, plot_data, raw.s_param_count, VGA_GREEN);
}
