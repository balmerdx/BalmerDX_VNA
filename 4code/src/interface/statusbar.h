#pragma once

// Статусбар занимает две нижние строчки.
// Позволяет выводить на них текст.

#include "colors.h"

#define STATUSBAR_BACKGROUND COLOR_BACKGROUND_BLUE
#define STATUSBAR_STR_LEN 40

//Копирует строку, strlen(data)==STATUSBAR_STR_LEN всегда.
//Оставшиеся позиции заполняются пробелами.
//Если text слишком длинный, то он обрезается.
void CopyText(char data[STATUSBAR_STR_LEN+1], const char* text, int start_spaces);

//internal call UTFT_setFont
int StatusbarYMin();

void StatusbarSetTextAndRedraw(const char* text0, const char* text1);

void StatusbarSetText0(const char* text0);
void StatusbarSetText1(const char* text1);

void StatusbarRedraw();


//Заголовок состоит из одной строки.
//Текст рисуется в центре.
//HeaderYEnd - первое значение Y за пределами header
int HeaderYEnd();
void HeaderSetText(const char* text);
void HeaderSetTextAndRedraw(const char* text);
void HeaderRedraw();
