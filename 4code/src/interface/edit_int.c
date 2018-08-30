#include "interface.h"
#include "edit_int.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "colors.h"


static int g_imin = 0;
static int g_imax = 100;
static int g_chars = 3;
static int g_pos_x = 0;
static int g_pos_y = 0;

static int g_value = 0;
const uint16_t EDIT_INT_BACK_COLOR = COLOR_BACKGROUND_DARK_GRAY;
const uint16_t EDIT_INT_BACK_COLOR_SELECT = COLOR_BACKGROUND_YELLOW_SELECT;
static int g_char_selected;

static const uint8_t* g_font = 0;
static const uint32_t* g_font_utf = 0;
static bool is_run = false;
static bool g_enable_compete;

void EditIntSetRange(int imin, int imax, int chars)
{
    g_imin = imin;
    g_imax = imax;
    g_chars = chars;
}

void EditIntSetPos(int x, int y)
{
    g_pos_x = x;
    g_pos_y = y;
}

void EditIntSetFont(const uint8_t* font)
{
    g_font = font;
    g_font_utf = 0;
}

void EditIntSetFontUTF(const uint32_t* font)
{
    g_font = 0;
    g_font_utf = font;
}

void EditIntSetValue(int value)
{
    g_value = value;
}

int EditIntGetValue()
{
    return g_value;
}

static void DrawSelectedInt()
{
    char c[2]="0";
    int value = g_value;

    int num_dx = UTF_StringWidth("0");
    int num_dy = UTF_Height();

    for(int i=0; i<g_chars; i++)
    {
        int x = g_pos_x+num_dx*(g_chars-1-i);

        bool isZero = value==0;
        int v = value%10;
        value /=10;

        if(i==g_char_selected)
        {
            UTFT_setBackColorW(EDIT_INT_BACK_COLOR_SELECT);
            UTFT_setColorW(VGA_BLACK);
        } else
        {
            UTFT_setBackColorW(EDIT_INT_BACK_COLOR);
            UTFT_setColorW(VGA_WHITE);
        }

        if(isZero && i!=0)
        {
            UTFT_fillRectBack(x, g_pos_y, x+num_dx-1, g_pos_y+num_dy-1);
        } else
        {
            c[0] = '0' + v;
            UTF_DrawString(x, g_pos_y, c);
        }
    }
}

void EditIntStart(bool enable_compete)
{
    if(g_font==0 && g_font_utf==0)
        return;

    g_enable_compete = enable_compete;

    is_run = true;

    g_char_selected = 0;
    if(g_font)
        UTFT_setFont(g_font);
    else
        UTF_SetFont(g_font_utf);
    UTFT_setColorW(VGA_WHITE);
    DrawSelectedInt();
}

void EditIntQuant()
{
    if(g_font==0 && g_font_utf==0)
        return;
    if(g_font)
        UTFT_setFont(g_font);
    else
        UTF_SetFont(g_font_utf);

    bool update = false;

    if(g_enable_compete)
    if(EncLButtonPressed() || EncRButtonPressed())
    {
        //exit from edit
        is_run = false;
        return;
    }

    if(EncLValueChanged())
    {
        update = true;
        int delta = EncLValueDelta();
        int mul = 1;
        for(int i=0; i<g_char_selected; i++)
            mul*=10;

        g_value += delta*mul;
        if(g_value<g_imin)
            g_value = g_imin;
        if(g_value>g_imax)
            g_value = g_imax;
    }

    if(EncRValueChanged())
    {
        update = true;
        int delta = EncRValueDelta();
        g_char_selected += delta;
        if(g_char_selected<0)
            g_char_selected = 0;
        if(g_char_selected>=g_chars)
            g_char_selected = g_chars-1;
    }

    if(update)
    {
        DrawSelectedInt();
    }
}


void EditIntDialog()
{
    EditIntStart(true);

    while(is_run)
    {
        DefaultQuant();
        EditIntQuant();
    }
}

bool EditIntComplete()
{
    return !is_run;
}
