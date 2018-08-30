#pragma once
//edit_int.h
//Редактирует int число
//Пока предполагаем, что оно может быть только положительным.
//EditIntSet надо вызывать до EditIntStart

extern const uint16_t EDIT_INT_BACK_COLOR;
extern const uint16_t EDIT_INT_BACK_COLOR_SELECT;

void EditIntSetRange(int imin, int imax, int chars);
void EditIntSetPos(int x, int y);
void EditIntSetFont(const uint8_t* font);
void EditIntSetFontUTF(const uint32_t* font);
void EditIntSetValue(int value);
int EditIntGetValue();

//Модальный диалог
void EditIntDialog();

//Немодальный вариант, который надо вызывать ручками
void EditIntStart(bool enable_compete);
void EditIntQuant(); //Вызывать, пока !EditIntComplete()
bool EditIntComplete(); //признак завершения немодального диалога.
