#pragma once
#include "data.h"

//12-Term Error Model e parameters
typedef struct EParams
{
    complexf e00;
    complexf e11;
    complexf De;
    complexf e10_01;
    complexf e30;
    complexf e22;
    complexf e10_32;
} EParams;

//GM = S11 измеренное
//freq - частота
//return S11 откорректированный текущими калибровочными коэффициентами
//Это вариант для CorrectionModel == CLM_S11
complexf CalibrationCalculateG(complexf Gm, freq_type freq);

//Это вариант CorrectionModel == CLM_RECIPROCAL_AND_SYMMETRYCAL
void Calibration_calculateS11_S21(complexf S11m, complexf S21m, freq_type freq, complexf* S11, complexf* S21);
SParam RawInterpolate(freq_type freq);

//Переводит S11 параметр в Z параметры, учитывая Zstd
complexf CalibrationGtoZ(complexf G);

EParams calcEParams(freq_type freq);

complexf CalibrationSOLT_calculateZfromS11_S21(complexf S11, complexf S21);
