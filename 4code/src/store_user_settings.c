#include "store_user_settings.h"
#include "data.h"
#include "main.h"
#include "hardware/store_to_stm32_flash.h"
#include "interface/scene_graph.h"
#include "interface/scene_single_freq.h"

bool StoreUserSettings()
{
    UserSettingsStruct settings;
    settings.calibration_index = g_calibration_index;
    settings.scan_points = g_scan_points;
    settings.min_freq_khz = g_min_freq_khz;
    settings.max_freq_khz = g_max_freq_khz;
    settings.graph_type = g_graph_type;
    settings.info_panel_mode = g_info_panel_mode;
    settings.scan_rx_and_tx = g_scan_rx_and_tx;
    settings.correction_model = g_correction_model;

    for(int i=0;i<2;i++)
        settings.single_freq_view_mode2[i] = g_single_freq_view_mode2[i];
    settings.single_freq_khz = g_single_freq_khz;


    return STM32WriteToFlash(FLASH_SECTOR_USER_SETTINGS, sizeof(settings), &settings);
}

int ClampFreqKhz(int freq)
{
    int min_khz = CalibrationMinFreq()/1000;
    int max_khz = CalibrationMaxFreq()/1000-1;

    if(freq<min_khz)
        freq=min_khz;
    if(freq>max_khz)
        freq=max_khz;
    return freq;
}

void LoadUserSettings()
{
    UserSettingsStruct settings;
    if(!STM32ReadFromFlash(FLASH_SECTOR_USER_SETTINGS, sizeof(settings), &settings))
        return;

    g_calibration_index = settings.calibration_index;
    if(g_calibration_index>=CALIBRATION_COUNT)
        g_calibration_index = 0;

    g_scan_points = settings.scan_points;
    if(g_scan_points<1)
        g_scan_points = 1;
    if(g_scan_points>MAX_FREQUENCIES)
        g_scan_points=MAX_FREQUENCIES;


    g_min_freq_khz = ClampFreqKhz(settings.min_freq_khz);
    g_max_freq_khz = ClampFreqKhz(settings.max_freq_khz);

    g_graph_type = settings.graph_type;
    if(g_graph_type>=GTME_COUNT)
        g_graph_type = GTME_S11;
    g_info_panel_mode = settings.info_panel_mode;
    if(g_info_panel_mode>IPM_COUNT)
        g_info_panel_mode = IPM_S11_S21_RAW;

    g_scan_rx_and_tx = settings.scan_rx_and_tx?true:false;

    if(settings.correction_model>=CLM_COUNT)
        settings.correction_model = CLM_RAW;
    g_correction_model = settings.correction_model;

    for(int i=0;i<2;i++)
    {
        uint8_t v = settings.single_freq_view_mode2[i];
        if(v>=VM_COUNT)
            v = g_single_freq_view_mode2[i];

        g_single_freq_view_mode2[i] = (SingleFreqViewMode)v;
    }

    g_single_freq_khz = ClampFreqKhz(settings.single_freq_khz);
}



