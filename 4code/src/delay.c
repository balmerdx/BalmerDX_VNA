#include "main.h"
#include "delay.h"

void DelayInit()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	TIM_TimeBaseInitTypeDef tim_init;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_Cmd(TIM6, DISABLE);
	TIM_DeInit(TIM6);

	//*2 for x2 timer prescaler
	tim_init.TIM_Prescaler = RCC_Clocks.PCLK1_Frequency*2/1000000-1; //1 us per tick
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 0xFFFF;
	tim_init.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM6, &tim_init);
	TIM_SetCounter(TIM6, 0);
	TIM_Cmd(TIM6, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_Cmd(TIM7, DISABLE);
	TIM_DeInit(TIM7);

	tim_init.TIM_Prescaler = RCC_Clocks.PCLK1_Frequency/4000-1; //0.125 ms per tick
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 0xFFFF;
	tim_init.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM7, &tim_init);
	TIM_SetCounter(TIM7, 0);
	TIM_Cmd(TIM7, ENABLE);
}

void DelayUs(uint16_t countUs)
{
	uint16_t start = TIM6->CNT;
	while(((uint16_t)(TIM6->CNT-start)) < countUs);
}

void DelayMs(uint16_t countMs)
{
	uint16_t start = TIM7->CNT;
	countMs *= 8;
	while(((uint16_t)(TIM7->CNT-start)) < countMs);
}

uint16_t TimeUs()
{
	return TIM6->CNT;
}

uint16_t TimeMs()
{
	return TIM7->CNT / 8;
}

uint16_t TimeMs8()
{
    return TIM7->CNT;
}
