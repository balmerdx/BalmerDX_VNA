#include "scene_about.h"

#include "menu.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "interface.h"
#include "bootloader_version.h"
#include "scene_settings.h"
#include "main.h"

void SceneAboutQuant();

void SceneAboutStart()
{
    HeaderSetTextAndRedraw("About");
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_setColorW(VGA_BLACK);
    UTFT_fillRect(0, HeaderYEnd(), UTFT_getDisplayXSize()-1, UTFT_getDisplayYSize()-1);
    UTFT_setColorW(VGA_WHITE);


    int xmin = 20;
    int y = HeaderYEnd() + UTF_Height();
    int x = xmin;
    x = UTF_DrawStringJustify(x, y, "Hardware version: ", 0, UTF_LEFT);
    x = UTF_printNumI(BootloaderGetVersion(), x, y, 0, UTF_LEFT);
    y += UTF_Height();

    x = xmin;
    x = UTF_DrawStringJustify(x, y, "Hardware date: ", 0, UTF_LEFT);
    x = UTF_DrawStringJustify(x, y, BootloaderGetDate(), 0, UTF_LEFT);
    y += UTF_Height();

    x = xmin;
    x = UTF_DrawStringJustify(x, y, "Software date: ", 0, UTF_LEFT);
    x = UTF_DrawStringJustify(x, y, __DATE__, 0, UTF_LEFT);
    y += UTF_Height();

    x = xmin;
    x = UTF_DrawStringJustify(x, y, "Intermediate Frequency: ", 0, UTF_LEFT);
    x = UTF_printNumF(GetIntermediateFrequency(), x, y, 4, 0, UTF_LEFT);
    y += UTF_Height();

    InterfaceGoto(SceneAboutQuant);
}

void SceneAboutQuant()
{
    if(EncLButtonPressed() || EncRButtonPressed())
    {
        SceneSettingsStart();
    }
}
