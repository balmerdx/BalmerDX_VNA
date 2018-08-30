#pragma once

typedef enum UTF_JUSTIFY
{
    UTF_LEFT,
    UTF_RIGHT,
    UTF_CENTER,

} UTF_JUSTIFY;


void UTF_SetFont(const uint32_t* font);

//x,y - левый верхний угол строки
//return x+UTF_StringWidth(str)
int UTF_DrawString(int x, int y, const char* str);

//width - ширина заполняемой области
//если текст меньше этой ширины, то дополнительное место заполняется пустотой
//justify - с какой стороны распологается текст в интервале от x до x+width
int UTF_DrawStringJustify(int x, int y, const char* str, int width, UTF_JUSTIFY justify);

//current font string width
int UTF_StringWidth(const char* str);
//current font height
int UTF_Height();

//current font ascent
int UTF_Ascent();

//print int number
//return x+str_width
int UTF_printNumI(long num, int x, int y, int width, UTF_JUSTIFY justify);

//print float number
//return x+str_width
int UTF_printNumF(float value, int x, int y, int places, int width, UTF_JUSTIFY justify);

