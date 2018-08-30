#include "interface.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "menu.h"
#include "edit_int.h"
#include "data.h"
#include "scene_debug_menu.h"

#include <string.h>

#define CHARS_OFFSET_FREQ 12

#define CHARS_OFFSET_POINTS 15
#define CHARS_SIZE_POINTS 3

void SceneCorrectionModelStart();
void SceneInfoPanelModeStart();

void SceneGraphParamsQuant();
void SceneGraphParamsUpdateScanType();
void SceneGraphParamsUpdateScanPoints();
void SceneGraphParamsUpdateMinFreq();
void SceneGraphParamsUpdateMaxFreq();
void SceneGraphParamsUpdateCorrectionModel();
void SceneGraphParamsUpdateScanSpeed();

enum SceneGraphParamsEnum
{
    SGPE_SCAN_TYPE,
    SGPE_CORRECTION_MODEL,
    SGPE_SCAN_POINTS,
    SGPE_MIN_FREQ,
    SGPE_MAX_FREQ,
    SGPE_INFO_PANEL_MENU,
    SGPE_DEBUG_MENU,
    SGPE_SCAN_SPEED,
};

#define F_MIN_STR "F min = "
#define F_MAX_STR "F max = "
#define SCAN_POINTS_STR "Scan points = "

void SceneGraphParamsStart()
{
    MenuReset2("Scan params");
    MenuAddX2(SGPE_SCAN_TYPE, "Click - change scan type", "RX&TX - scan two ports");
    MenuAddX2(SGPE_CORRECTION_MODEL, "Select correction model", "");
    MenuAdd("Info panel mode", SGPE_INFO_PANEL_MENU);
    MenuAddX2(SGPE_SCAN_POINTS, "Click to edit", "Scan points");
    MenuAddX2(SGPE_MIN_FREQ, "Click to edit", "Minimal scan frequency");
    MenuAddX2(SGPE_MAX_FREQ, "Click to edit", "Maximal scan frequency");
    //MenuAddX2(SGPE_SCAN_SPEED, "Select scan speed", "Slow = 0.1x, Fast = 1x");

    MenuAdd("DEBUG", SGPE_DEBUG_MENU);

    SceneGraphParamsUpdateScanType();
    SceneGraphParamsUpdateMinFreq();
    SceneGraphParamsUpdateMaxFreq();
    SceneGraphParamsUpdateScanPoints();
    SceneGraphParamsUpdateCorrectionModel();
    SceneGraphParamsUpdateScanSpeed();
    MenuRedraw();

    InterfaceGoto(SceneGraphParamsQuant);
}

void SceneGraphParamsQuant()
{
    MenuQuant();
    bool pressed = EncLButtonPressed();

    if(EncRButtonPressed())
    {
        SceneGraphStart();
        return;
    }

    if(MenuData()==SGPE_SCAN_TYPE && pressed)
    {
        g_scan_rx_and_tx = !g_scan_rx_and_tx;
        SceneGraphParamsUpdateScanType();
    }

    if(MenuData()==SGPE_SCAN_SPEED && pressed)
    {
        g_scan_speed_slow = !g_scan_speed_slow;
        SceneGraphParamsUpdateScanSpeed();
    }

    if(MenuData()==SGPE_CORRECTION_MODEL && pressed)
    {
        SceneCorrectionModelStart();
        return;
    }

    if(MenuData()==SGPE_SCAN_POINTS && pressed)
    {
        int x,y;
        MenuXY(&x,&y);
        EditIntSetRange(2, MAX_FREQUENCIES, CHARS_SIZE_POINTS);
        EditIntSetPos(x+UTF_StringWidth(SCAN_POINTS_STR), y);
        EditIntSetFontUTF(g_default_font);
        EditIntSetValue(g_scan_points);
        EditIntDialog();
        g_scan_points = EditIntGetValue();
        SceneGraphParamsUpdateScanPoints();
    }

    if(MenuData()==SGPE_MIN_FREQ && pressed)
    {
        int x,y;
        MenuXY(&x,&y);
        EditIntSetRange(CalibrationMinFreq()/1000, CalibrationMaxFreq()/1000-1, CHARS_SIZE_FREQ);
        EditIntSetPos(x+UTF_StringWidth(F_MIN_STR), y);
        EditIntSetFontUTF(g_default_font);
        EditIntSetValue(g_min_freq_khz);
        EditIntDialog();
        g_min_freq_khz = EditIntGetValue();
        SceneGraphParamsUpdateMinFreq();

        if(g_min_freq_khz>=g_max_freq_khz)
        {
            g_max_freq_khz = g_min_freq_khz+1;
            SceneGraphParamsUpdateMaxFreq();
        }

        raw.s_param_count = 0;
    }

    if(MenuData()==SGPE_MAX_FREQ && pressed)
    {
        int x,y;
        MenuXY(&x,&y);
        EditIntSetRange(CalibrationMinFreq()/1000+1, CalibrationMaxFreq()/1000, CHARS_SIZE_FREQ);
        EditIntSetPos(x+UTF_StringWidth(F_MAX_STR), y);
        EditIntSetFontUTF(g_default_font);
        EditIntSetValue(g_max_freq_khz);
        EditIntDialog();
        g_max_freq_khz = EditIntGetValue();
        SceneGraphParamsUpdateMaxFreq();

        if(g_min_freq_khz>=g_max_freq_khz)
        {
            g_min_freq_khz = g_max_freq_khz-1;
            SceneGraphParamsUpdateMinFreq();
        }

        raw.s_param_count = 0;
    }

    if(MenuData()==SGPE_DEBUG_MENU && pressed)
    {
        SceneDebugMenuStart();
        return;
    }

    if(MenuData()==SGPE_INFO_PANEL_MENU && pressed)
    {
        SceneInfoPanelModeStart();
        return;
    }

}

void SceneGraphParamsUpdateScanType()
{
    MenuSetNameAndUpdate(MenuIndexByData(SGPE_SCAN_TYPE),
                         g_scan_rx_and_tx? "Scan type = RX&TX" : "Scan type = RX");
}

void SceneGraphParamsUpdateScanSpeed()
{
    MenuSetNameAndUpdate(MenuIndexByData(SGPE_SCAN_SPEED),
                         g_scan_speed_slow? "Scan speed = SLOW" : "Scan speed = FAST");
}

void SceneGraphParamsUpdateMinFreq()
{
    char str[STATUSBAR_STR_LEN+1];
    strcpy(str, F_MIN_STR);
    intToString(str+strlen(str), g_min_freq_khz, CHARS_SIZE_FREQ, NUM_SPACE);
    strcat(str, " KHz");
    MenuSetNameAndUpdate(MenuIndexByData(SGPE_MIN_FREQ), str);
}

void SceneGraphParamsUpdateMaxFreq()
{
    char str[STATUSBAR_STR_LEN+1];
    strcpy(str, F_MAX_STR);
    intToString(str+strlen(str), g_max_freq_khz, CHARS_SIZE_FREQ, NUM_SPACE);
    strcat(str, " KHz");
    MenuSetNameAndUpdate(MenuIndexByData(SGPE_MAX_FREQ), str);
}

void SceneGraphParamsUpdateScanPoints()
{
    char str[STATUSBAR_STR_LEN+1];
    strcpy(str, SCAN_POINTS_STR);
    intToString(str+strlen(str), g_scan_points, CHARS_SIZE_POINTS, NUM_SPACE);
    MenuSetNameAndUpdate(MenuIndexByData(SGPE_SCAN_POINTS), str);
}

void SceneGraphParamsUpdateCorrectionModel()
{
    switch(g_correction_model)
    {
    case CLM_RAW:
        MenuSetNameAndUpdate(MenuIndexByData(SGPE_CORRECTION_MODEL), "Calibration = RAW");
        break;
    case CLM_S11:
        MenuSetNameAndUpdate(MenuIndexByData(SGPE_CORRECTION_MODEL), "Calibration = S11");
        break;
    case CLM_RECIPROCAL_AND_SYMMETRYCAL:
        MenuSetNameAndUpdate(MenuIndexByData(SGPE_CORRECTION_MODEL), "Calibration = R&S");
        break;
    case CLM_COUNT:
        break;
    }
}

void SceneCorrectionModelQuant()
{
    MenuQuant();

    bool pressed = EncLButtonPressed() || EncRButtonPressed();
    if(pressed)
    {
        g_correction_model = MenuIndex();
        SceneGraphStart();
        return;
    }
}

void SceneCorrectionModelStart()
{
    MenuReset("Select correction type");
    MenuAdd1("RAW", CLM_RAW, "Not use correction");
    MenuAdd2("S11", CLM_S11, "Open-short-load correction", "S11 (RX port)");
    MenuAdd2("R&S", CLM_RECIPROCAL_AND_SYMMETRYCAL, "SOLT correction", "S11 & S21 (Reciprocal & Symmetrical)");
    MenuSetIndex(MenuIndexByData(g_correction_model));
    MenuRedraw();

    InterfaceGoto(SceneCorrectionModelQuant);
}

void SceneInfoPanelModeQuant()
{
    MenuQuant();

    bool pressed = EncLButtonPressed() || EncRButtonPressed();
    if(pressed)
    {
        g_info_panel_mode = MenuIndex();
        SceneGraphStart();
        return;
    }
}

void SceneInfoPanelModeStart()
{
    MenuReset("Info panel mode");
    MenuRedraw();
    MenuAdd1("S11 & S21", IPM_S11_S21_RAW, "View S11 and S21 params");
    MenuAdd1("Z(S11)", IPM_S11_Z, "Convert S11 to Z");
    MenuAdd2("SERIAL (ESR & L/C)", IPM_S11_RLC_SERIAL, "Convert S11 to serial Z", "ESR and L/C");
    MenuAdd2("PARALLEL (EPR & L/C)", IPM_S11_RLC_PARALLEL, "Convert S11 to parallel Z", "EPR and L/C");
    MenuSetIndex(MenuIndexByData(g_info_panel_mode));
    MenuRedraw();

    InterfaceGoto(SceneInfoPanelModeQuant);
}
