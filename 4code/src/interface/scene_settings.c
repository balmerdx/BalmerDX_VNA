#include "scene_settings.h"

#include "menu.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "interface.h"
#include "scene_about.h"
#include "scene_solt_calibration.h"
#include "scene_if_calibration.h"
#include "scene_select_calibration.h"

typedef enum SettingsMenuEnum
{
    SME_ABOUT,
    SME_INTERMEDIATE_FREQUENCY_CALIBRATION,
    SME_SOLT_CALIBRATION,
    SME_SELECT_SOLT_CALIBRATION,
} SettingsMenuEnum;

void SceneSettingsQuant();

void SceneSettingsStart()
{
    MenuReset("Settings");
    MenuAdd2("IF Calibration", SME_INTERMEDIATE_FREQUENCY_CALIBRATION, "Intermediate frequency", "calibration");
    MenuAdd2("SOLT Calibration", SME_SOLT_CALIBRATION, "Short Open Load Thru", "calibration");
    MenuAdd2("Select calibration", SME_SELECT_SOLT_CALIBRATION, "Select used", "SOLT calibration.");
    MenuAdd1("About", SME_ABOUT, "Hardware & Software version");
    MenuRedraw();

    InterfaceGoto(SceneSettingsQuant);
}

void SceneSettingsQuant()
{
    MenuQuant();

    if(EncRButtonPressed())
    {
        SceneMenuStart();
        return;
    }

    if(EncLButtonPressed())
    switch(MenuData())
    {
    case SME_ABOUT:
        SceneAboutStart();
        break;
    case SME_INTERMEDIATE_FREQUENCY_CALIBRATION:
        SceneIfCalibrationStart();
        break;
    case SME_SOLT_CALIBRATION:
        SceneSoltCalibrationStart();
        break;
    case SME_SELECT_SOLT_CALIBRATION:
        SceneSelectCalibration();
        break;
    }
}
