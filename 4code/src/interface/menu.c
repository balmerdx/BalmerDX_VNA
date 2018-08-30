#include "menu.h"
#include "interface.h"
#include <string.h>

#include "colors.h"

#define MENU_MAX_ELEMENTS 14
#define MENU_BACKGROUND VGA_BLACK
#define MENU_BACKGROUND_HIGLIGHT COLOR_BACKGROUND_DARK_GRAY
#define MENU_X_START 16

typedef struct MenuElem
{
    char name[STATUSBAR_STR_LEN+1];
    char hint1[STATUSBAR_STR_LEN+1];
    char hint2[STATUSBAR_STR_LEN+1];
    int data;
} MenuElem;

static int menu_index = 0;//Текущий выбранный пункт меню
static int menu_elem_count = 0;
static MenuElem elems[MENU_MAX_ELEMENTS];
static bool menu_2line = false;

void MenuReset(const char *head)
{
    menu_elem_count = 0;
    menu_index = 0;
    menu_2line = false;

    HeaderSetText(head);
}

void MenuReset2(const char* head)
{
    menu_elem_count = 0;
    menu_index = 0;
    menu_2line = true;

    HeaderSetText(head);
}


void MenuAdd(const char* name, int data)
{
    MenuAdd1(name, data, "");
}

void MenuAdd1(const char* name, int data, const char* hint1)
{
    MenuAdd2(name, data, hint1, "");
}

void MenuAddX2(int data, const char* hint1, const char* hint2)
{
    MenuAdd2("", data, hint1, hint2);
}

void MenuAdd2(const char* name, int data, const char* hint1, const char* hint2)
{
    if(menu_elem_count >= MENU_MAX_ELEMENTS)
        return;
    MenuElem* e = elems + menu_elem_count;

    strncpy(e->name, name, STATUSBAR_STR_LEN);
    e->data = data;
    strncpy(e->hint1, hint1, STATUSBAR_STR_LEN);
    strncpy(e->hint2, hint2, STATUSBAR_STR_LEN);

    menu_elem_count++;
}

static void MenuElemPos(int idx, int* x, int* y, int* width, int* height)
{
    int y_top = HeaderYEnd();
    *height = UTF_Height();

    if(menu_2line)
    {
        *width = UTFT_getDisplayXSize()/2;
        *x = (idx&1)?*width:0;
        *y = y_top + *height *(idx/2);
    } else
    {
        *width = UTFT_getDisplayXSize();
        *x = 0;
        *y = y_top + *height*idx;
    }
}

void MenuRedrawElem(int idx)
{
    UTF_SetFont(g_default_font);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(idx==menu_index?MENU_BACKGROUND_HIGLIGHT:MENU_BACKGROUND);

    int x, y, width, height;
    MenuElemPos(idx, &x, &y, &width, &height);

    UTFT_fillRectBack(x, y, x+MENU_X_START-1, y+height);
    UTF_DrawStringJustify(x+MENU_X_START, y, elems[idx].name, width-MENU_X_START, UTF_LEFT);
}

void MenuRedraw()
{
    MenuElem* e = elems + menu_index;
    StatusbarSetTextAndRedraw(e->hint1, e->hint2);
    HeaderRedraw();

    for(int i=0; i<menu_elem_count; i++)
        MenuRedrawElem(i);

    UTFT_setColorW(MENU_BACKGROUND);
    int x, y, width, height;
    int ecount = menu_elem_count;
    if(menu_2line)
    {
        if(ecount&1)
        {
            MenuElemPos(ecount, &x, &y, &width, &height);
            UTFT_fillRect(x, y, x+width-1, y+height-1);
            ecount++;
        }
    }

    MenuElemPos(ecount, &x, &y, &width, &height);
    UTFT_fillRect(0, y, UTFT_getDisplayXSize()-1, StatusbarYMin()-1);
}

void MenuQuant()
{
    if(!EncLValueChanged())
        return;

    int prev_menu_index = menu_index;
    AddSaturated(&menu_index, -EncLValueDelta(), menu_elem_count);
    MenuRedrawElem(prev_menu_index);
    MenuRedrawElem(menu_index);

    MenuElem* e = elems + menu_index;
    StatusbarSetTextAndRedraw(e->hint1, e->hint2);
}

int MenuIndex()
{
    return menu_index;
}

void MenuSetIndex(int idx)
{
    if(idx>=menu_elem_count)
        idx = menu_elem_count-1;
    if(idx<0)
        idx = 0;

    menu_index = idx;
}

void MenuXY(int* x, int* y)
{
    int width, height;
    UTF_SetFont(g_default_font);
    MenuElemPos(menu_index, x, y, &width, &height);
    *x += MENU_X_START;
}


int MenuData()
{
    if(menu_index<0 || menu_index>=menu_elem_count)
        return 0;
    return elems[menu_index].data;
}

int MenuIndexByData(int data)
{
    for(int i=0; i<menu_elem_count; i++)
    {
        if(elems[i].data==data)
            return i;
    }

    return -1;
}

void MenuSetNameAndUpdate(int idx, const char* name)
{
    if(idx<0 || idx>=menu_elem_count)
        return;
    MenuElem* e = elems + idx;
    //CopyText(e->name, name, 1);
    strncpy(e->name, name, STATUSBAR_STR_LEN);
    MenuRedrawElem(idx);
}
