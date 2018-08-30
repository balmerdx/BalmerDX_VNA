#include "main.h"
#include "interface.h"
#include "colors.h"
#include "measure_freq.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "statusbar.h"

#include "scene_check_intermediate_frequency.h"

static float g_freq = 12345.f;
static int g_current_y = 0;
static const int g_min_x = 50;
static SceneStartHandler g_prev_scene = 0;

static bool IsValidIFreq()
{
    g_freq = MeasureFrequencyRef();

    //Промежуточная частота нормальная
    return g_freq>990 && g_freq<1100;
}

void SceneCheckIntermediateFrequencyQuant()
{
    if(JobState() == JOB_CALCULATING_COMPLETE)
    {
        UTF_DrawString(g_min_x, g_current_y, IsValidIFreq()?"Reset OK.":"Reset FAIL.");
        g_current_y += UTF_Height();

        int x = UTF_DrawString(g_min_x, g_current_y, "IF=");
        x = UTF_printNumF(g_freq, x, g_current_y, 2, 6, UTF_LEFT);
        UTF_DrawString(x, g_current_y, " Hz");
        g_current_y += UTF_Height();

        UTF_DrawString(g_min_x, g_current_y, "Press to exit.");
        g_current_y += UTF_Height();

        JobSetState(JOB_NONE);
    }

    if(JobState() == JOB_NONE)
    if(EncLButtonPressed() || EncRButtonPressed())
    {
        g_prev_scene();
        return;
    }
}

bool CheckIntermediateFrequency(SceneStartHandler prev_scene)
{
    g_prev_scene = prev_scene;

    if(IsValidIFreq())
        return true;

    //Запускаем сцену, которая выведет эту ошибку на экран и попытается переинициализировать DDS
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_setColorW(VGA_WHITE);
    UTFT_fillRectBack(0, HeaderYEnd(), UTFT_getDisplayXSize()-1, UTFT_getDisplayYSize()-1);

    HeaderSetTextAndRedraw("Bad intermediate frequency!");

    g_current_y = HeaderYEnd();

    UTFT_setBackColorW(VGA_BLACK);
    int x = UTF_DrawString(g_min_x, g_current_y, "IF=");
    x = UTF_printNumF(g_freq, x, g_current_y, 2, 6, UTF_LEFT);
    UTF_DrawString(x, g_current_y, " Hz");
    g_current_y += UTF_Height();

    UTF_DrawString(g_min_x, g_current_y, "Trying reset DDS...");
    g_current_y += UTF_Height();

    AD9958_Init();

    DelayMs(50);

    RawParamSetFreq(30000);
    setTX(false);
    JobStartSampling();

    InterfaceGoto(SceneCheckIntermediateFrequencyQuant);
    return false;
}
