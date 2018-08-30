#pragma once
#include "data.h"

//str = 12.3 Ohm
void formatR(char* str, float R);
void formatC(char* str, float C);
void formatL(char* str, float L);

//str_number = 12.3 str_type = mOhm, Ohm, KOhm
void formatR2(char* str_number, char* str_type, float R);
void formatC2(char* str_number, char* str_type, float C);
void formatL2(char* str_number, char* str_type, float L);

int floatToStringWithE(char * outstr, int out_str_len, float value, int places, int minwidth);

void SparamToString(complexf sparam, char* out_re, char* out_im, int outstr_size);
