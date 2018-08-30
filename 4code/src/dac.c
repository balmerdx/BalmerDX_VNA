//balmer@inbox.ru
//STM32F405+CS4272 DSP module

#include <math.h>
//#include "SysTick/systick.h"
#include "dac.h"


#define DAC_AMPLITUDE 1200

//max 12 khz
#define MIN_SINUS_PERIOD 4

static uint16_t g_dac_amplitude = DAC_AMPLITUDE;
static uint16_t g_sinusBuffer[DAC_BUFFER_SIZE];
static uint32_t SinusBufferSize = DAC_BUFFER_SIZE;
static uint32_t g_dac_period = 0; // * 1/SystemCoreClock sec SystemCoreClock==72000000
float g_sinusBufferFloat[DAC_BUFFER_SIZE];

uint16_t* DacGetBuffer()
{
	return g_sinusBuffer;
}

uint16_t DacGetBufferSize()
{
	return DAC_BUFFER_SIZE;
}

uint16_t DacGetReadPos()
{
	return DAC_BUFFER_SIZE - DMA1_Stream5->NDTR;
}

uint32_t DacPeriod(void)
{
	return g_dac_period;
}

float DacFrequency()
{
	if(g_dac_period==0)
		return 1.0f;
	return SystemCoreClock/(float)g_dac_period;
}

uint32_t DacSamplesPerPeriod(void)
{
	return SinusBufferSize;
}

uint32_t DacSampleTicks(void)
{
	return g_dac_period/SinusBufferSize;
}
/*
static void DacSquareCalculate()
{
	for(int i=0; i<SinusBufferSize; i++)
	{
		if(i<(SinusBufferSize/2))
			g_sinusBuffer[i] = DAC_ZERO+g_dac_amplitude;
		else
			g_sinusBuffer[i] = DAC_ZERO-g_dac_amplitude;
	}
}
*/

void DacSinusCalculate()
{
	float mul = 2*pi/SinusBufferSize;
	for(int i=0; i<SinusBufferSize; i++)
	{
		float s = sin(i*mul);
		g_sinusBufferFloat[i] = s;
		g_sinusBuffer[i] = (uint16_t) lround(s*g_dac_amplitude)+DAC_ZERO;
	}
}

void DacInit(void)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DAC_InitTypeDef DAC_InitStructure;

	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R, DAC_ZERO);
}


void DacSetFrequency(uint32_t frequency)
{
	DacSetPeriod(48000/frequency, DAC_AMPLITUDE);
}

static void DacDeinitDmaAndTimer()
{
	//DMA_DeInit(DMA1_Stream5);
	DMA_Cmd(DMA1_Stream5, DISABLE);
	TIM_Cmd(TIM2, DISABLE);

	//DAC_SetChannel1Data(DAC_Align_12b_R, DAC_ZERO);
}

static void DacInitDmaAndTimer()
{
	uint32_t prescaler;
	uint32_t period;
	prescaler = 1;
	period = (3500)/2;

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&g_sinusBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = SinusBufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA1_Stream5, &DMA_InitStructure);	
	DMA_Cmd(DMA1_Stream5, ENABLE);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_DMACmd(DAC_Channel_1, ENABLE);

	//168 MHz / TIM_Prescaler / TIM_Period
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = period-1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//TIM_SetCounter(TIM2, 0);
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	g_dac_period = period * prescaler * SinusBufferSize;
}

//	sinusPeriod in SystemCoreClock quants
void DacSetPeriod(uint32_t sinusPeriod, uint16_t amplitude)
{
	g_dac_amplitude = amplitude;
	if(sinusPeriod<MIN_SINUS_PERIOD)
		sinusPeriod = MIN_SINUS_PERIOD;
	if(sinusPeriod>DAC_BUFFER_SIZE)
		sinusPeriod = DAC_BUFFER_SIZE;

	DacDeinitDmaAndTimer();


	SinusBufferSize = sinusPeriod;

	DacSinusCalculate();
	//DacSquareCalculate();
	DacInitDmaAndTimer();
}

void DacInitFullBuffer()
{
	SinusBufferSize = DAC_BUFFER_SIZE;
	DacDeinitDmaAndTimer();

	for(int i=0; i<SinusBufferSize; i++)
		g_sinusBuffer[i] = DAC_ZERO;

	DacInitDmaAndTimer();
}


void DacStart()
{
	TIM_Cmd(TIM2, ENABLE);
}
