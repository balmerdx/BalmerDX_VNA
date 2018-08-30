#include "scene_single_freq.h"
#include "font_condensed30.h"
#include "font_condensed59.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "colors.h"
#include "interface.h"
#include "edit_int.h"
#include "main.h"
#include "srlc_format.h"
#include "calibration_solt.h"
#include "calculate_lc.h"
#include "scene_check_intermediate_frequency.h"
#include "store_user_settings.h"

#include <string.h>

typedef enum SingleEditElem
{
    SEE_FREQUENCY = 0,
    SEE_CORRECTION,
    SEE_RX_AND_TX,
    SEE_VIEW_MODE0,
    SEE_VIEW_MODE1,
    SEE_COUNT
} SingleEditElem;


extern bool g_scan_rx_and_tx;
int g_single_freq_khz = 10;
SingleFreqViewMode g_single_freq_view_mode2[2] = {VM_S11, VM_S11_Z};

static SingleEditElem see_index = SEE_FREQUENCY;

static int freq_x;
static int freq_y;
static int freq_y_max;
static int freq_width;

static int correction_x;
static int correction_y;
static int correction_width;

static int scan_rx_and_tx_x;
static int scan_rx_and_tx_y;
static int scan_rx_and_tx_width;

static int view_mode_x;
static int view_mode_y;
static int view_mode_width;

//Параметры для вывода блока информации (таковых блоков два)
//Один блок смещен относительно другого на пол экрана по их.
//pb - param block
static int pb_param_width;
static int pb_param_x;
static int pb_param1_y;
static int pb_param2_y;
static int pb_name_x;
static int pb_name_width;
#define PB_BORDER_WIDTH 2
#define PB_BORDER_COLOR0 UTFT_COLOR(128, 160, 128)
#define PB_BORDER_COLOR1 UTFT_COLOR(128, 128, 255)

//#define PB_BACKGROUND_RE UTFT_COLOR(128, 0, 0)
//#define PB_BACKGROUND_IM UTFT_COLOR(0, 128, 0)
#define PB_BACKGROUND_RE UTFT_COLOR(0, 0, 0)
#define PB_BACKGROUND_IM UTFT_COLOR(70, 70, 70)

static int pb_border_x;
static int pb_border_y;
static int pb_border_width;
static int pb_border_height;

static const char* s_freq = "Freq = ";
static const char* s_g_single_freq_khz = " KHz";
#define FONT_OFFSET_30TO59 24

void SceneSingleFreqQuant();
void SceneSingleFreqDrawFreq();
void SceneSingleFreqDrawCorrection();
void SceneSingleFreqDrawScanRxTx();

int SceneSingleFreqValuesOffset(int view_mode_idx);
void SceneSingleFreqDrawBorderRect(int xoffset, uint16_t color);
void SceneSingleFreqDrawViewMode(int view_mode_idx);
void SceneSingleFreqDrawValue(int view_mode_idx);
void SceneSingleFreqDrawNames(int view_mode_idx);
void SceneSingleFreqDrawValue2();
void SceneSingleFreqDrawNames2();
void SceneSingleFreqDrawViewMode2();

void QuantEditFrequency();


freq_type FreqHz()
{
    return 1000*(freq_type)g_single_freq_khz;
}

void SceneSingleFreqStart()
{
    int y;
    UTFT_setColorW(VGA_WHITE);
    UTF_SetFont(font_condensed59);
    y = 5;
    freq_y = y;
    y += UTF_Height();
    freq_y_max = y;

    UTFT_setBackColorW(COLOR_BACKGROUND_BLUE);
    UTFT_fillRectBack(0, 0, UTFT_getDisplayXSize()-1, freq_y_max-1);
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_fillRectBack(0, freq_y_max, UTFT_getDisplayXSize()-1, UTFT_getDisplayYSize()-1);

    int x_center = UTFT_getDisplayXSize()/2;

    UTF_SetFont(font_condensed30);
    y = UTFT_getDisplayYSize()-UTF_Height()-5;

    correction_x = x_center;
    correction_y = y;
    correction_width = x_center;

    scan_rx_and_tx_x = 0;
    scan_rx_and_tx_y = y;
    scan_rx_and_tx_width = correction_width;

    y += UTF_Height();


    y = freq_y_max;
    pb_name_x = PB_BORDER_WIDTH;
    pb_name_width = 54;
    UTF_SetFont(font_condensed30);
    pb_border_x = 0;
    pb_border_y = y;
    pb_border_width = x_center;
    y += PB_BORDER_WIDTH;

    view_mode_x = PB_BORDER_WIDTH;
    view_mode_y = y;
    view_mode_width = pb_border_width-PB_BORDER_WIDTH*2;
    y += UTF_Height();

    UTF_SetFont(font_condensed59);
    pb_param1_y = y;
    y += UTF_Height();
    pb_param2_y = y;
    y += UTF_Height();
    y += PB_BORDER_WIDTH;
    pb_border_height = y-pb_border_y;

    int s_freq_width;
    {
        //Рассчитываем центр строки Freq=000000 KHz
        int width = 0;
        UTF_SetFont(font_condensed30);
        s_freq_width = UTF_StringWidth(s_freq);
        width +=  s_freq_width + UTF_StringWidth(s_g_single_freq_khz);
        UTF_SetFont(font_condensed59);
        freq_width = UTF_StringWidth("000000");
        width += freq_width;

        freq_x = (UTFT_getDisplayXSize()-width)/2+s_freq_width;
    }

    UTFT_setBackColorW(COLOR_BACKGROUND_BLUE);
    UTF_SetFont(font_condensed30);
    UTF_DrawString(freq_x-s_freq_width, freq_y+FONT_OFFSET_30TO59, s_freq);
    UTF_SetFont(font_condensed59);
    freq_width = UTF_StringWidth("000000");
    UTF_SetFont(font_condensed30);
    UTF_DrawString(freq_x+freq_width, freq_y+FONT_OFFSET_30TO59, s_g_single_freq_khz);

    pb_param_x = pb_name_x+pb_name_width;
    pb_param_width = pb_border_width-PB_BORDER_WIDTH*2-pb_name_width;

    SceneSingleFreqDrawFreq();
    SceneSingleFreqDrawCorrection();
    SceneSingleFreqDrawScanRxTx();
    SceneSingleFreqDrawViewMode2();
    SceneSingleFreqDrawNames2();
    SceneSingleFreqDrawBorderRect(SceneSingleFreqValuesOffset(0), PB_BORDER_COLOR0);
    SceneSingleFreqDrawBorderRect(SceneSingleFreqValuesOffset(1), PB_BORDER_COLOR1);

    setTX(false);
    RawParamSetFreq(FreqHz());
    JobStartSampling();

    InterfaceGoto(SceneSingleFreqQuant);
}

void SceneSingleFreqDrawBorderRect(int xoffset, uint16_t color)
{
    UTFT_setColorW(color);
    int xmin = pb_border_x + xoffset;
    int xmax = xmin+pb_border_width;
    int ymax = pb_border_y+pb_border_height;

    //top
    UTFT_fillRect(xmin, pb_border_y, xmax-1, pb_border_y+PB_BORDER_WIDTH-1);

    //bottom
    UTFT_fillRect(xmin, ymax-PB_BORDER_WIDTH, xmax-1, ymax-1);

    //left
    UTFT_fillRect(xmin, pb_border_y+PB_BORDER_WIDTH, xmin+PB_BORDER_WIDTH-1, ymax-1);

    //right
    UTFT_fillRect(xmax-PB_BORDER_WIDTH, pb_border_y+PB_BORDER_WIDTH, xmax-1, ymax-1);

}

//Предполагается, что str, это строчка у которой может быть - вначале
//Если минуса нет, то оставляем под него пустое место.
int DrawNumberMinus(int x, int y, const char* str, int width)
{
    if(str[0]=='-')
    {
        return UTF_DrawStringJustify(x, y, str, width, UTF_LEFT);
    }

    int minus_width = UTF_StringWidth("-");
    UTFT_fillRectBack(x, y, x+minus_width-1, y+UTF_Height()-1);

    return UTF_DrawStringJustify(x+minus_width, y, str, width-minus_width, UTF_LEFT);
}

void DrawNumberType(int x, int y, const char* str_number, const char* str_type, int width)
{
    UTF_SetFont(font_condensed59);
    int height_big = UTF_Height();

    int x1;
    if(str_number[0]=='-')
    {
        x1 = UTF_DrawString(x, y, str_number);
    } else
    {
        int minus_width = UTF_StringWidth("-");
        UTFT_fillRectBack(x, y, x+minus_width-1, y+UTF_Height()-1);

        x1 = UTF_DrawString(x+minus_width, y, str_number);
    }

    UTF_SetFont(font_condensed30);
    int height_small = UTF_Height();
    int yadd = FONT_OFFSET_30TO59;
    int x2 = UTF_DrawString(x1, y+yadd, str_type);

    UTFT_fillRectBack(x1, y, x2-1, y+yadd-1);
    UTFT_fillRectBack(x1, y+yadd+height_small, x2-1, y+height_big-1);

    if(width+x>x2)
    {
        UTFT_fillRectBack(x2, y, width+x-1, y+height_big-1);
    }
}

void SceneSingleFreqQuant()
{

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

        if(!CheckIntermediateFrequency(SceneSingleFreqStart))
            return;

        SceneSingleFreqDrawValue2();

        setTX(false);
        RawParamSetFreq(FreqHz());

        raw.n_mean = 10;

        JobStartSampling();
    }

    if(see_index == SEE_FREQUENCY)
    {
        QuantEditFrequency();
    } else
    if(see_index==SEE_CORRECTION)
    {
        if(EncRValueChanged())
        {
            int idx = g_correction_model;
            AddSaturated(&idx, EncRValueDelta(), CLM_RECIPROCAL_AND_SYMMETRYCAL+1);
            g_correction_model = idx;
            SceneSingleFreqDrawCorrection();
        }
    } else
    if(see_index==SEE_RX_AND_TX)
    {
        if(EncRValueChanged())
        {
            EncRValueDelta();
            g_scan_rx_and_tx = !g_scan_rx_and_tx;
            if(g_correction_model!=CLM_RAW)
            {
                g_correction_model = g_scan_rx_and_tx?CLM_RECIPROCAL_AND_SYMMETRYCAL:CLM_S11;
            }

            SceneSingleFreqDrawScanRxTx();
            SceneSingleFreqDrawCorrection();
            SceneSingleFreqDrawNames2();
        }
    } else
    if(see_index==SEE_VIEW_MODE0 || see_index==SEE_VIEW_MODE1)
    {
        if(EncRValueChanged())
        {
            int view_mode_idx = see_index==SEE_VIEW_MODE0?0:1;
            int idx = g_single_freq_view_mode2[view_mode_idx];
            AddSaturated(&idx, EncRValueDelta(), VM_COUNT);
            g_single_freq_view_mode2[view_mode_idx] = idx;
            SceneSingleFreqDrawViewMode(view_mode_idx);
            SceneSingleFreqDrawNames(view_mode_idx);
        }
    }

    if(EditIntComplete())
    {
        if(EncLValueChanged())
        {
            int idx = see_index;
            AddSaturated(&idx, EncLValueDelta(), SEE_COUNT);
            see_index = idx;

            SceneSingleFreqDrawFreq();
            SceneSingleFreqDrawCorrection();
            SceneSingleFreqDrawScanRxTx();
            SceneSingleFreqDrawViewMode2();
        }
    }

    if(EncRButtonPressed())
    {
        StoreUserSettings();
        JobSetState(JOB_NONE);
        InterfaceGoto(SceneMenuStart);
        return;
    }
}

void SceneSingleFreqDrawFreq()
{
    UTF_SetFont(font_condensed59);
    UTFT_setColorW(VGA_WHITE);
    if(see_index == SEE_FREQUENCY)
        UTFT_setBackColorW(COLOR_BACKGROUND_DARK_GRAY);
    else
        UTFT_setBackColorW(COLOR_BACKGROUND_BLUE);
    UTF_printNumI(g_single_freq_khz, freq_x, freq_y, freq_width, UTF_RIGHT);
}

void SceneSingleFreqDrawCorrection()
{
    UTF_SetFont(font_condensed30);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);
    int x = UTF_DrawString(correction_x, correction_y, "MODEL=");

    if(see_index == SEE_CORRECTION)
        UTFT_setBackColorW(COLOR_BACKGROUND_DARK_GRAY);
    else
        UTFT_setBackColorW(VGA_BLACK);

    const char* model = "";
    switch(g_correction_model)
    {
    case CLM_RAW:
        model = "RAW";
        break;
    case CLM_S11:
        model = "S11";
        break;
    case CLM_RECIPROCAL_AND_SYMMETRYCAL:
        model = "R&S";
        break;
    case CLM_COUNT:
        break;
    }

    UTF_DrawStringJustify(x, correction_y, model, correction_width+correction_x-x, UTF_LEFT);
}

void SceneSingleFreqDrawScanRxTx()
{
    UTF_SetFont(font_condensed30);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);
    int x = UTF_DrawString(scan_rx_and_tx_x, scan_rx_and_tx_y, "SCAN=");

    if(see_index == SEE_RX_AND_TX)
        UTFT_setBackColorW(COLOR_BACKGROUND_DARK_GRAY);
    else
        UTFT_setBackColorW(VGA_BLACK);

    const char* model = g_scan_rx_and_tx?"RX & TX":"RX";
    UTF_DrawStringJustify(x, scan_rx_and_tx_y, model, scan_rx_and_tx_width+scan_rx_and_tx_x-x, UTF_LEFT);
}

void QuantEditFrequency()
{
    if(EditIntComplete())
    {
        if(EncLButtonPressed())
        {
            EditIntSetRange(CalibrationMinFreq()/1000, CalibrationMaxFreq()/1000, CHARS_SIZE_FREQ);
            EditIntSetPos(freq_x, freq_y);
            EditIntSetValue(g_single_freq_khz);
            EditIntSetFontUTF(font_condensed59);
            EditIntStart(true);
        }
    } else
    {
        EditIntQuant();
        g_single_freq_khz = EditIntGetValue();
        if(EditIntComplete())
        {
            SceneSingleFreqDrawFreq();
        }
    }
}

int SceneSingleFreqValuesOffset(int view_mode_idx)
{
    if(view_mode_idx==0)
        return 0;
    return UTFT_getDisplayXSize()/2;
}

void SceneSingleFreqDrawValue2()
{
    SceneSingleFreqDrawValue(0);
    SceneSingleFreqDrawValue(1);
}

void SceneSingleFreqDrawNames2()
{
    SceneSingleFreqDrawNames(0);
    SceneSingleFreqDrawNames(1);
}

void SceneSingleFreqDrawViewMode2()
{
    SceneSingleFreqDrawViewMode(0);
    SceneSingleFreqDrawViewMode(1);
}

void SceneSingleFreqDrawViewMode(int view_mode_idx)
{
    int xoffset = SceneSingleFreqValuesOffset(view_mode_idx);
    SingleFreqViewMode view_mode =  g_single_freq_view_mode2[view_mode_idx];
    UTF_SetFont(font_condensed30);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);

    if(see_index == SEE_VIEW_MODE0 && view_mode_idx==0)
        UTFT_setBackColorW(COLOR_BACKGROUND_DARK_GRAY);
    else
    if(see_index == SEE_VIEW_MODE1 && view_mode_idx==1)
        UTFT_setBackColorW(COLOR_BACKGROUND_DARK_GRAY);
    else
    {
        if(view_mode_idx==0)
            UTFT_setBackColorW(PB_BORDER_COLOR0);
        else
            UTFT_setBackColorW(PB_BORDER_COLOR1);
    }

    const char* mode = "";
    switch(view_mode)
    {
    case VM_S11:
        mode = "S11";
        break;
    case VM_S21:
        mode = "S21";
        break;
    case VM_S11_Z:
        mode = "Z(S11)";
        break;
    case VM_RX_RLC_SERIAL:
        mode = "RLC SERIAL";
        break;
    case VM_RX_RLC_PARALLEL:
        mode = "RLC PARALLEL";
        break;
    case VM_RX_TX_Z:
        mode = "RX-[]-TX Z";
        break;
    case VM_RX_TX_RLC_SERIAL:
        mode = "RX-[]-TX SER";
        break;
    case VM_RX_TX_RLC_PARALLEL:
        mode = "RX-[]-TX PAR";
        break;
    case VM_COUNT:
        break;
    }

    UTF_DrawStringJustify(view_mode_x+xoffset, view_mode_y, mode, view_mode_width, UTF_CENTER);
}


void SceneSingleFreqDrawNames(int view_mode_idx)
{
    int xoffset = SceneSingleFreqValuesOffset(view_mode_idx);
    SingleFreqViewMode view_mode =  g_single_freq_view_mode2[view_mode_idx];
    UTF_SetFont(font_condensed59);
    int yadd_max = UTF_Height();
    UTF_SetFont(font_condensed30);
    int yadd_min = FONT_OFFSET_30TO59+UTF_Height();
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);

    const char* s_param1 = "";
    const char* s_param2 = "";

    if(view_mode == VM_S11 || view_mode == VM_S21 || view_mode == VM_S11_Z || view_mode==VM_RX_TX_Z)
    {
        s_param1 = "RE=";
        s_param2 = "IM=";
    } else
    if(view_mode == VM_RX_RLC_SERIAL || view_mode == VM_RX_TX_RLC_SERIAL)
    {
        s_param1 = "ESR=";
    } else
    if(view_mode == VM_RX_RLC_PARALLEL || view_mode == VM_RX_TX_RLC_PARALLEL)
    {
        s_param1 = "EPR=";
    }

    int xmin = pb_name_x+xoffset;
    int xmax = pb_name_x+xoffset+pb_name_width;
    UTFT_setBackColorW(PB_BACKGROUND_RE);
    UTFT_fillRectBack(xmin, pb_param1_y, xmax-1, pb_param1_y+FONT_OFFSET_30TO59-1);
    UTF_DrawStringJustify(pb_name_x+xoffset, pb_param1_y+FONT_OFFSET_30TO59, s_param1, pb_name_width, UTF_RIGHT);
    UTFT_fillRectBack(xmin, pb_param1_y+yadd_min, xmax-1, pb_param1_y+yadd_max-1);

    UTFT_setBackColorW(PB_BACKGROUND_IM);
    UTFT_fillRectBack(xmin, pb_param2_y, xmax-1, pb_param2_y+FONT_OFFSET_30TO59-1);
    UTF_DrawStringJustify(pb_name_x+xoffset, pb_param2_y+FONT_OFFSET_30TO59, s_param2, pb_name_width, UTF_RIGHT);
    UTFT_fillRectBack(xmin, pb_param2_y+yadd_min, xmax-1, pb_param2_y+yadd_max-1);
}

void SceneSingleFreqDrawValue(int view_mode_idx)
{
    int xoffset = SceneSingleFreqValuesOffset(view_mode_idx);
    SingleFreqViewMode view_mode =  g_single_freq_view_mode2[view_mode_idx];
    UTF_SetFont(font_condensed30);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);

    const int outstr_size = 10;
    char str_re[outstr_size];
    char str_im[outstr_size];
    char str_re_type[outstr_size];
    char str_im_type[outstr_size];
    complexf S11, S21;
    CalibrationCalculateI(0, &S11, &S21);

    if(view_mode==VM_S11)
    {
        SparamToString(S11, str_re, str_im, outstr_size);
        UTF_SetFont(font_condensed59);
        UTFT_setBackColorW(PB_BACKGROUND_RE);
        DrawNumberMinus(pb_param_x+xoffset, pb_param1_y, str_re, pb_param_width);
        UTFT_setBackColorW(PB_BACKGROUND_IM);
        DrawNumberMinus(pb_param_x+xoffset, pb_param2_y, str_im, pb_param_width);
    } else
    if(view_mode==VM_S21)
    {
        UTF_SetFont(font_condensed59);
        if(g_scan_rx_and_tx)
        {
            SparamToString(S21, str_re, str_im, outstr_size);
        } else
        {
            str_re[0] = 0;
            str_im[0] = 0;
        }

        UTFT_setBackColorW(PB_BACKGROUND_RE);
        DrawNumberMinus(pb_param_x+xoffset, pb_param1_y, str_re, pb_param_width);
        UTFT_setBackColorW(PB_BACKGROUND_IM);
        DrawNumberMinus(pb_param_x+xoffset, pb_param2_y, str_im, pb_param_width);
    } else
    if(view_mode==VM_S11_Z || view_mode==VM_RX_TX_Z)
    {
        complexf Z;
        if(view_mode==VM_S11_Z)
            Z = CalibrationGtoZ(S11);
        else
            Z = CalibrationSOLT_calculateZfromS11_S21(S11, S21);

        formatR2(str_re, str_re_type, crealf(Z));
        formatR2(str_im, str_im_type, cimagf(Z));
        UTFT_setBackColorW(PB_BACKGROUND_RE);

        //strcpy(str_re, "-7.777");
        //strcpy(str_re_type, " KOm");

        DrawNumberType(pb_param_x+xoffset, pb_param1_y, str_re, str_re_type, pb_param_width);
        UTFT_setBackColorW(PB_BACKGROUND_IM);
        DrawNumberType(pb_param_x+xoffset, pb_param2_y, str_im, str_im_type, pb_param_width);
    } else
    if(view_mode==VM_RX_RLC_SERIAL || view_mode==VM_RX_RLC_PARALLEL ||
       view_mode==VM_RX_TX_RLC_SERIAL || view_mode==VM_RX_TX_RLC_PARALLEL)
    {
        freq_type freq = FreqHz();
        bool is_serial = view_mode==VM_RX_RLC_SERIAL || view_mode==VM_RX_TX_RLC_SERIAL;
        complexf Z;
        if(view_mode==VM_RX_RLC_SERIAL || view_mode==VM_RX_RLC_PARALLEL)
            Z = CalibrationGtoZ(S11);
        else
            Z = CalibrationSOLT_calculateZfromS11_S21(S11, S21);

        float L = 1, C = 1, Rout = 1;
        bool isC = true;
        calculateLC(Z, freq, is_serial,
                    &L, &C, &isC, &Rout);

        formatR2(str_re, str_re_type, Rout);
        if(isC)
        {
            formatC2(str_im, str_im_type, C);
        } else
        {
            formatL2(str_im, str_im_type, L);
        }

        UTFT_setBackColorW(PB_BACKGROUND_RE);
        DrawNumberType(pb_param_x+xoffset, pb_param1_y, str_re, str_re_type, pb_param_width);
        UTFT_setBackColorW(PB_BACKGROUND_IM);
        UTF_DrawStringJustify(pb_name_x+xoffset, pb_param2_y+FONT_OFFSET_30TO59, isC?"C = " : "L = ", pb_name_width, UTF_RIGHT);
        DrawNumberType(pb_param_x+xoffset, pb_param2_y, str_im, str_im_type, pb_param_width);
    }
}
