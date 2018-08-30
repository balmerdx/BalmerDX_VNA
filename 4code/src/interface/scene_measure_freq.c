#include "scene_measure_freq.h"

#include "font_condensed30.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "interface.h"
#include "edit_int.h"
#include "job.h"
#include "process_sound.h"
#include "measure_freq.h"

static int big_freq_x;
static int big_freq_y;
static int big_freq_width;

static int measure_freq_x;
static int measure_freq_y;
static int measure_freq_width;

static int calc_freq_x;
static int calc_freq_y;
static int calc_freq_width;

static int freq_hz = 10000;
static MeasureFreqData g_freq_data;

void SceneMeasureFreqQuant();
void SceneMeasureNewFreq();
void SceneMeasureSoundCallback(int32_t sampleQ, int32_t sampleI);

void SceneMeasureFreqStart()
{
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_clrScr();
    UTF_SetFont(font_condensed30);

    int x_center = UTFT_getDisplayXSize()/2;
    int y = 50;
    big_freq_x = x_center;
    big_freq_y = y;
    big_freq_width = UTF_StringWidth("000000000");
    y += UTF_Height();
    measure_freq_x = x_center;
    measure_freq_y = y;
    measure_freq_width = UTF_StringWidth("0000.000");
    y += UTF_Height();
    calc_freq_x = x_center;
    calc_freq_y = y;
    calc_freq_width = UTF_StringWidth("0000.000");
    y += UTF_Height();

    UTF_DrawStringJustify(0, big_freq_y, "AD9958(Hz)=", x_center, UTF_RIGHT);
    UTF_DrawStringJustify(0, measure_freq_y, "Measure(Hz)=", x_center, UTF_RIGHT);
    UTF_DrawStringJustify(0, calc_freq_y, "Calc(Hz)=", x_center, UTF_RIGHT);

    EditIntSetRange(CalibrationMinFreq(), CalibrationMaxFreq(), 9);
    EditIntSetPos(big_freq_x, big_freq_y);
    EditIntSetValue(freq_hz);
    EditIntSetFontUTF(font_condensed30);
    EditIntStart(false);

    SetOnSouncCallback(SceneMeasureSoundCallback);

    SceneMeasureNewFreq();

    InterfaceGoto(SceneMeasureFreqQuant);
}


void SceneMeasureFreqQuant()
{
    EditIntQuant();
    int new_freq_hz = EditIntGetValue();

    if(freq_hz!=new_freq_hz)
    {
        freq_hz = new_freq_hz;
        SceneMeasureNewFreq();
    }

    if(g_freq_data.count_received_samples%SAMPLE_FREQUENCY==0)
    {
        float value = MeasureFrequencyValue(&g_freq_data);
        UTFT_setColorW(VGA_WHITE);
        UTFT_setBackColorW(VGA_BLACK);
        UTF_SetFont(font_condensed30);
        UTF_printNumF(value, measure_freq_x, measure_freq_y, 4, 8, UTF_LEFT);
    }
}

void SceneMeasureNewFreq()
{
    SetFreqWithCalibration(freq_hz);
    MeasureFrequencyStart(&g_freq_data, 1000, SAMPLE_FREQUENCY);

}

void SceneMeasureSoundCallback(int32_t sampleQ, int32_t sampleI)
{
    MeasureFrequencyQuant(&g_freq_data, sampleQ);
}
