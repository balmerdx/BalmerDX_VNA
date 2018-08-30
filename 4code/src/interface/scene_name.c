#include "scene_name.h"
#include "interface.h"
#include "ili/UTFT.h"
#include "ili/DefaultFonts.h"
#include "delay.h"
#include <string.h>

#include "scene_solt_calibration.h"

static const uint16_t NAME_TOP_BACK_COLOR = VGA_BLACK;
static const uint16_t NAME_TOP_BACK_COLOR_SELECTED = UTFT_COLOR(64, 64, 255);
static const uint16_t NAME_BOTTOM_BACK_COLOR = UTFT_COLOR(49, 38, 27);
static const uint16_t NAME_BOTTOM_BACK_COLOR_SELECTED = UTFT_COLOR(64, 64, 255);

#define KEYBOARD_LINES 3
#define KEYBOARD_ROWS 13

#define KEYBOARD_X 4
#define KEYBOARD_Y0 120
#define KEYBOARD_Y1 127
#define KEYBOARD_DY 40

#define NAME_X 80
#define NAME_Y 64

#define HEAD_Y 20

const char* g_keyboard[KEYBOARD_LINES] =
{
//320/8 = 40 40 = 13*3 + 1
//012345678901234567890123456789012345678
//000111222333444555666777888999AAABBBCCC
 " 0  1  2  3  4  5  6  7  8  9  _  ! <--",
 " A  B  C  D  E  F  G  H  I  K  L  M END",
 " N  O  P  Q  R  S  T  U  V  W  X  Y  Z ",
};

static int cur_line = 0;
static int cur_row = 0;

char g_name_string[MAX_NAME_LEN+1] = {0};
static int cur_edit_pos = 0;

void SceneNameQuant();
void DrawOneChar(int line, int row);
void DrawTopChar(int row);

void SceneNameStart()
{
    UTFT_setColorW(NAME_TOP_BACK_COLOR);
    UTFT_fillRect(0, 0, UTFT_getDisplayXSize()-1, KEYBOARD_Y0-1);
    UTFT_setColorW(NAME_BOTTOM_BACK_COLOR);
    UTFT_fillRect(0, KEYBOARD_Y0, UTFT_getDisplayXSize()-1, UTFT_getDisplayYSize()-1);

    UTFT_setFont(FONT8x15);
    UTFT_setColorW(VGA_YELLOW);
    UTFT_print("Enter calibration name", UTFT_CENTER, HEAD_Y);

    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(NAME_BOTTOM_BACK_COLOR);

    for(int i=0; i<KEYBOARD_LINES; i++)
    {
        UTFT_print(g_keyboard[i], KEYBOARD_X, KEYBOARD_Y1+i*KEYBOARD_DY);
    }

    UTFT_setBackColorW(NAME_BOTTOM_BACK_COLOR_SELECTED);
    DrawOneChar(cur_line, cur_row);

    cur_edit_pos = strlen(g_name_string);
    if(cur_edit_pos==MAX_NAME_LEN)
        cur_edit_pos = MAX_NAME_LEN-1;

    UTFT_setBackColorW(NAME_BOTTOM_BACK_COLOR);
    for(int i=0; i<cur_edit_pos; i++)
        DrawTopChar(i);

    InterfaceGoto(SceneNameQuant);
}

void DrawOneChar(int line, int row)
{
    char str[4];
    for(int i=0; i<3; i++)
        str[i] = g_keyboard[line][i+row*3];
    str[3] = 0;
    UTFT_print(str, KEYBOARD_X + row*UTFT_getFontXsize()*3, KEYBOARD_Y1+line*KEYBOARD_DY);
}

void DrawTopChar(int row)
{
    char str[2];
    str[0] = g_name_string[row];
    str[1] = 0;
    if(str[0]==0)
        str[0] = ' ';
    UTFT_print(str, NAME_X + row*UTFT_getFontXsize(), NAME_Y);
}

void SceneNameQuant()
{
    int prev_edit_pos = cur_edit_pos;

    if(EncLButtonPressed() || EncRButtonPressed())
    {
        const char* c = &g_keyboard[cur_line][cur_row*3];
        if(c[0]==' ')
        {//normal char
            if(cur_edit_pos<MAX_NAME_LEN)
            {
                g_name_string[cur_edit_pos] = c[1];
                g_name_string[cur_edit_pos+1] = 0;
            }

            if(cur_edit_pos<MAX_NAME_LEN-1)
                cur_edit_pos++;

        } else
        {
            if(c[0]=='<')
            {
                //del
                if(g_name_string[cur_edit_pos]==0)
                {
                    if(cur_edit_pos>0)
                        cur_edit_pos--;
                }

                g_name_string[cur_edit_pos] = 0;
            }
            if(c[0]=='E')
            {
                SceneSoltCalibrationStart();
                return;
            }

        }
    }

    int prev_line = cur_line;
    int prev_row = cur_row;

    if(EncLValueChanged())
    {
        AddSaturated(&cur_row, EncLValueDelta(), KEYBOARD_ROWS);
    }

    if(EncRValueChanged())
    {
        AddSaturated(&cur_line, EncRValueDelta(), KEYBOARD_LINES);
    }

    const uint16_t blink_speed = 1000;
    uint16_t blink_time = TimeMs()%blink_speed;
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(blink_time > blink_speed/2?NAME_TOP_BACK_COLOR_SELECTED:NAME_TOP_BACK_COLOR);
    DrawTopChar(cur_edit_pos);

    if(prev_edit_pos!=cur_edit_pos)
    {
        UTFT_setBackColorW(NAME_TOP_BACK_COLOR);
        DrawTopChar(prev_edit_pos);
        //UTFT_print(g_name_string, NAME_X, NAME_Y);
    }

    if(cur_line==prev_line && cur_row==prev_row)
        return;

    UTFT_setBackColorW(NAME_BOTTOM_BACK_COLOR);
    DrawOneChar(prev_line, prev_row);

    UTFT_setBackColorW(NAME_BOTTOM_BACK_COLOR_SELECTED);
    DrawOneChar(cur_line, cur_row);
}

void SceneSetName(const char* name)
{
    strncpy(g_name_string, name, MAX_NAME_LEN);
}

const char* SceneGetName()
{
    return g_name_string;
}
