#include "main.h"
#include "interface.h"
#include "scene_menu.h"
#include <stm32f4xx_flash.h>
#include <string.h>
#include "ili/UTFT.h"
#include "images/tools_normal.h"
#include "images/tools_selected.h"
#include "images/plot_normal.h"
#include "images/plot_selected.h"
#include "images/voltmeter_normal.h"
#include "images/voltmeter_selected.h"
#include "statusbar.h"
#include "edit_int.h"
#include "scene_single_freq.h"
#include "font_condensed30.h"
#include "scene_measure_freq.h"
#include "scene_settings.h"

uint16_t MENU_BACK_COLOR = VGA_BLACK;

static bool update = false;
static int root_menu_index = RootMenuIndex_Graph;

void SceneMenuQuant();
void SceneMenuDrawCalibration();

void SceneMenuStart()
{
    MENU_BACK_COLOR = VGA_BLACK;

    UTFT_setBackColorW(MENU_BACK_COLOR);
    UTFT_setColorW(VGA_WHITE);
    UTFT_fillRectBack(0, HeaderYEnd(), UTFT_getDisplayXSize()-1, StatusbarYMin()-1);
    HeaderSetTextAndRedraw("BalmerDX VNA");
    SceneMenuDrawCalibration();
    update = true;
    InterfaceGoto(SceneMenuQuant);
}

void SceneMenuQuant()
{
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(MENU_BACK_COLOR);
    if(EncLButtonPressed())
    {
        if(root_menu_index==RootMenuIndex_Graph)
        {
            SceneGraphStart();
            return;
        }

        if(root_menu_index==RootMenuIndex_Tools)
        {
            SceneSettingsStart();
            return;
        }

        if(root_menu_index==RootMenuIndex_SingleFreq)
        {
            SceneSingleFreqStart();
            return;
        }
    }

    if(EncLValueChanged())
    {
        AddSaturated(&root_menu_index, EncLValueDelta(), RootMenuIndex_Count);
        update = true;
    }

    if(!update)
        return;

    int y = (HeaderYEnd()+StatusbarYMin()-plot_normal_img.height)/2;
    int xc = (UTFT_getDisplayXSize()-plot_normal_img.width)/2;
    int xdelta = 130;
    int x[] = {xc-xdelta, xc, xc+xdelta};

    UTFT_drawBitmap4(x[0], y, root_menu_index==RootMenuIndex_Tools?&tools_selected_img:&tools_normal_img);
    UTFT_drawBitmap4(x[1], y, root_menu_index==RootMenuIndex_Graph?&plot_selected_img:&plot_normal_img);
    UTFT_drawBitmap4(x[2], y, root_menu_index==RootMenuIndex_SingleFreq?&voltmeter_selected_img:&voltmeter_normal_img);

    switch(root_menu_index)
    {
    case RootMenuIndex_Tools:
        StatusbarSetTextAndRedraw("Calibration and user settings", "");
        break;
    case RootMenuIndex_Graph:
        StatusbarSetTextAndRedraw("Measure and draw graph", "");
        break;
    case RootMenuIndex_SingleFreq:
        StatusbarSetTextAndRedraw("Measure on single freq", "");
        break;
    }

    update = false;
}

void SceneMenuDrawCalibration()
{
    int y = StatusbarYMin();
    UTF_SetFont(g_default_font);
    y -= UTF_Height();
    char text[64]="Calibration=";

    if(IsCalibrationValid())
    {
        UTFT_setBackColorW(VGA_BLACK);
        strcat(text, g_calibration.name);
    } else
    {
        UTFT_setBackColorW(COLOR_BACKGROUND_RED);
        strcat(text, g_calibration.name);
        strcat(text, " - INVALID");
    }

    UTFT_setColorW(VGA_WHITE);
    UTF_DrawStringJustify(0, y, text, UTFT_getDisplayXSize(), UTF_CENTER);
}
