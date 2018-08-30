#include "main.h"
#include "interface.h"


/*
    срабатывание кнопки
    подавляем программный дребезг
    если кнопка нажата, то второй раз не отсылаем, что нажата
*/
typedef struct EncButtonData
{
    //Время предыдущего нажатия кнопки.
    uint16_t last_pressed_ms;
    uint16_t last_unpressed_ms;
    bool is_logical_pessed;
    bool is_hardware_pessed;
} EncButtonData;


static int16_t encValue1 = 0;
static int16_t encValue2 = 0;
SceneQuantHandler g_quant_handler = NULL;

static EncButtonData button1 = {};
static EncButtonData button2 = {};

const uint32_t* g_default_font = NULL;

void InterfaceStart()
{
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_clrScr();

    UTF_SetFont(g_default_font);
    UTFT_setColorW(VGA_WHITE);

    if(0)
    {
        uint16_t startMs = TimeMs();
        UTFT_setColor(255, 255, 255);
        UTFT_setColor(255, 255, 255);
        int y = 0;
        UTFT_setFont(BigFont);
        UTFT_print(" !\"#$%&'()*+,-./", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("0123456789:;<=>?", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("@ABCDEFGHIJKLMNO", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("PQRSTUVWXYZ[\\]^_", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("`abcdefghijklmno", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("pqrstuvwxyz{|}~ ", UTFT_CENTER, y); y+=UTFT_getFontYsize();

        UTFT_setColor(255, 0, 0);
        UTFT_setFont(SmallFont);
        UTFT_print(" !\"#$%&'()*+,-./0123456789:;<=>?", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("`abcdefghijklmnopqrstuvwxyz{|}~ ", UTFT_CENTER, y); y+=UTFT_getFontYsize();

        UTFT_setColor(255, 255, 0);
        UTFT_setFont(FONT8x15);
        UTFT_print(" !\"#$%&'()*+,-./0123456789:;<=>?", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_", UTFT_CENTER, y); y+=UTFT_getFontYsize();
        UTFT_print("`abcdefghijklmnopqrstuvwxyz{|}~ ", UTFT_CENTER, y); y+=UTFT_getFontYsize();

        UTFT_setColor(0, 0, 255);
        UTFT_setFont(SevenSegNumFont);
        UTFT_print("0123456789", UTFT_CENTER, y);

        uint16_t deltaMs = TimeMs()-startMs;
        UTFT_setFont(BigFont);
        UTFT_printNumI(deltaMs, 0, 240-16, 5, ' ');
        while(1);
    }

    encValue1 = QuadEncValue1();
    encValue2 = QuadEncValue2();

    SceneMenuStart();
}

void InterfaceQuant()
{
    if(g_quant_handler)
        (*g_quant_handler)();
}

void InterfaceGoto(SceneQuantHandler handler)
{
    EncClear();
    g_quant_handler = handler;
}

void EncClear()
{
    if(EncLValueChanged())
    {
        EncLValueDelta();
    }

    if(EncRValueChanged())
    {
        EncRValueDelta();
    }

    EncRButtonPressed();
    EncLButtonPressed();
}

bool EncRValueChanged()
{
    return QuadEncValue1()/2 != encValue1;
}

int EncRValueDelta()
{
    int16_t v = QuadEncValue1()/2;
    int16_t delta = v - encValue1;
    encValue1 = v;
    return delta;
}

bool EncLValueChanged()
{
    return QuadEncValue2()/2 != encValue2;
}

int EncLValueDelta()
{
    int16_t v = QuadEncValue2()/2;
    int16_t delta = v - encValue2;
    encValue2 = v;
    return -delta;
}

bool EncLValueChangedSub()
{
    return QuadEncValue2() != encValue2;
}

int EncLValueDeltaSub()
{
    int16_t v = QuadEncValue2();
    int16_t delta = v - encValue2;
    encValue2 = v;
    return -delta;
}

bool EncButtonPressed(EncButtonData* data, bool hardware_pressed)
{
    uint16_t cur_time = TimeMs();
    uint16_t dt_pressed = cur_time-data->last_pressed_ms;
    uint16_t dt_unpressed = cur_time-data->last_unpressed_ms;

    if(hardware_pressed)
    {
        if(!data->is_hardware_pessed)
        {
            data->is_hardware_pessed = true;
            data->last_pressed_ms = cur_time;
        }

        if(!data->is_logical_pessed)
        {
            data->is_logical_pessed = true;
            return true;
        }
    } else
    {
        if(data->is_hardware_pessed)
        {
            data->is_hardware_pessed = false;
            data->last_unpressed_ms = cur_time;
        }

        if(dt_pressed>200 && dt_unpressed>100)
        {
            data->is_logical_pessed = false;
        }
    }

    return false;
}

bool EncRButtonPressed()
{
    return EncButtonPressed(&button1, QuadEncButton1());
}

bool EncLButtonPressed()
{
    return EncButtonPressed(&button2, QuadEncButton2());
}

void AddSaturated(int *value, int add, int max)
{
    *value = (*value + add)%max;
    if(*value<0)
        *value += max;

}
