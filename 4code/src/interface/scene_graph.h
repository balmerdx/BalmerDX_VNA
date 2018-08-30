#pragma once

typedef enum GraphTypeMenuEnum
{
    //debug calibration graphs
    GTME_S11,
    GTME_ABS_S11,

    GTME_S21,
    GTME_DB_S21,
    GTME_ABS_S21,
    GTME_REF_DB,
    GTME_IQ_BUFFER,
    GTME_ERROR_S21,
    GTME_E30,
    GTME_COUNT
} GraphTypeMenuEnum;

typedef enum InfoPanelMode
{
    IPM_S11_S21_RAW,
    IPM_S11_Z,
    IPM_S11_RLC_SERIAL,
    IPM_S11_RLC_PARALLEL,
    IPM_COUNT
} InfoPanelMode;

//Настройки
extern bool g_scan_rx_and_tx;
extern bool g_scan_speed_slow;
extern int g_scan_points;
extern int g_min_freq_khz;
extern int g_max_freq_khz;
extern GraphTypeMenuEnum g_graph_type;
extern InfoPanelMode g_info_panel_mode;

void SceneGraphStart();
