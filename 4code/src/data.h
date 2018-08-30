#pragma once

#include <complex.h>

#include "process_sound.h"
#include "interface/plot.h"
#include "interface/scene_name.h"

typedef complex float complexf;
typedef uint64_t freq_type;

//Максимальное количество частот, которые могут быть отсэмплированны
#define MAX_FREQUENCIES 256

typedef enum CorrectionModel
{
    //Калибровка отключенна, данные идут в RAW формате на экран.
    CLM_RAW,
    CLM_S11,
    CLM_RECIPROCAL_AND_SYMMETRYCAL,
    CLM_COUNT
} CorrectionModel;

extern CorrectionModel g_correction_model;

typedef struct SParam
{
    freq_type freq;
    complexf S11;
    complexf S21;

    float S11err;
    float S21err;

    //Амплитуда сигнала в ref канале (для тестового графика)
    float ref_amplitude;
} SParam;

typedef struct CalibrationElem
{
    freq_type freq;
    complexf S11_open;
    complexf S11_short;
    complexf S11_load;

    complexf S11_thru;//thru ==transmission
    complexf S21_thru;
    complexf S21_open;
} CalibrationElem;

//Около 12 кб данных
typedef struct CalibrationData
{
    char name[MAX_NAME_LEN+1];
    int count;
    bool open_valid;
    bool short_valid;
    bool load_valid;
    bool thru_valid;
    CalibrationElem elems[MAX_FREQUENCIES];
} CalibrationData;

typedef struct RawData
{
    SParam s_param[MAX_FREQUENCIES];
    int s_param_count;
    int n_mean; //Количество циклов усреднения
    bool s11_valid;
    bool s21_valid;
} RawData;

extern RawData raw;

extern Point plot_data[PLOT_MAX_POINTS];
extern Point plot_data2[PLOT_MAX_POINTS];

//Текущие коэффициенты калибровки
extern CalibrationData g_calibration;
extern int g_calibration_index;

//Умножаем raw S11, S21 на эти числа, чтобы результаты максимальные были около 1
extern const float mul_raw_s11;
extern const float mul_raw_s21;

//Уровень сигнала, подаваемый на деталь. MAX_RELATIVE_LEVEL_RF 100 - максимальный уровень.
extern uint8_t g_relative_level_rf;

bool IsCalibrationValid();

//index - индекс в массиве raw_s_param
//S11, S21 - уже откалиброванные параметры. Используется калибровка g_calibration.
//Если S21 не рассчитывается, то возвращается S21 = 0.
//Если IsCalibrationValid()==false, то возвращаются RAW данные.
void CalibrationCalculateI(int index, complexf* S11, complexf* S21);
void CalibrationCalculateF(float freq, complexf* S11, complexf* S21);
void CalibrationCalculateS(const SParam* sparam, complexf* S11, complexf* S21);

//Очистить массив raw_s_param
//Установить raw_s_param_count = size
void RawParamReset(int size);

//Заполнить список стандартным рядом частот
void RawParamSetStandartFreq();
void RawParamSetFreqList(float fmin, float fmax, int count);
//Установить одну частоту для сканирования
void RawParamSetFreq(freq_type freq);

//Минимальная и максимальная частота в калибровочных коэффициентах
uint32_t CalibrationMinFreq();
uint32_t CalibrationMaxFreq();

#define CHARS_SIZE_FREQ 6


#define CALIBRATION_COUNT 3
void CalibrationClear(CalibrationData* data);
//Читает в переменные g_calibration и g_calibration_index
void ReadCalibration(int index);

//Пишет g_calibration в g_calibration_index кусок памяти
void WriteCalibration();


float dB(float y);
//Draw
void DrawIQBuffer(bool drawI, bool drawQ);
void DrawRefAmplitudeDB();
void DrawS21ErrorDB();

void DrawS11();
void DrawAbsS11();

void DrawS21();
void Draw_dB_S21();
void DrawAbsS21();
void Draw_e30DB();

