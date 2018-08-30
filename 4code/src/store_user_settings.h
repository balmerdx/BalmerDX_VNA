#pragma once

typedef struct UserSettingsStruct
{
    uint16_t calibration_index;
    //graph settings
    uint16_t scan_points;
    uint32_t min_freq_khz;
    uint32_t max_freq_khz;
    uint16_t graph_type;
    uint16_t info_panel_mode;

    uint8_t scan_rx_and_tx;
    uint8_t correction_model;
    uint8_t single_freq_view_mode2[2];

    uint32_t single_freq_khz;
} UserSettingsStruct;


bool StoreUserSettings();
void LoadUserSettings();
