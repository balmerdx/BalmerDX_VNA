#include "scene_if_calibration.h"

#include "interface.h"
#include "edit_int.h"
#include "job.h"
#include "process_sound.h"
#include "measure_freq.h"
#include "scene_settings.h"
#include "statusbar.h"
#include "progress_bar.h"
#include "hardware/store_to_stm32_flash.h"
#include "main.h"

static int measure_freq_x;
static int measure_freq_y;
static int measure_freq_width;

static int calc_freq_x;
static int calc_freq_y;
static int calc_freq_width;
static int32_t last_count_received_samples;
#define MAX_SECONDS_COUNT 120

static MeasureFreqData g_freq_data;

static float g_intermediate_frequency = 1000.0f;

void SceneIfCalibrationQuant();
void SceneIfCalibrationSoundCallback(int32_t sampleQ, int32_t sampleI);
bool StoreToFlashIF(float freq);

void SceneIfCalibrationStart()
{
    HeaderSetTextAndRedraw("Calibration IF");
    StatusbarSetTextAndRedraw("IF - Intermediate frequency. Wait 2 min.", "Press R button to cancel.");

    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_fillRectBack(0, HeaderYEnd(), UTFT_getDisplayXSize()-1, StatusbarYMin()-1);

    int x_center = UTFT_getDisplayXSize()/2;
    int y = 50;
    y += UTF_Height();
    measure_freq_x = x_center;
    measure_freq_y = y;
    measure_freq_width = UTF_StringWidth("0000.0000");
    y += UTF_Height();
    calc_freq_x = x_center;
    calc_freq_y = y;
    calc_freq_width = UTF_StringWidth("0000.000");
    y += UTF_Height();

    UTF_DrawStringJustify(0, measure_freq_y, "Measure(Hz)=", x_center, UTF_RIGHT);

    last_count_received_samples = 0;
    int pborder = 10;
    ProgressInit(pborder, y, UTFT_getDisplayXSize()-pborder*2, 20);
    ProgressSetPos(0);
    ProgressSetVisible(true);

    SetOnSouncCallback(SceneIfCalibrationSoundCallback);

    SetFreqWithCalibration(1234000);
    MeasureFrequencyStart(&g_freq_data, 1000, SAMPLE_FREQUENCY);

    InterfaceGoto(SceneIfCalibrationQuant);
}


void SceneIfCalibrationQuant()
{
    if(g_freq_data.count_received_samples+SAMPLE_FREQUENCY >= last_count_received_samples)
    {
        last_count_received_samples += SAMPLE_FREQUENCY;
        int seconds_count = g_freq_data.count_received_samples/SAMPLE_FREQUENCY;

        float value = MeasureFrequencyValue(&g_freq_data);
        UTFT_setColorW(VGA_WHITE);
        UTFT_setBackColorW(VGA_BLACK);
        UTF_SetFont(g_default_font);
        UTF_printNumF(value, measure_freq_x, measure_freq_y, 4, 8, UTF_LEFT);

        if(seconds_count<MAX_SECONDS_COUNT)
        {
            seconds_count++;
            ProgressSetPos(seconds_count/(float)MAX_SECONDS_COUNT);
        } else
        {
            SetOnSouncCallback(NULL);
            if(StoreToFlashIF(value))
            {
                StatusbarSetTextAndRedraw("Calibration saved", "Press any button to exit");
            } else
            {
                StatusbarSetTextAndRedraw("ERROR", "Store to flash FAILED!");
            }
        }
    }

    if(EncLButtonPressed() || EncRButtonPressed())
    {
        SetOnSouncCallback(NULL);
        SceneSettingsStart();
        return;
    }
}

void SceneIfCalibrationSoundCallback(int32_t sampleQ, int32_t sampleI)
{
    MeasureFrequencyQuant(&g_freq_data, sampleQ);
}

bool StoreToFlashIF(float freq)
{
    g_intermediate_frequency = freq;
    return STM32WriteToFlash(SPI_FLASH_SECTOR_IF, sizeof(freq), &freq);
}

bool ReadIntermediateFrequencyFromFlash()
{
    return STM32ReadFromFlash(SPI_FLASH_SECTOR_IF, sizeof(g_intermediate_frequency), &g_intermediate_frequency);
}

float GetIntermediateFrequency()
{
    return g_intermediate_frequency;
}
