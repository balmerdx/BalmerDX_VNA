#include "interface.h"
#include "ili/UTFT.h"
#include "menu.h"

void SceneGraphSelectQuant();

void SceneGraphSelectStart()
{
    MenuReset2("Select graph menu");
    MenuAdd("S11 real & imag", GTME_S11);
    MenuAdd("abs(S11)", GTME_ABS_S11);

    MenuAdd("S21 real & imag", GTME_S21);
    MenuAdd("dB(S21)", GTME_DB_S21);
    MenuAdd("abs(S21)", GTME_ABS_S21);

    MenuAdd1("dB(REF)", GTME_REF_DB, "Debug graph");
    MenuAdd1("IQ BUFFER", GTME_IQ_BUFFER, "Debug graph");
    MenuAdd1("Error S21", GTME_ERROR_S21, "Debug graph");
    MenuAdd1("Calibration e30", GTME_E30, "Debug graph");



    MenuSetIndex(MenuIndexByData(g_graph_type));
    MenuRedraw();

    InterfaceGoto(SceneGraphSelectQuant);
}

void SceneGraphSelectQuant()
{
    MenuQuant();

    if(EncLButtonPressed())
    {
        g_graph_type = MenuData();
        SceneGraphStart();
        return;
    }

    if(EncRButtonPressed())
    {
        SceneGraphStart();
        return;
    }
}
