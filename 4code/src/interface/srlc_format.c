#include "srlc_format.h"
#include "ili/float_to_string.h"
#include <string.h>
#include <math.h>

static void floatToStringHelper(char * outstr, float value, int places)
{
    floatToString(outstr, 16, value, places, places+2, true);
}


static void strcat2(char* str_number, char* str_type, const char* str)
{
    if(str_type)
        strcpy(str_type, str);
    else
        strcat(str_number, str);

}

void formatR(char* str, float R)
{
    formatR2(str, 0, R);
}

void formatR2(char* str_number, char* str_type, float R)
{
    float RA = fabsf(R);
/*
    if(RA<1e-2)
    {
        floatToStringHelper(str_number, R*1e3, 3);
        strcat2(str_number, str_type, " mOm");
        return;
    }

    if(RA<1)
    {
        floatToStringHelper(str_number, R*1e3, 2);
        strcat2(str_number, str_type, " mOm");
        return;
    }
*/
    if(RA<1)
    {//Более загрубленный вариант, точнее 1 mOhm все равно нет точночти
        floatToStringHelper(str_number, R*1e3, 0);
        strcat2(str_number, str_type, " mOm");
        return;
    }

    if(RA<1e1)
    {
        floatToStringHelper(str_number, R, 3);
        strcat2(str_number, str_type, " Om");
        return;
    }

    if(RA<1e3)
    {
        floatToStringHelper(str_number, R, 2);
        strcat2(str_number, str_type, " Om");
        return;
    }

    if(RA<8e3)
    {
        floatToStringHelper(str_number, R*1e-3, 3);
        strcat2(str_number, str_type, " KOm");
        return;
    }

    if(RA<2e4)
    {
        floatToStringHelper(str_number, R*1e-3, 2);
        strcat2(str_number, str_type, " KOm");
        return;
    }

    if(RA<1e6)
    {
        floatToStringHelper(str_number, R*1e-3, 1);
        strcat2(str_number, str_type, " KOm");
        return;
    }

    if(RA<1e7)
    {
        floatToStringHelper(str_number, R*1e-6, 3);
        strcat2(str_number, str_type, " MOm");
        return;
    }

    {
        floatToStringHelper(str_number, R*1e-6, 2);
        strcat2(str_number, str_type, " MOm");
        return;
    }
}

void formatC(char* str, float C)
{
    formatC2(str, 0, C);
}

void formatC2(char* str_number, char* str_type, float C)
{
    float CA = fabsf(C);
    if(CA<1e-12)
    {
        floatToStringHelper(str_number, C*1e15, 1);
        strcat2(str_number, str_type, " fF");
        return;
    }

    if(CA<1e-11)
    {
        floatToStringHelper(str_number, C*1e12, 3);
        strcat2(str_number, str_type, " pF");
        return;
    }

    if(CA<1e-9)
    {
        floatToStringHelper(str_number, C*1e12, 1);
        strcat2(str_number, str_type, " pF");
        return;
    }

    if(CA<1e-8)
    {
        floatToStringHelper(str_number, C*1e9, 2);
        strcat2(str_number, str_type, " nF");
        return;
    }

    if(CA<1e-6)
    {
        floatToStringHelper(str_number, C*1e9, 1);
        strcat2(str_number, str_type, " nF");
        return;
    }

    if(CA<1e-5)
    {
        floatToStringHelper(str_number, C*1e6, 2);
        strcat2(str_number, str_type, " uF");
        return;
    }

    if(CA<1e-3)
    {
        floatToStringHelper(str_number, C*1e6, 1);
        strcat2(str_number, str_type, " uF");
        return;
    }

    if(CA<1e-2)
    {
        floatToStringHelper(str_number, C*1e3, 2);
        strcat2(str_number, str_type, " mF");
        return;
    }

    if(CA<1)
    {
        floatToStringHelper(str_number, C*1e3, 1);
        strcat2(str_number, str_type, " mF");
        return;
    }

    if(CA<1e1)
    {
        floatToStringHelper(str_number, C, 2);
        strcat2(str_number, str_type, " F");
        return;
    }

    {
        floatToStringHelper(str_number, C, 1);
        strcat2(str_number, str_type, " F");
        return;
    }
}

void formatL(char* str, float L)
{
    formatL2(str, 0, L);
}

void formatL2(char* str_number, char* str_type, float L)
{
    float LA = fabsf(L);
    if(LA<1e-8)
    {
        floatToStringHelper(str_number, L*1e9, 2);
        strcat2(str_number, str_type, " nH");
        return;
    }

    if(LA<1e-6)
    {
        floatToStringHelper(str_number, L*1e9, 1);
        strcat2(str_number, str_type, " nH");
        return;
    }

    if(LA<1e-5)
    {
        floatToStringHelper(str_number, L*1e6, 2);
        strcat2(str_number, str_type, " uH");
        return;
    }

    if(LA<1e-3)
    {
        floatToStringHelper(str_number, L*1e6, 1);
        strcat2(str_number, str_type, " uH");
        return;
    }

    if(LA<1e-2)
    {
        floatToStringHelper(str_number, L*1e3, 2);
        strcat2(str_number, str_type, " mH");
        return;
    }

    if(LA<1)
    {
        floatToStringHelper(str_number, L*1e3, 1);
        strcat2(str_number, str_type, " mH");
        return;
    }

    if(LA<1e1)
    {
        floatToStringHelper(str_number, L, 2);
        strcat2(str_number, str_type, " H");
        return;
    }

    {
        floatToStringHelper(str_number, L, 1);
        strcat2(str_number, str_type, " H");
        return;
    }
}

//places>3 always
int floatToStringWithE(char * outstr, int out_str_len, float value, int places, int minwidth)
{
    if(fabsf(value)>=1e-2f)
    {
        return floatToStringWithoutZero(outstr, out_str_len, value, places, minwidth, false);
    }

    //exponential form
    places -= 3;
    if(places<0)
        places = 0;

    int exp = 2;
    value *= 100;
    for(;exp<9 && fabsf(value)<1.0f; exp++)
    {
        value *= 10;
    }

    int sz = floatToStringWithoutZero(outstr, out_str_len, value, places, 0, false);
    outstr[sz++] = 'E';
    outstr[sz++] = '-';
    outstr[sz++] = '0'+exp;

    while(sz<minwidth)
        outstr[sz++] = ' ';

    return sz;
}

void SparamToString(complexf sparam, char* out_re, char* out_im, int outstr_size)
{
    float sabs = cabsf(sparam);
    float sre = crealf(sparam);
    float sim = cimagf(sparam);

    int places = 4;
    int minwidth = 7;
    bool rightjustify = false;

    if(sabs<0.02f)
        places = 5;
    else
    if(sabs<0.002f)
        places = 6;

    floatToString(out_re, outstr_size, sre, places, minwidth, rightjustify);
    floatToString(out_im, outstr_size, sim, places, minwidth, rightjustify);
}
