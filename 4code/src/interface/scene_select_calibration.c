#include "scene_select_calibration.h"
#include "menu.h"
#include "interface.h"
#include "ili/UTFT.h"
#include "scene_settings.h"
#include "data.h"
#include "store_user_settings.h"
#include <string.h>

void SceneSelectCalibrationQuant();

void SceneSelectCalibration()
{
    MenuReset("Select SOLT calibration");

    int last_calibration_index = g_calibration_index;
    for(int i=0; i<CALIBRATION_COUNT; i++)
    {
        ReadCalibration(i);
        char name[MAX_NAME_LEN+11];
        strcpy(name, g_calibration.name);

        if(!IsCalibrationValid())
        {
            strcat(name, " - INVALID");
        }

        MenuAdd(name, i);
    }

    MenuSetIndex(last_calibration_index);

    MenuRedraw();
    ReadCalibration(last_calibration_index);
    InterfaceGoto(SceneSelectCalibrationQuant);
}

void SceneSelectCalibrationQuant()
{
    MenuQuant();

    if(EncRButtonPressed())
    {
        SceneSettingsStart();
        return;
    }

    if(EncLButtonPressed())
    {
        ReadCalibration(MenuData());
        StoreUserSettings();
        SceneSettingsStart();
        return;
    }

}
