#include "main.h"
#include "interface.h"
#include "scene_graph.h"
#include "scene_check_intermediate_frequency.h"
#include "progress_bar.h"
#include "main.h"
#include "calculate_lc.h"
#include "srlc_format.h"
#include "calibration_solt.h"
#include "measure_freq.h"
#include "store_user_settings.h"
#include "colors.h"
#include <string.h>

static int max_str_len = 20;

bool g_scan_rx_and_tx = false;
bool g_scan_speed_slow = false;
int g_scan_points = 99;
int g_min_freq_khz = 1000;
int g_max_freq_khz = 200000;
GraphTypeMenuEnum g_graph_type = GTME_S11;
InfoPanelMode g_info_panel_mode = IPM_S11_S21_RAW;

static bool start_scan_in_device = false; //true = запускаем сканирование с устройства, false - запустили сканирование с компьютера
static int job_last_data_index = -1;

static const uint16_t BOTTOM_BACK_COLOR = UTFT_COLOR(0,128,0);//COLOR_BACKGROUND_BLUE;
static const uint16_t TOP_BACK_COLOR = UTFT_COLOR(255,0,0);//COLOR_BACKGROUND_BLUE;
static const uint16_t FREQ_BACK_COLOR = COLOR_BACKGROUND_DARK_GRAY;

#define LINE_TOP_Y 0
static uint16_t LINE1_Y;
static uint16_t LINE2_Y;

#define INFO_CHARS 26
static uint16_t X_SEL = 0;

char* g_graph_name = NULL;

void SceneGraphQuant();
void SceneGraphStartScan();
void SceneGraphQuantScan();

void SceneGraphDrawFreq();
void SceneGraphDrawFminFmax();
void SceneGraphDrawName();
void SceneGraphDrawCorrectionModel();
void SceneGraphDrawInfoPanel();

void SceneGraphDrawCurrentGraph();


void SceneGraphStart()
{
    StoreUserSettings();

    LINE1_Y = UTFT_getDisplayYSize()-240+210;
    LINE2_Y = UTFT_getDisplayYSize()-240+225;

    UTFT_setFont(FONT8x15);
    int cx = UTFT_getFontXsize();
    int cy = UTFT_getFontYsize();

    UTFT_setBackColorW(VGA_BLACK);
    UTFT_clrScr();

    PlotInit(0, 15, UTFT_getDisplayXSize(), UTFT_getDisplayYSize()-240+195);

    if(0)
    {
        PlotSetAxis(0, 1, -1, 1);
        #define DATA_COUNT 50
        for(int i=0; i<DATA_COUNT; i++)
        {
            float x = (i/(float)(DATA_COUNT-1));
            float y = sin(x*6*pi);
            plot_data[i].x = x;
            plot_data[i].y = y;
        }
        PlotDrawGraph(0, plot_data, DATA_COUNT, VGA_RED);

        for(int i=0; i<DATA_COUNT; i++)
        {
            float x = (i/(float)(DATA_COUNT-1));
            float y = x*x;
            plot_data[i].x = x;
            plot_data[i].y = y;
        }
        PlotDrawGraph(1, plot_data, DATA_COUNT, VGA_GREEN);
    }

    SceneGraphDrawCurrentGraph();

    UTFT_setColorW(VGA_YELLOW);
    UTFT_setBackColorW(VGA_BLACK);

    {
        int x_after_freq_min = cx*11;
        int x_before_freq_max = UTFT_getDisplayXSize()-cx*10;
        int x_after_freq_bottom = cx*7;
        X_SEL = UTFT_getDisplayXSize()-cx*4; //Надписи выбора математической модели и SER/PAR
        int x_after_sel = UTFT_getDisplayXSize()-cx;

        UTFT_setColorW(BOTTOM_BACK_COLOR);
        UTFT_fillRect(x_after_freq_bottom+cx/2, LINE1_Y, X_SEL-cx/2-1, UTFT_getDisplayYSize()-1);

        UTFT_setColorW(FREQ_BACK_COLOR);
        UTFT_fillRect(0, LINE_TOP_Y, cx-1, LINE_TOP_Y+cy-1); //before top freq min
        UTFT_fillRect(x_after_freq_min, LINE_TOP_Y, x_after_freq_min+cx/2-1, LINE_TOP_Y+cy-1); //after top freq min

        UTFT_fillRect(x_before_freq_max-cx/2, LINE_TOP_Y, x_before_freq_max-1, LINE_TOP_Y+cy-1); //before top freq min

        UTFT_fillRect(0, LINE1_Y, cx-1, UTFT_getDisplayYSize()-1); //before bottom freq
        UTFT_fillRect(x_after_freq_bottom, LINE1_Y, x_after_freq_bottom+cx/2-1, UTFT_getDisplayYSize()-1); //after bottom freq
        UTFT_fillRect(X_SEL-cx/2, LINE1_Y, X_SEL-1, UTFT_getDisplayYSize()-1);
        UTFT_fillRect(x_after_sel, LINE1_Y, UTFT_getDisplayXSize()-1, UTFT_getDisplayYSize()-1);

        UTFT_setColorW(TOP_BACK_COLOR);
        UTFT_fillRect(x_after_freq_min+cx/2, LINE_TOP_Y, x_after_freq_min+cx-1, LINE_TOP_Y+cy-1);
        UTFT_fillRect(x_before_freq_max-cx, LINE_TOP_Y, x_before_freq_max-cx/2-1, LINE_TOP_Y+cy-1);
    }

    PlotLineSetVisible(true);
    SceneGraphDrawFreq();
    SceneGraphDrawFminFmax();
    SceneGraphDrawCorrectionModel();
    SceneGraphDrawInfoPanel();

    InterfaceGoto(SceneGraphQuant);
}

void SceneGraphQuant()
{
    if(start_scan_in_device)
    {
        SceneGraphQuantScan();
        return;
    }

    if(EncRValueChanged())
    {
        int delta = EncRValueDelta();
        if(delta<0)
        {
            SceneGraphSelectStart();
        } else
        {
            SceneGraphParamsStart();
        }
        return;
    }

    if(EncLValueChangedSub())
    {
        int delta = EncLValueDeltaSub();

        UTFT_setFont(FONT8x15);
        UTFT_setColorW(VGA_YELLOW);
        UTFT_setBackColorW(VGA_BLACK);

        int x = PlotLinePos();
        x += delta;
        PlotLineSetPos(x);
        SceneGraphDrawFreq();
        SceneGraphDrawInfoPanel();
    }

    if(EncLButtonPressed() && JobState()==JOB_NONE)
    {
        SceneGraphStartScan();
    }


    if(EncRButtonPressed())
    {
        SceneMenuStart();
    }
}

void SceneGraphDrawFreq()
{
    //Частота занимает 6 знакомест на двух строках внизу.
    UTFT_setFont(FONT8x15);
    UTFT_setBackColorW(FREQ_BACK_COLOR);
    freq_type freq = PlotLineVisible()?PlotLineXround():0;
    int cx = UTFT_getFontXsize();
    int xstart = cx;

    int freqMHz = freq/1e6f;
    int freqKHz = (freq-freqMHz*1e6f)/1e3f;
    int freqHz = freq-freqMHz*1e6f-freqKHz*1e3f;

    UTFT_setColorW(VGA_GREEN);
    if(freqMHz>0)
        UTFT_printNumI(freqMHz, xstart, LINE1_Y, 3, ' ');
    else
        UTFT_print("   ", xstart, LINE1_Y);

    UTFT_setColorW(VGA_WHITE);
    UTFT_printNumI(freqKHz, xstart+3*cx, LINE1_Y, 3, freqMHz==0?' ':'0');

    UTFT_setColorW(VGA_WHITE);
    UTFT_printNumI(freqHz, xstart, LINE2_Y, 3, '0');
    UTFT_print(" Hz", xstart+3*cx, LINE2_Y);
}

//Рисует 123456 KHz 10 символов суммарно
static void SceneGraphFreq(uint32_t freq, int x, int y)
{
    int freqMHz = freq/1000000;
    int freqKHz = (freq-freqMHz*1000000)/1000;

    int cx = UTFT_getFontXsize();
    UTFT_setColorW(VGA_GREEN);
    if(freqMHz>0)
        UTFT_printNumI(freqMHz, x, y, 3, ' ');
    else
        UTFT_print("   ", x, y);

    UTFT_setColorW(VGA_WHITE);
    UTFT_printNumI(freqKHz, x+3*cx, y, 3, freqMHz==0?' ':'0');
    UTFT_print(" KHz", x+6*cx, y);
}

void SceneGraphDrawFminFmax()
{
    uint32_t fmin = 0;
    uint32_t fmax = 0;

    if(raw.s_param_count>0)
    {
        fmin = raw.s_param[0].freq;
        fmax = raw.s_param[raw.s_param_count-1].freq;
    } else
    {
        fmin = g_min_freq_khz * 1000;
        fmax = g_max_freq_khz * 1000;
    }

    UTFT_setFont(FONT8x15);
    UTFT_setBackColorW(FREQ_BACK_COLOR);

    int cx = UTFT_getFontXsize();
    SceneGraphFreq(fmin, cx, LINE_TOP_Y);
    SceneGraphFreq(fmax, UTFT_getDisplayXSize()-cx*10, LINE_TOP_Y);
}

void SceneGraphDrawName()
{
#define GRAPH_NAME_LEN 17
    char name[GRAPH_NAME_LEN+1];
    memset(name, ' ', GRAPH_NAME_LEN);
    name[GRAPH_NAME_LEN] = 0;

    if(g_graph_name)
    {
        int len = strlen(g_graph_name);
        if(len>GRAPH_NAME_LEN)
            len = GRAPH_NAME_LEN;
        int offset = (GRAPH_NAME_LEN-len)/2;
        memcpy(name+offset, g_graph_name, len);
    }

    int cx = UTFT_getFontXsize();
    UTFT_setFont(FONT8x15);
    UTFT_setBackColorW(TOP_BACK_COLOR);
    UTFT_setColorW(VGA_WHITE);
    UTFT_print(name, 12*cx, LINE_TOP_Y);
#undef GRAPH_NAME_LEN
}

void SceneGraphStartScan()
{
    start_scan_in_device = true;
    job_last_data_index = -1;

    setTX(false);
    RawParamSetFreqList(g_min_freq_khz*1000, g_max_freq_khz*1000, g_scan_points);
    if(g_scan_speed_slow)
        raw.n_mean = 10; //slow and previse scan
    else
        raw.n_mean = 1;

    ProgressSetVisible(true);

    JobStartSampling();
}

void SceneGraphQuantScan()
{
    if(JobCurrentDataIndex()!=job_last_data_index)
    {
        job_last_data_index = JobCurrentDataIndex();
        if(raw.s_param_count > 0)
        {
            if(g_scan_rx_and_tx)
            {
                ProgressSetPos(0.5f*job_last_data_index/(float)raw.s_param_count+(isTX()?0.5f:0.0f));
            } else
            {
                ProgressSetPos(job_last_data_index/(float)raw.s_param_count);
            }
        }
    }

    if(JobState() == JOB_CALCULATING_COMPLETE)
    {
        if(g_scan_rx_and_tx)
        {
            if(!isTX())
            {
                setTX(true);
                JobStartSampling();
                return;
            }
        }

        start_scan_in_device = false;
        JobSetState(JOB_NONE);
        SceneGraphDrawCurrentGraph();
        ProgressSetVisible(false);
        PlotLineSetVisible(true);
        SceneGraphDrawInfoPanel();
        SceneGraphDrawFminFmax();

        //Она должн быть в конце, т.к может запускать новую сцену
        CheckIntermediateFrequency(SceneGraphStart);
    }
}

void SceneGraphDrawCurrentGraph()
{
    switch(g_graph_type)
    {
    case GTME_REF_DB:
        g_graph_name = "dB(REF)";
        DrawRefAmplitudeDB();
        break;
    case GTME_S11:
        g_graph_name = "S11";
        DrawS11();
        break;
    case GTME_ABS_S11:
        g_graph_name = "abs(S11)";
        DrawAbsS11();
        break;
    case GTME_S21:
        g_graph_name = "S21";
        DrawS21();
        break;
    case GTME_DB_S21:
        g_graph_name = "dB(S21)";
        Draw_dB_S21();
        break;
    case GTME_ABS_S21:
        g_graph_name = "abs(S21)";
        DrawAbsS21();
        break;
    case GTME_IQ_BUFFER:
        g_graph_name = "IQ BUFFER";
        DrawIQBuffer(true, true);
        SceneGraphDrawInfoPanel();
        break;
    case GTME_ERROR_S21:
        g_graph_name = "Error dB(S21)";
        DrawS21ErrorDB();
        break;
    case GTME_E30:
        g_graph_name = "dB(e30)";
        Draw_e30DB();
        break;
    case GTME_COUNT:
        break;
    }

    SceneGraphDrawName();
}

void SceneGraphDrawCorrectionModel()
{
    UTFT_setFont(FONT8x15);
    UTFT_setBackColorW(FREQ_BACK_COLOR);
    UTFT_setColorW(VGA_WHITE);

    if(!IsCalibrationValid() && g_correction_model!=CLM_RAW)
    {
        UTFT_setColorW(VGA_RED);
    }

    switch(g_correction_model)
    {
    case CLM_RAW:
        UTFT_print("RAW", X_SEL, LINE1_Y);
        break;
    case CLM_S11:
        UTFT_print("S11", X_SEL, LINE1_Y);
        break;
    case CLM_RECIPROCAL_AND_SYMMETRYCAL:
        UTFT_print("R&S", X_SEL, LINE1_Y);
        break;
    case CLM_COUNT:
        break;
    }
}

//Если длинна троки слишком короткая, то добавляет пробелов,
//чтобы strlen(str) == line_len
void AppendLineSpaces(char* str, int line_len)
{
    int len = strlen(str);
    if(len >= line_len)
        return;

    for(int i=len; i<line_len; i++)
        str[i] = ' ';
    str[line_len] = 0;
}

void SceneGraphDrawInfoPanel()
{
    UTFT_setFont(FONT8x15);
    UTFT_setBackColorW(FREQ_BACK_COLOR);
    UTFT_setColorW(VGA_WHITE);
    if(g_info_panel_mode==IPM_S11_RLC_SERIAL)
    {
        UTFT_print("SER", X_SEL, LINE2_Y);
    } else
    if(g_info_panel_mode==IPM_S11_RLC_PARALLEL)
    {
        UTFT_print("PAR", X_SEL, LINE2_Y);
    } else
    {
        UTFT_print("   ", X_SEL, LINE2_Y);
    }

    int cx = UTFT_getFontXsize();
    UTFT_setBackColorW(BOTTOM_BACK_COLOR);
    UTFT_setColorW(VGA_WHITE);

    char line1[INFO_CHARS+1];
    char line2[INFO_CHARS+1];
    memset(line1, ' ', INFO_CHARS);
    line1[INFO_CHARS] = 0;
    memset(line2, ' ', INFO_CHARS);
    line2[INFO_CHARS] = 0;
    int xstart = cx*8;

    if(!PlotLineVisible())
    {
        UTFT_setColorW(VGA_WHITE);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_print(line2, xstart, LINE2_Y);
        return;
    }

    const int places = 4;
    const int minwidth = places+3;
    freq_type freq = PlotLineXround();
    complexf S11, S21;
    CalibrationCalculateF(freq, &S11, &S21);

    if(g_graph_type==GTME_ERROR_S21)
    {
        SParam s = RawInterpolate(freq);
        strncpy(line1, "dB(S21 error) =      ", INFO_CHARS);
        floatToStringWithoutZero(line1+15, max_str_len, dB(s.S21err*mul_raw_s21), 2, minwidth, false);

        strncpy(line2, "dB(S21) =      ", INFO_CHARS);
        floatToStringWithoutZero(line2+9, max_str_len, dB(s.S21*mul_raw_s21), 2, minwidth, false);

        UTFT_setColorW(VGA_RED);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_setColorW(VGA_GREEN);
        UTFT_print(line2, xstart, LINE2_Y);
    } else
    if(g_graph_type==GTME_IQ_BUFFER)
    {
        float freq = MeasureFrequencyRef();
        strncpy(line1, "freq = ", INFO_CHARS);
        floatToStringWithoutZero(line1+7,max_str_len, freq, 2, minwidth, false);

        UTFT_setColorW(VGA_WHITE);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_print(line2, xstart, LINE2_Y);
    } else
    if(g_info_panel_mode==IPM_S11_S21_RAW)
    {
        strncpy(line1, "S11 re=-X.XXXX im=-X.XXXX", INFO_CHARS);
        floatToStringWithE(line1+7, max_str_len, crealf(S11), places, minwidth);
        floatToStringWithE(line1+18, max_str_len, cimagf(S11), places, minwidth);

        if(g_correction_model!=CLM_S11)
        {
            strncpy(line2, "S21 re=-X.XXXX im=-X.XXXX", INFO_CHARS);
            floatToStringWithE(line2+7, max_str_len, crealf(S21), places, minwidth);
            floatToStringWithE(line2+18, max_str_len, cimagf(S21), places, minwidth);
        }

        UTFT_setColorW(raw.s11_valid?VGA_WHITE:VGA_RED);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_setColorW(raw.s21_valid?VGA_WHITE:VGA_RED);
        UTFT_print(line2, xstart, LINE2_Y);
    } else
    if(g_info_panel_mode==IPM_S11_Z)
    {
        complexf Z = CalibrationGtoZ(S11);
        strcpy(line1, "Z(S11) re = ");
        formatR(line1+strlen(line1), crealf(Z));
        AppendLineSpaces(line1, INFO_CHARS);

        strcpy(line2, "Z(S11) im = ");
        formatR(line2+strlen(line2), cimagf(Z));
        AppendLineSpaces(line2, INFO_CHARS);

        UTFT_setColorW(raw.s11_valid?VGA_WHITE:VGA_RED);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_print(line2, xstart, LINE2_Y);
    } else
    if(g_info_panel_mode==IPM_S11_RLC_SERIAL || g_info_panel_mode==IPM_S11_RLC_PARALLEL)
    {
        bool is_serial = g_info_panel_mode==IPM_S11_RLC_SERIAL;
        complexf Z = CalibrationGtoZ(S11);

        float L = 1, C = 1, Rout = 1;
        bool isC = true;
        calculateLC(Z, freq, is_serial,
                    &L, &C, &isC, &Rout);

        strcpy(line1, is_serial?"ESR = ":"EPR = ");
        formatR(line1+strlen(line1), Rout);
        AppendLineSpaces(line1, INFO_CHARS);

        if(isC)
        {
            strcpy(line2, "C =");
            formatC(line2+strlen(line2), C);
        } else
        {
            strcpy(line2, "L =");
            formatL(line2+strlen(line2), L);
        }
        AppendLineSpaces(line2, INFO_CHARS);

        UTFT_setColorW(raw.s11_valid?VGA_WHITE:VGA_RED);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_print(line2, xstart, LINE2_Y);
    } else
    {
        //empty line
        UTFT_setColorW(VGA_WHITE);
        UTFT_print(line1, xstart, LINE1_Y);
        UTFT_print(line2, xstart, LINE2_Y);
        return;
    }

}
