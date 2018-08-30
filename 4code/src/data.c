#include <stdint.h>
#include "data.h"
#include "ili/UTFT.h"
#include "hardware/store_to_stm32_flash.h"
#include "calibration_solt.h"
#include "main.h"
#include "commands.h"

#include <math.h>
#include <string.h>

uint8_t g_relative_level_rf = MAX_RELATIVE_LEVEL_RF;
CorrectionModel g_correction_model = CLM_RAW;

uint32_t calibration_frequencies[221]={
10000, 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 
20000, 22000, 24000, 26000, 28000, 30000, 32000, 34000, 36000, 38000, 
40000, 42000, 44000, 46000, 48000, 50000, 52000, 54000, 56000, 58000, 
60000, 62000, 64000, 66000, 68000, 70000, 72000, 74000, 76000, 78000, 
80000, 82000, 84000, 86000, 88000, 90000, 92000, 94000, 96000, 98000, 
100000, 110000, 120000, 130000, 140000, 150000, 160000, 170000, 180000, 190000, 
200000, 220000, 240000, 260000, 280000, 300000, 320000, 340000, 360000, 380000, 
400000, 420000, 440000, 460000, 480000, 500000, 520000, 540000, 560000, 580000, 
600000, 620000, 640000, 660000, 680000, 700000, 720000, 740000, 760000, 780000, 
800000, 820000, 840000, 860000, 880000, 900000, 920000, 940000, 960000, 980000, 
1000000, 1100000, 1200000, 1300000, 1400000, 1500000, 1600000, 1700000, 1800000, 1900000, 
2000000, 2200000, 2400000, 2600000, 2800000, 3000000, 3200000, 3400000, 3600000, 3800000, 
4000000, 4200000, 4400000, 4600000, 4800000, 5000000, 5200000, 5400000, 5600000, 5800000, 
6000000, 6200000, 6400000, 6600000, 6800000, 7000000, 7200000, 7400000, 7600000, 7800000, 
8000000, 8200000, 8400000, 8600000, 8800000, 9000000, 9200000, 9400000, 9600000, 9800000, 
10000000, 11000000, 12000000, 13000000, 14000000, 15000000, 16000000, 17000000, 18000000, 19000000, 
20000000, 22000000, 24000000, 26000000, 28000000, 30000000, 32000000, 34000000, 36000000, 38000000, 
40000000, 42000000, 44000000, 46000000, 48000000, 50000000, 52000000, 54000000, 56000000, 58000000, 
60000000, 62000000, 64000000, 66000000, 68000000, 70000000, 72000000, 74000000, 76000000, 78000000, 
80000000, 82000000, 84000000, 86000000, 88000000, 90000000, 92000000, 94000000, 96000000, 98000000, 
100000000, 105000000, 110000000, 115000000, 120000000, 125000000, 130000000, 135000000, 140000000, 145000000, 
150000000, 155000000, 160000000, 165000000, 170000000, 175000000, 180000000, 185000000, 190000000, 195000000, 
200000000};


CalibrationData g_calibration;
int g_calibration_index = 0;

RawData raw =
{
    .s_param_count = 0,
    .n_mean = 1,
    .s11_valid = false,
    .s21_valid = false,
};

void RawParamReset(int size)
{
    if(size<0)
        size = 0;
    if(size>MAX_FREQUENCIES)
        size = MAX_FREQUENCIES;

    raw.s_param_count = size;
    raw.n_mean = 1;
    for(uint32_t i=0; i<raw.s_param_count; i++)
    {
        SParam* p = raw.s_param + i;
        p->freq = 0;
        p->S11 = 0;
        p->S21 = 0;
        p->S11err = 0;
        p->S21err = 0;
        p->ref_amplitude = 1e-7f;
    }
}

uint32_t CalibrationMinFreq()
{
    return calibration_frequencies[0];
}

uint32_t CalibrationMaxFreq()
{
    return calibration_frequencies[sizeof(calibration_frequencies)/sizeof(calibration_frequencies[0])-1];
}


void RawParamSetStandartFreq()
{
    RawParamReset(sizeof(calibration_frequencies)/sizeof(calibration_frequencies[0]));
    for(uint32_t i=0; i<raw.s_param_count; i++)
    {
        raw.s_param[i].freq = calibration_frequencies[i];
    }
}

void RawParamSetFreqList(float fmin, float fmax, int count)
{
    int maxsz = sizeof(calibration_frequencies)/sizeof(calibration_frequencies[0]);
    if(count<2)
        count = 2;
    if(count>maxsz)
        count = maxsz;
    RawParamReset(count);

    float delta = (fmax-fmin)/(count-1);

    for(uint32_t i=0; i<raw.s_param_count; i++)
    {
        raw.s_param[i].freq = delta*i+fmin;
    }
}

void RawParamSetFreq(freq_type freq)
{
    RawParamReset(1);
    raw.s_param[0].freq = freq;
}

void CalibrationClear(CalibrationData* data)
{
    memset(data, 0xFF, sizeof(CalibrationData));
    data->name[0] = 0;
    data->count = sizeof(calibration_frequencies)/sizeof(calibration_frequencies[0]);
    data->open_valid = false;
    data->short_valid = false;
    data->load_valid = false;
    data->thru_valid = false;

    for(int i=0; i<data->count; i++)
    {
        data->elems[i].freq = calibration_frequencies[i];
    }
}

bool IsCalibrationValid()
{
    if(g_calibration.count != sizeof(calibration_frequencies)/sizeof(calibration_frequencies[0]))
        return false;

    return g_calibration.load_valid && g_calibration.open_valid && g_calibration.short_valid && g_calibration.thru_valid;
}

static uint16_t GetFlashSector(int index)
{
    switch(index)
    {
    case 0: return SPI_FLASH_SECTOR_SOLT0;
    case 1: return SPI_FLASH_SECTOR_SOLT1;
    case 2: return SPI_FLASH_SECTOR_SOLT2;
    }

    return 0;
}

void ReadCalibration(int index)
{
    if(index<0 || index>=CALIBRATION_COUNT)
        return;

    g_calibration_index = index;

    bool ok = true;
    if(STM32ReadFromFlash(GetFlashSector(index), sizeof(CalibrationData), &g_calibration))
    {
        ok = g_calibration.count==(sizeof(calibration_frequencies)/sizeof(calibration_frequencies[0]));
    } else
    {
        ok = false;
    }

    if(!ok)
    {
        CalibrationClear(&g_calibration);
        g_calibration.name[0] = '1' + index;
        g_calibration.name[1] = 0;
    }
}

void WriteCalibration()
{
    if(g_calibration_index<0 || g_calibration_index>=CALIBRATION_COUNT)
        return;

    STM32WriteToFlash(GetFlashSector(g_calibration_index), sizeof(CalibrationData), &g_calibration);
}

void CalibrationCalculateS(const SParam* sparam, complexf* S11, complexf* S21)
{
    *S11 = 0;
    *S21 = 0;

    if(!IsCalibrationValid())
    {
        *S11 = sparam->S11*mul_raw_s11;
        *S21 = sparam->S21*mul_raw_s21;
        return;
    }

    switch(g_correction_model)
    {
    case CLM_RAW:
        *S11 = sparam->S11*mul_raw_s11;
        *S21 = sparam->S21*mul_raw_s21;
        break;
    case CLM_S11:
        *S11 = CalibrationCalculateG(sparam->S11, sparam->freq);
        break;
    case CLM_RECIPROCAL_AND_SYMMETRYCAL:
        Calibration_calculateS11_S21(sparam->S11, sparam->S21, sparam->freq, S11, S21);
        break;
    case CLM_COUNT:
        break;
    }

}

void CalibrationCalculateI(int index, complexf* S11, complexf* S21)
{
    CalibrationCalculateS(raw.s_param + index, S11, S21);
}

void CalibrationCalculateF(float freq, complexf* S11, complexf* S21)
{
    SParam p = RawInterpolate(freq);
    CalibrationCalculateS(&p, S11, S21);
}
