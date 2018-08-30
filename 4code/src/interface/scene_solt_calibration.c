#include "scene_solt_calibration.h"
#include "menu.h"
#include "ili/UTFT.h"
#include "interface.h"
#include "scene_name.h"
#include "progress_bar.h"
#include "main.h"
#include "scene_settings.h"
#include "scene_check_intermediate_frequency.h"

#include <string.h>

typedef enum CalibrationMenuEnum
{
    CME_NAME,
    CME_OPEN,
    CME_SHORT,
    CME_LOAD,
    CME_THRU,
    CME_SAVE_AND_EXIT,
    CME_DISCARD_AND_EXIT,
    CME_CLEAR,
} CalibrationMenuEnum;

static bool on_name_edit = false;
static int job_last_data_index;
static bool start_scan_in_device;

static void SceneSoltCalibrationQuant();
static void SceneCalibrationRedrawProgress(bool show);
static void SceneRefreshName();
static void SceneCompleteJob(CalibrationMenuEnum en);
static void SceneRefreshCalName(CalibrationMenuEnum en);

void SceneSoltCalibrationStart()
{
    start_scan_in_device = false;

    MenuReset2("Calibration menu");
    MenuAddX2(CME_NAME, "Left press - change calibration name", "Right rotate - change calibration set");
    MenuAdd("CLEAR", CME_CLEAR);
    MenuAddX2(CME_OPEN, "Open RX socket", "");
    MenuAddX2(CME_SHORT, "Short circuit RX", "");
    MenuAddX2(CME_LOAD, "Connect to RX 50 om resistance", "");
    MenuAddX2(CME_THRU, "Connect RX to TX", "");
    MenuAdd1("SAVE AND EXIT", CME_SAVE_AND_EXIT, "Save calibration data to flash");
    MenuAdd1("DISCARD AND EXIT", CME_DISCARD_AND_EXIT, "Restore calibration data from flash");

    if(on_name_edit)
    {
        on_name_edit = false;
        strncpy(g_calibration.name, SceneGetName(), MAX_NAME_LEN);
    }

    SceneRefreshName();
    SceneRefreshCalName(CME_OPEN);
    SceneRefreshCalName(CME_SHORT);
    SceneRefreshCalName(CME_LOAD);
    SceneRefreshCalName(CME_THRU);
    MenuRedraw();

    InterfaceGoto(SceneSoltCalibrationQuant);
}

static void SceneSoltCalibrationQuant()
{
    if(start_scan_in_device)
    {
        if(JobCurrentDataIndex()!=job_last_data_index)
        {
            job_last_data_index = JobCurrentDataIndex();
            if(raw.s_param_count > 0)
            {
                if(MenuData()==CME_OPEN || MenuData()==CME_THRU)
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
            SceneCompleteJob(MenuData());

            if(!CheckIntermediateFrequency(SceneSoltCalibrationStart))
                return;
        }

        EncClear();
        return;
    }

    MenuQuant();

    if(EncLButtonPressed())
    switch(MenuData())
    {
    case CME_NAME:
        {
            SceneSetName(g_calibration.name);
            SceneNameStart();
            on_name_edit = true;
            return;
        }

    case CME_OPEN:
    case CME_SHORT:
    case CME_LOAD:
    case CME_THRU:
        {
            SceneCalibrationRedrawProgress(true);
            setTX(false);
            RawParamSetStandartFreq();
            raw.n_mean = 1;
            JobStartSampling();
            job_last_data_index = -1;
            start_scan_in_device = true;
            return;
        }

   case CME_SAVE_AND_EXIT:
        {
            WriteCalibration();
            SceneSettingsStart();
            return;
        }

   case CME_DISCARD_AND_EXIT:
        {
            ReadCalibration(g_calibration_index);
            SceneSettingsStart();
            return;
        }
    case CME_CLEAR:
        {
            CalibrationClear(&g_calibration);
            g_calibration.name[0] = g_calibration_index+'1';
            g_calibration.name[1] = 0;

            SceneRefreshName();
            SceneRefreshCalName(CME_OPEN);
            SceneRefreshCalName(CME_SHORT);
            SceneRefreshCalName(CME_LOAD);
            SceneRefreshCalName(CME_THRU);
            return;
        }
    }

    if(EncRButtonPressed())
    {
        SceneSettingsStart();
        return;
    }
}

static void SceneCalibrationRedrawProgress(bool show)
{
    uint16_t back_color = UTFT_COLOR(32,32,32);
    UTFT_setFont(FONT8x15);
    UTFT_setBackColorW(back_color);
    int ymin = 150;
    int ymax = ymin+4*UTFT_getFontYsize();

    UTFT_setColorW(show?back_color:VGA_BLACK);
    UTFT_fillRect(0, ymin, UTFT_getDisplayXSize()-1, ymax-1);
    if(!show)
        return;

    UTFT_setColorW(VGA_WHITE);
    int offs = 5;
    UTFT_drawRect(offs, ymin+offs, UTFT_getDisplayXSize()-1-offs, ymax-1-offs);
    UTFT_print("Processing", UTFT_CENTER, ymin+UTFT_getFontYsize());

    ProgressInit(UTFT_getFontXsize(), ymin+2*UTFT_getFontYsize(), UTFT_getDisplayXSize()-1-2*UTFT_getFontXsize(), UTFT_getFontYsize());
    ProgressSetVisible(true);
}

static void SceneRefreshName()
{
    char menu_name[STATUSBAR_STR_LEN+1];
    strncpy(menu_name, "NAME   - ", STATUSBAR_STR_LEN);
    strncat(menu_name, g_calibration.name, STATUSBAR_STR_LEN);

    MenuSetNameAndUpdate(MenuIndexByData(CME_NAME), menu_name);
}

static void SceneRefreshCalName(CalibrationMenuEnum en)
{
    //en==CME_OPEN | CME_SHORT | CME_LOAD | CME_THRU
    switch(en)
    {
    case CME_OPEN:
        MenuSetNameAndUpdate(MenuIndexByData(en), g_calibration.open_valid?"OPEN   - OK   ":"OPEN   - EMPTY");
        return;
    case CME_SHORT:
        MenuSetNameAndUpdate(MenuIndexByData(en), g_calibration.short_valid?"SHORT  - OK   ":"SHORT  - EMPTY");
        return;
    case CME_LOAD:
        MenuSetNameAndUpdate(MenuIndexByData(en), g_calibration.load_valid?"LOAD   - OK   ":"LOAD   - EMPTY");
        return;
    case CME_THRU:
        MenuSetNameAndUpdate(MenuIndexByData(en), g_calibration.thru_valid?"THRU   - OK   ":"THRU   - EMPTY");
        return;
    default:;
    }
}

static void SceneCompleteJob(CalibrationMenuEnum en)
{
    if(en==CME_OPEN || en==CME_THRU)
    {
        if(!isTX())
        {
            setTX(true);
            JobStartSampling();
            return;
        }
    }

    start_scan_in_device = false;
    SceneCalibrationRedrawProgress(false);

    switch(en)
    {
    case CME_OPEN:
        g_calibration.open_valid = true;
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            e->S11_open = raw.s_param[i].S11;
            e->S21_open = raw.s_param[i].S21;
        }
        break;
    case CME_SHORT:
        g_calibration.short_valid = true;
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            e->S11_short = raw.s_param[i].S11;
        }
        break;
    case CME_LOAD:
        g_calibration.load_valid = true;
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            e->S11_load = raw.s_param[i].S11;
        }
        break;
    case CME_THRU:
        g_calibration.thru_valid = true;
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            e->S11_thru = raw.s_param[i].S11;
            e->S21_thru = raw.s_param[i].S21;
        }
        break;
    default:
        return;
    }

    JobSetState(JOB_NONE);
    SceneRefreshCalName(MenuData());
}
