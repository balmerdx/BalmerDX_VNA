#pragma once

float MeasureFrequency(int32_t* samples, uint32_t samples_count, uint32_t sample_frequency);

//Рассчитать частоту в ref канале
float MeasureFrequencyRef();

typedef struct MeasureFreqData
{
    int32_t count_calculate_mid;
    uint32_t sample_frequency;

    int64_t ysum;
    int32_t ymax;
    int32_t ymin;
    int32_t ymid;

    int32_t count_received_samples;
    int32_t deltam;
    int32_t deltap;

    int tstart;
    int tstop;
    int tcount;

    bool prev_is_inited;
    bool prev_is_plus;
} MeasureFreqData;

//Итеративный расчет частоты
//count_calculate_mid - количестов сэмплов для определения параметров сигнала
void MeasureFrequencyStart(MeasureFreqData* data, int32_t count_calculate_mid, uint32_t sample_frequency);
void MeasureFrequencyQuant(MeasureFreqData* data, int32_t sample);
float MeasureFrequencyValue(MeasureFreqData* data);
