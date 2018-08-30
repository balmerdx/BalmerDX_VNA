// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef _DAC_H_
#define _DAC_H_

#define pi  3.14159f
#define DAC_BUFFER_SIZE 2000
#define DEFAULT_DAC_AMPLITUDE 1200
#define DAC_ZERO 2047


extern float g_sinusBufferFloat[DAC_BUFFER_SIZE];

void DacInit(void);

/*
	Stop DAC and set frequency
*/
void DacSetFrequency(uint32_t frequency);
void DacSetPeriod(uint32_t sinusPeriod, uint16_t amplitude);

/*
	Stop DAC and max buffer size
*/
void DacInitFullBuffer();

//Запуск, до него должно быть вызванно DacSetFrequency или DacSetPeriod или DacInitFullBuffer()
void DacStart();

uint32_t DacPeriod(void);
float DacFrequency();
uint32_t DacSamplesPerPeriod(void);
uint32_t DacSampleTicks(void);

uint16_t* DacGetBuffer();
uint16_t DacGetBufferSize();
//Play position
uint16_t DacGetReadPos();

#endif//_DAC_H_