#include "quadrature_encoder.h"


#define SWAP_BUTTONS_AND_ENCODERS

//-------------------------------------------#DEFINE------------------------------------------
#define Codeur_RCC         RCC_AHB1Periph_GPIOA


#ifndef SWAP_BUTTONS_AND_ENCODERS

#define Codeur1_A           GPIO_Pin_0
#define Codeur1_A_SOURCE    GPIO_PinSource0
#define Codeur1_B           GPIO_Pin_1
#define Codeur1_B_SOURCE    GPIO_PinSource1
#define Codeur1_GPIO        GPIOA
#define Codeur1_AF          GPIO_AF_TIM2
#define Codeur1_TIMER       TIM2

#define Codeur2_A           GPIO_Pin_6
#define Codeur2_A_SOURCE    GPIO_PinSource6
#define Codeur2_B           GPIO_Pin_7
#define Codeur2_B_SOURCE    GPIO_PinSource7
#define Codeur2_GPIO        GPIOA
#define Codeur2_AF          GPIO_AF_TIM3
#define Codeur2_TIMER       TIM3

#define BUTTON_PIN1 		GPIO_Pin_2
#define BUTTON_PIN2         GPIO_Pin_5
#define BUTTON_GPIO			GPIOA

#endif// !SWAP_BUTTONS_AND_ENCODERS

#ifdef SWAP_BUTTONS_AND_ENCODERS

#define Codeur2_A           GPIO_Pin_0
#define Codeur2_A_SOURCE    GPIO_PinSource0
#define Codeur2_B           GPIO_Pin_1
#define Codeur2_B_SOURCE    GPIO_PinSource1
#define Codeur2_GPIO        GPIOA
#define Codeur2_AF          GPIO_AF_TIM2
#define Codeur2_TIMER       TIM2

#define Codeur1_A           GPIO_Pin_6
#define Codeur1_A_SOURCE    GPIO_PinSource6
#define Codeur1_B           GPIO_Pin_7
#define Codeur1_B_SOURCE    GPIO_PinSource7
#define Codeur1_GPIO        GPIOA
#define Codeur1_AF          GPIO_AF_TIM3
#define Codeur1_TIMER       TIM3

#define BUTTON_PIN2 		GPIO_Pin_2
#define BUTTON_PIN1         GPIO_Pin_5
#define BUTTON_GPIO			GPIOA

#endif// SWAP_BUTTONS_AND_ENCODERS

void QuadEncInit()
{
    RCC_AHB1PeriphClockCmd(Codeur_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 65535; // Maximal
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    {//Codeur1
        // 2 Inputs for A and B Encoder Channels
        GPIO_InitStructure.GPIO_Pin = Codeur1_A|Codeur1_B;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(Codeur1_GPIO, &GPIO_InitStructure);
     
        //Timer AF Pins Configuration
        GPIO_PinAFConfig(Codeur1_GPIO, Codeur1_A_SOURCE, Codeur1_AF);
        GPIO_PinAFConfig(Codeur1_GPIO, Codeur1_B_SOURCE, Codeur1_AF);
     
        TIM_TimeBaseInit(Codeur1_TIMER, &TIM_TimeBaseStructure);
        // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
        TIM_EncoderInterfaceConfig(Codeur1_TIMER, TIM_EncoderMode_TI2, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
        TIM_Cmd(Codeur1_TIMER, ENABLE);
    }

    {//Codeur2
        // 2 Inputs for A and B Encoder Channels
        GPIO_InitStructure.GPIO_Pin = Codeur2_A|Codeur2_B;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(Codeur2_GPIO, &GPIO_InitStructure);
     
        //Timer AF Pins Configuration
        GPIO_PinAFConfig(Codeur2_GPIO, Codeur2_A_SOURCE, Codeur2_AF);
        GPIO_PinAFConfig(Codeur2_GPIO, Codeur2_B_SOURCE, Codeur2_AF);
     
        TIM_TimeBaseInit(Codeur2_TIMER, &TIM_TimeBaseStructure);
        // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
        TIM_EncoderInterfaceConfig(Codeur2_TIMER, TIM_EncoderMode_TI2, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
        TIM_Cmd(Codeur2_TIMER, ENABLE);
    }

    //button 1 & 2
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN1 | BUTTON_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);

#ifdef DISPLAY_ILI9481
    //PA3 == always 3.3V
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOA, GPIO_Pin_3, 1);
#else
    //PC3 == always 3.3V
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
#endif
}

int16_t QuadEncValue1()
{
    return (int16_t)TIM_GetCounter (Codeur1_TIMER) ;
}

int16_t QuadEncValue2()
{
    return (int16_t)TIM_GetCounter (Codeur2_TIMER) ;
    //return GPIO_ReadInputDataBit(Codeur2_GPIO, Codeur2_A) + GPIO_ReadInputDataBit(Codeur2_GPIO, Codeur2_B)*2;
}

bool QuadEncButton1()
{
    return GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN1)==0;
}

bool QuadEncButton2()
{
    return GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN2)==0;
}
