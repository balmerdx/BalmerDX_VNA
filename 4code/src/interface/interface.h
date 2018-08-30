#pragma once

#include "ili/UTFT.h"
#include "ili/utf_font.h"

#include "scene_graph.h"
#include "scene_graph_select.h"
#include "scene_menu.h"
#include "scene_name.h"
#include "scene_graph_params.h"

typedef void (*SceneQuantHandler)();
typedef void (*SceneStartHandler)();

void InterfaceStart();
void InterfaceQuant();

//Очищает все нажатия, произошедшие до этого.
//Нужно вызывать после длительных операций, при которых не требуется пользовательский ввод.
void EncClear();

//R - right encoder
//L - left encoder
//Проверяет, что значение изменилось с предыдущего вызова EncRValueDelta()
bool EncRValueChanged();
//На сколько изменилось положение энкодера
int EncRValueDelta();

bool EncLValueChanged();
int EncLValueDelta();

//Изменения аналогично EncLValueChanged, но с удвоенной точностью
//Когда пользуемся этими функциями, нельзя пользоваться вариантом без Sub.
//Первый вызов EncLValueDeltaSub() или EncLValueDelta() при миксе - вернет мусор.
//Потом начнет правильно работать.
bool EncLValueChangedSub();
int EncLValueDeltaSub();

void InterfaceGoto(SceneQuantHandler handler);

//Нажата ли кнопка?
//Возвращает true только при первом вызове.
//При последующих вызовах будет возвращать false,
//пока кнопка опять не отожмется и не нажмется заново.
bool EncRButtonPressed();
bool EncLButtonPressed();

//Складывает value+=add
//Значение после сложения помещается в интервал 0..max-1
void AddSaturated(int* value, int add, int max);

//Квант, который необходимо вызывать внутри диалогов
void DefaultQuant();

//Фонт, которым рисуются menu/statusbar/
const uint32_t* g_default_font;
