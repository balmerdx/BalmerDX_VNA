#include "statusbar.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include <string.h>

extern const uint32_t* g_default_font;

static char g_text0[STATUSBAR_STR_LEN+1] = {0};
static char g_text1[STATUSBAR_STR_LEN+1] = {0};
static char g_header_text[STATUSBAR_STR_LEN+1] = {0};

void CopyText(char data[STATUSBAR_STR_LEN+1], const char* text, int start_spaces)
{
    int i=0;
    while(i<start_spaces)
        data[i++] = ' ';
    while(i<STATUSBAR_STR_LEN && *text)
        data[i++] = *text++;
    while(i<STATUSBAR_STR_LEN)
        data[i++] = ' ';
    data[i++] = 0;
}

int StatusbarYMin()
{
    UTF_SetFont(g_default_font);
    return UTFT_getDisplayYSize() - UTF_Height()*2;
}

void StatusbarCopyText(char* data, const char* text)
{
    //CopyText(data, text, 1);
    strncpy(data, text, STATUSBAR_STR_LEN);
}

void StatusbarRedrawX(int idx)
{
    char* text = idx==0?g_text0:g_text1;
    int y = StatusbarYMin();
    y += idx*UTF_Height();
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(STATUSBAR_BACKGROUND);
    //UTFT_print(text, 0, y);
    UTF_DrawStringJustify(0, y, text, UTFT_getDisplayXSize(), UTF_CENTER);
}

void StatusbarSetTextAndRedraw(const char* text0, const char* text1)
{
    StatusbarSetText0(text0);
    StatusbarSetText1(text1);
    StatusbarRedraw();
}

void StatusbarSetText0(const char* text0)
{
    StatusbarCopyText(g_text0, text0);
}

void StatusbarSetText1(const char* text1)
{
    StatusbarCopyText(g_text1, text1);
}

void StatusbarRedraw()
{
    StatusbarRedrawX(0);
    StatusbarRedrawX(1);
}

int HeaderYEnd()
{
    UTF_SetFont(g_default_font);
    return UTF_Height();
}

void HeaderSetText(const char* text)
{
    strncpy(g_header_text, text, STATUSBAR_STR_LEN);
}

void HeaderSetTextAndRedraw(const char* text)
{
    HeaderSetText(text);
    HeaderRedraw();
}

void HeaderRedraw()
{
    UTF_SetFont(g_default_font);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(STATUSBAR_BACKGROUND);
    UTFT_print(g_header_text, 0, 0);

    UTF_DrawStringJustify(0, 0, g_header_text, UTFT_getDisplayXSize(), UTF_CENTER);
}
