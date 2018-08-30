#include "interface.h"
#include "ili/UTFT.h"
#include "menu.h"
#include "data.h"
#include "ad9958_drv.h"

#include "scene_debug_menu.h"

enum SceneDebugMenuEnum
{
    SDME_RETURN,
    SDME_COPY_OPEN,
    SDME_COPY_SHORT,
    SDME_COPY_LOAD,
    SDME_COPY_THRU,
    SDME_BAD_IF,
};


void SceneDebugMenuQuant();

void SceneDebugMenuStart()
{
    MenuReset("Debug menu");
    if(g_calibration.open_valid)
        MenuAdd1("Copy calibration open", SDME_COPY_OPEN, "Copy S11 open param");
    if(g_calibration.short_valid)
        MenuAdd1("Copy calibration short", SDME_COPY_SHORT, "Copy S11 open param");
    if(g_calibration.load_valid)
        MenuAdd1("Copy calibration load", SDME_COPY_LOAD, "Copy S11 load 50 Om param");
    if(g_calibration.thru_valid)
        MenuAdd1("Copy calibration thru", SDME_COPY_THRU, "Copy S11 & S21 transmission param");
    MenuAdd1("Bad IF", SDME_BAD_IF, "Set bad intermediate frequency");

    MenuAdd1("RETURN", SDME_RETURN, "Return to previous menu");

    MenuRedraw();

    InterfaceGoto(SceneDebugMenuQuant);
}

void SceneDebugMenuQuant()
{
    MenuQuant();

    bool lpressed = EncLButtonPressed();
    bool rpressed = EncRButtonPressed();
    bool pressed = lpressed || rpressed;

    if(MenuData()==SDME_RETURN && pressed)
    {
        SceneGraphParamsStart();
        return;
    }

    if(MenuData()==SDME_COPY_OPEN && pressed)
    {
        RawParamReset(g_calibration.count);
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            raw.s_param[i].freq = e->freq;
            raw.s_param[i].S11 = e->S11_open;
            raw.s_param[i].S21 = e->S21_open;
        }

        SceneGraphStart();
        return;
    }

    if(MenuData()==SDME_COPY_SHORT && pressed)
    {
        RawParamReset(g_calibration.count);
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            raw.s_param[i].freq = e->freq;
            raw.s_param[i].S11 = e->S11_short;
        }

        SceneGraphStart();
        return;
    }

    if(MenuData()==SDME_COPY_LOAD && pressed)
    {
        RawParamReset(g_calibration.count);
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            raw.s_param[i].freq = e->freq;
            raw.s_param[i].S11 = e->S11_load;
        }

        SceneGraphStart();
        return;
    }

    if(MenuData()==SDME_COPY_THRU && pressed)
    {
        RawParamReset(g_calibration.count);
        for(int i=0; i<g_calibration.count; i++)
        {
            CalibrationElem* e = g_calibration.elems + i;
            raw.s_param[i].freq = e->freq;
            raw.s_param[i].S11 = e->S11_thru;
            raw.s_param[i].S21 = e->S21_thru;
        }

        SceneGraphStart();
        return;
    }

    if(MenuData()==SDME_BAD_IF && pressed)
    {
        AD9958_Set_Bad_PLL_MUL();
        SceneGraphStart();
        return;
    }
}
