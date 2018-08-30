#ifndef _JOB_H_
#define _JOB_H_

#include "data.h"
#include "job_state.h"


void JobQuant();

JOB_STATE JobState();
void JobSetState(JOB_STATE state);

//Сэмплирование текущего набора частот.
void JobStartSampling();

//Отсылает последний результат.
void JobSendCalculated();

//Текущий обрабатываем индекс
int JobCurrentDataIndex();

float JobResultFreq();

void SetFreqWithLevel(uint32_t freq, int32_t levelRF, int32_t levelLO);
void SetFreqWithCalibration(freq_type freq);

#endif//_JOB_H_
