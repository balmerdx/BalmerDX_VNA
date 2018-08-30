#pragma once
#include "statusbar.h"

//Меню состоит из header, пунктов меню и status bar
//Очищает все данные в меню
//head - имя заголовка
void MenuReset(const char* head);

//Меню в два столбика
void MenuReset2(const char* head);

//Перерисовывает все меню. 
//Надо вызывать после того, как добавили все пункты меню.
void MenuRedraw();

//Добавляет пункт меню 
//name - название пункта меню
//data - пользовательские данные
//hint1, hint2 - подсказки в statusbar
void MenuAdd(const char* name, int data);
void MenuAdd1(const char* name, int data, const char* hint1);
void MenuAdd2(const char* name, int data, const char* hint1, const char* hint2);
void MenuAddX2(int data, const char* hint1, const char* hint2);

//Изменяем имя для уже существующего пункта меню
void MenuSetNameAndUpdate(int idx, const char* name);

//Квант в котором происходит перемещение по пунктам меню
//Используется EncLValueChanged()
void MenuQuant();

//Текущее положение в меню.
int MenuIndex();
void MenuSetIndex(int idx);

//Текущий пункт меню, координаты
//x - место откуда начинается текст (перед ним есть еще чутка пустого пространства)
void MenuXY(int* x, int* y);
//user defined цифра, привязанная к текущему пункту меню
int MenuData();


int MenuIndexByData(int data);
