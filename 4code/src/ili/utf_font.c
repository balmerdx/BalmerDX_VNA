#include "utf_font.h"
#include "UTFT.h"

#include <string.h>

/*
 * Фонт с пропорциональными шрифтами.
 * Может выводить UTF8 символы, если они есть в библиотеке.
 * Сначала идет структура UtfFontHeader
 * Потом UtfFontCharInterval*UtfFontHeader::char_intervals_count
 * Потом UtfFontCharInfo в количестве суммы UtfFontCharInterval::char_count
 * Потом идут данные о буквах. 1 бит на пиксель
 * Данные о буквах смещены на data_offset слов размером uint32_t относительно начала фонта.
*/

typedef struct UtfFontHeader
{
    uint8_t height;// высота буквы
    uint8_t ascent;// высота верхней части буквы
    uint8_t char_intervals_count;//
    uint8_t bits_per_pixel; //1 or 2 bits
} UtfFontHeader;

typedef struct UtfFontCharInterval
{
    uint16_t char_start;// начала интервала буковок в utf16 кодировке
    uint16_t char_count;// количество буковок в этом интервале
} UtfFontCharInterval;

typedef struct UtfFontCharInfo
{
    uint16_t data_offset;
    uint8_t xoffset;
    uint8_t yoffset;
    uint8_t width;
    uint8_t height;
    uint8_t xadvance;
    uint8_t pad;
} UtfFontCharInfo;

void UTFT_drawUtfChar1bit(int x, int y, int char_width, int char_height, uint32_t* data, int top_space, int bottom_space);
void UTFT_drawUtfChar2bit(int x, int y, int char_width, int char_height, uint32_t* data, int top_space, int bottom_space, uint16_t palette[4]);
const char* UTF_UTF8to16(const char* in, uint16_t* out_symbol);
void UTF_CalcPalette2bit(uint16_t palette[4]);

extern uint32_t* utf_current_font;//defined in UTFT

UtfFontCharInfo* UTF_FindInfo(uint16_t symbol)
{
    UtfFontHeader* h = (UtfFontHeader*)utf_current_font;

    int idx_char = 0;
    bool found = false;
    for(int it = 0; it < h->char_intervals_count; it++)
    {
        UtfFontCharInterval* interval = it+(UtfFontCharInterval*)(h+1);
        if(symbol >= interval->char_start  && symbol < interval->char_start+interval->char_count)
        {
            idx_char += symbol-interval->char_start;
            found = true;
            break;
        }

        idx_char += interval->char_count;
    }

    if(!found)
    {
        idx_char = 0;
    }

    return idx_char+(UtfFontCharInfo*)(utf_current_font+(sizeof(UtfFontHeader)+sizeof(UtfFontCharInterval)*h->char_intervals_count)/4);

}

void UTF_SetFont(const uint32_t* font)
{
    utf_current_font = (uint32_t*)font;
}

int UTF_DrawString(int x, int y, const char* str)
{
    if(utf_current_font==0)
    {
        if(UTFT_getFont())
        {
            UTFT_print(str, x, y);
            return x+UTF_StringWidth(str);
        }

        return x;
    }

    int prev_dx = 0;
    UtfFontHeader* h = (UtfFontHeader*)utf_current_font;

    uint16_t palette[4];
    if(h->bits_per_pixel==2)
    {
        UTF_CalcPalette2bit(palette);
    }

    while(*str)
    {
        uint16_t cur_char;
        str =  UTF_UTF8to16(str, &cur_char);
        if(cur_char==0)
            continue;

        UtfFontCharInfo* info = UTF_FindInfo(cur_char);

        int dx_fill = prev_dx+info->xoffset;
        if(dx_fill)
        {
            UTFT_fillRectBack(x, y, x+dx_fill-1, y+h->height-1);
            x += dx_fill;
        }

        if(h->bits_per_pixel==2)
        {
            UTFT_drawUtfChar2bit(x, y, info->width, info->height, utf_current_font+info->data_offset, info->yoffset, h->height-info->yoffset-info->height, palette);
        } else
        {
            UTFT_drawUtfChar1bit(x, y, info->width, info->height, utf_current_font+info->data_offset, info->yoffset, h->height-info->yoffset-info->height);
        }
        x += info->width;

        prev_dx = info->xadvance-info->xoffset-info->width;
        if(prev_dx<0)
            prev_dx = 0;

        if(*str==0)
        {
            //last char
            int dx_fill = prev_dx;
            if(dx_fill)
            {
                UTFT_fillRectBack(x, y, x+dx_fill-1, y+h->height-1);
                x += dx_fill;
            }
        }
    }

    return x;
}

int UTF_StringWidth(const char* str)
{
    if(utf_current_font==0)
    {
        if(UTFT_getFont())
            return UTFT_getFontXsize()*strlen(str);
        return 0;
    }

    int sx = 0;
    while(*str)
    {
        uint16_t cur_char;
        str =  UTF_UTF8to16(str, &cur_char);
        if(cur_char==0)
            continue;

        UtfFontCharInfo* info = UTF_FindInfo(cur_char);
        sx += info->xadvance;
    }

    return sx;
}

int UTF_Height()
{
    if(utf_current_font==0)
    {
        if(UTFT_getFont())
            return UTFT_getFontYsize();
        return 0;
    }

    UtfFontHeader* h = (UtfFontHeader*)utf_current_font;
    return h->height;
}

int UTF_Ascent()
{
    if(utf_current_font==0)
    {
        if(UTFT_getFont())
            return UTFT_getFontYsize();
        return 0;
    }

    UtfFontHeader* h = (UtfFontHeader*)utf_current_font;
    return h->ascent;
}

// convert utf8 to utf16
// in - input utf8 string
// out_symbol - output utf16 symbol
// return in + utf8_len(*out_symbol)
// *out_symbol == 0 if symbol not parsed;
const char* UTF_UTF8to16(const char* in, uint16_t* out_symbol)
{
    unsigned int codepoint = 0;
    int following = 0;
    while(*in)
    {
        unsigned char ch = *in++;
        if (ch <= 0x7f)
        {
            codepoint = ch;
            following = 0;
        }
        else if (ch <= 0xbf)
        {
            if (following > 0)
            {
                codepoint = (codepoint << 6) | (ch & 0x3f);
                --following;
            }
        }
        else if (ch <= 0xdf)
        {
            codepoint = ch & 0x1f;
            following = 1;
        }
        else if (ch <= 0xef)
        {
            codepoint = ch & 0x0f;
            following = 2;
        }
        else
        {
            codepoint = ch & 0x07;
            following = 3;
        }

        if (following == 0)
        {
            if (codepoint > 0xffff)
                *out_symbol = 0;
            else
                *out_symbol = codepoint;
            return in;
        }
    }

    *out_symbol = 0;
    return in;
}

//alpha=0 return color0
//alpha=256 return color1
uint16_t UTF_InterpolateColor(uint16_t color0, uint16_t color1, uint16_t alpha)
{
    uint16_t r0 = UTFT_RED(color0);
    uint16_t g0 = UTFT_GREEN(color0);
    uint16_t b0 = UTFT_BLUE(color0);

    uint16_t r1 = UTFT_RED(color1);
    uint16_t g1 = UTFT_GREEN(color1);
    uint16_t b1 = UTFT_BLUE(color1);

    uint16_t r2 = (r0*(256-alpha)+r1*alpha)>>8;
    uint16_t g2 = (g0*(256-alpha)+g1*alpha)>>8;
    uint16_t b2 = (b0*(256-alpha)+b1*alpha)>>8;

    return UTFT_COLOR(r2,g2,b2);
}

void UTF_CalcPalette2bit(uint16_t palette[4])
{
    palette[0] = UTFT_getBackColor();
    palette[3] = UTFT_getColor();

    palette[1] = UTF_InterpolateColor(palette[0], palette[3], 80);
    palette[2] = UTF_InterpolateColor(palette[0], palette[3], 160);
}

int UTF_DrawStringJustify(int x, int y, const char* str, int width, UTF_JUSTIFY justify)
{
    if(utf_current_font==0 && UTFT_getFont()==0)
    {
        return x;
    }

    int dx = UTF_StringWidth(str);
    int height = UTF_Height();
    if(dx>=width)
    {
        return UTF_DrawString(x, y, str);
    }

    switch(justify)
    {
    case UTF_LEFT:
        UTF_DrawString(x, y, str);
        UTFT_fillRectBack(x+dx, y, x+width-1, y+height-1);
        break;
    case UTF_RIGHT:
        {
            int fill_width = width-dx;
            UTFT_fillRectBack(x, y, x+fill_width-1, y+height-1);
            UTF_DrawString(x+fill_width, y, str);
        }
        break;
    case UTF_CENTER:
        {
            int fill_width = (width-dx)/2;
            UTFT_fillRectBack(x, y, x+fill_width-1, y+height-1);
            x += fill_width;
            width -= fill_width;
            UTF_DrawString(x, y, str);
            UTFT_fillRectBack(x+dx, y, x+width-1, y+height-1);
        }
        break;
    }

    return x+width;
}

int UTF_printNumI(long num, int x, int y, int width, UTF_JUSTIFY justify)
{
    char st[27];
    intToString(st, num, 0, ' ');
    return UTF_DrawStringJustify(x, y, st, width, justify);
}

int UTF_printNumF(float value, int x, int y, int places, int width, UTF_JUSTIFY justify)
{
    char st[27];
    floatToString(st, 27, value, places, 0, false);
    return UTF_DrawStringJustify(x, y, st, width, justify);
}
