
/* Includes ------------------------------------------------------------------*/
//#include "hw_config.h"
#include "ad9958_drv.h"
#include "delay.h"


#define DDS_POWER_DOWN_ENABLE
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup AD9958_Private_Variables
* @{
*/
#define PLL_MUL 20				// PLL multiplier
//#define PLL_MUL 4

#define REFCLK					25e6			// Reference clock: 25 MHz
#define MASTER_CLOCK			(REFCLK*PLL_MUL)// Master clock: 500 MHz

#define MAX_OUTPUT_FREQUENCY	MAX_FREQUENCY		/* 230 MHz */


#ifdef DISPLAY_ILI9481

#define DDS_SCLK(b) GPIO_WriteBit(GPIOE, GPIO_Pin_2, b)
#define DDS_SDIO_0(b) GPIO_WriteBit(GPIOE, GPIO_Pin_3, b)

#ifdef DDS_POWER_DOWN_ENABLE
#define DDS_PWR_DOWN(b)	GPIO_WriteBit(GPIOE, GPIO_Pin_5, b)
#endif

#define DDS_RESET(b)		GPIO_WriteBit(GPIOE, GPIO_Pin_4, b)
#define DDS_IO_UPD(b)		GPIO_WriteBit(GPIOE, GPIO_Pin_0, b)
#define DDS_CS(b)			GPIO_WriteBit(GPIOE, GPIO_Pin_1, b)

#else

#define DDS_SCLK(b) GPIO_WriteBit(GPIOB, GPIO_Pin_2, b)
#define DDS_SDIO_0(b) GPIO_WriteBit(GPIOB, GPIO_Pin_1, b)

#ifdef DDS_POWER_DOWN_ENABLE
#define DDS_PWR_DOWN(b)	GPIO_WriteBit(GPIOB, GPIO_Pin_0, b)
#endif

#define DDS_RESET(b)		GPIO_WriteBit(GPIOC, GPIO_Pin_5, b)
#define DDS_IO_UPD(b)		GPIO_WriteBit(GPIOB, GPIO_Pin_10, b)
#define DDS_CS(b)			GPIO_WriteBit(GPIOB, GPIO_Pin_11, b)

#endif

#define DelayTick() DelayUs(10)


// CSR: 3-wire mode, MSB first, channels enabled
static uint8_t wire = 2;
// CSR: 2-wire mode, MSB first, channels enabled
//static uint8_t wire = 0;

static void dds_send(uint8_t u8Data);
static void WR_CSR(uint8_t u8Data);
static void WR_FR1(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3);
static void WR_FR2(uint8_t u8Data1,uint8_t u8Data2);
static void WR_CFR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3);
static void WR_CFTW0(uint32_t u32ftWord);
static void WR_CPOW0(uint32_t u32CpWord);
static void WR_ACR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3);
static void IOUpdatePulse(void);

static bool AD9958_SetChannel(int iChannel);
static void AD9958_SetAllChannel();

#define HI8(x) (uint8_t)(x>>8)
#define LO8(x) (uint8_t)(x)

static void GPIO_Configuration(void)
{
#ifdef DISPLAY_ILI9481
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

    //RESET SDIO_0 SCLK IO_UPD CS
    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOE, &gpio);

#ifdef DDS_POWER_DOWN_ENABLE
    gpio.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOE, &gpio);
#endif

#else

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

    //RESET
    gpio.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &gpio);

    //SDIO_0 SCLK IO_UPD CS
    gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &gpio);

#ifdef DDS_POWER_DOWN_ENABLE
    gpio.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &gpio);
#endif

#endif
}


/**
  * @brief  Initializes the AD9958 device.
  *
  * @retval None
  */
void AD9958_Init(void)
{

    GPIO_Configuration();

  	// DDS configuration -------------------------------------------------------
  	/* Resets DDS device */
#ifdef DDS_POWER_DOWN_ENABLE    
	DDS_PWR_DOWN(0);
#endif
	DDS_IO_UPD(1);
	DDS_CS(1);

	DDS_RESET(0);
    DelayTick();
	DDS_RESET(1);
	DelayMs(2);
	DDS_RESET(0);
	DelayMs(2);


    WR_CSR(0xc0|wire);

	/* FR1: PLL=20  */
    //WR_FR1(0xd0,0x00,0x00);

    DelayMs(2);

    if(PLL_MUL<8)
        WR_FR1(0x00+(PLL_MUL*4),0x00,0x00);
    else
        WR_FR1(0x80+(PLL_MUL*4),0x00,0x00);

    //PLL=20, Charge Pump=max
    //WR_FR1(0xd3,0x00,0x00);

    //PLL=4, Charge Pump=default, <160 MHz
    //WR_FR1(0x00,0x00,0x00);

    // Reset phase
 	WR_CPOW0(0x00);
/*
	// Gain for main channel
 	WR_CSR(0x40|wire);
	WR_CFR(0x00,0x03,0x00);
	WR_ACR(0x00, 0x11, 0x80);
 	WR_CFTW0(0);

	// Gain for LO
	WR_CSR(0x80|wire);
	WR_CFR(0x00,0x03,0x00);
	WR_ACR(0x00, 0x11, 0x00);
 	WR_CFTW0(0);
*/
	/* Enable both channel cmds */
	AD9958_SetAllChannel();

	/* Transfer registers */
	IOUpdatePulse();
}

static bool AD9958_SetChannel(int iChannel)
{
    uint8_t ch = 0;
#ifdef DDS_AD9959
    if(iChannel<0 || iChannel>=4)
        return false;
    ch = 0x10<<iChannel;
#else
    //AD9958
    if(iChannel<0 || iChannel>=2)
        return false;
    ch = 0x40<<iChannel;
#endif
    WR_CSR(ch|wire);
    return true;
}

static void AD9958_SetAllChannel()
{
    uint8_t ch = 0;
#ifdef DDS_AD9959
    ch = 0xF0;
#else
    //AD9958
    ch = 0xc0;
#endif
    WR_CSR(ch|wire);
}

/**
  * @brief  Sets the DDS frequency
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  dfFreq 		Frequency in Hz
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
bool AD9958_Set_Frequency(int iChannel, double dfFreq)
{
  uint32_t freqWord = AD9958_Calc_FrequencyWord(dfFreq);
  return AD9958_Set_FrequencyWord(iChannel, freqWord);
}

uint32_t AD9958_Calc_FrequencyWord(double dfFreq)
{
  return (uint32_t)(dfFreq*(0xFFFFFFFF/(float)MASTER_CLOCK+1.0/MASTER_CLOCK));
}

bool AD9958_Set_FrequencyWord(int iChannel, uint32_t freqWord)
{
    if(!AD9958_SetChannel(iChannel))
        return false;

    /* Write frequency word */
    WR_CFTW0(freqWord);

    AD9958_SetAllChannel();

    /* All channels clear phase for synchronization */
    WR_FR2(0x10,0x00);
    WR_FR2(0x00,0x00);

    /* Transfer registers */
    IOUpdatePulse();
    return true;
}

/**
  * @brief  Sets the amplitude level for the selected channel
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  u16Level 	Amplitude word 511 - max
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
bool AD9958_Set_Level(int iChannel,uint16_t u16Level)
{
    if(!AD9958_SetChannel(iChannel))
        return false;

    /* Adjust amplitude level */
    uint8_t u8High=HI8(u16Level)|0x10;
    uint8_t u8Low=LO8(u16Level);
    WR_ACR(0x00, u8High, u8Low);

    AD9958_SetAllChannel();

    // Transfer registers
    IOUpdatePulse();
    return true;
}

/**
  * @brief  Enter or resumes from low power mode
  *
  * @param  u8Enable 	Action:
  *						@li true: resume from power down
  *						@li false: enter power down
  * @retval None
  */
#ifdef DDS_POWER_DOWN_ENABLE
void AD9958_Power_Control(uint8_t u8Enable)
{
	if (u8Enable)
	{
	  	/* Resume from power down */
		DDS_PWR_DOWN(0);
	 	WR_FR1(0xd0,0x00,0x00);
	}
	else
	{
		/* Enter power down */
	 	WR_FR1(0xd0,0x00,0xc0);
		DDS_PWR_DOWN(1);
	}
}
#endif
/**
  * @brief  Write Channel Select Register register
  *
  * @param  u8Data	Reg value
  * @retval None
  */
static void WR_CSR(uint8_t u8Data)
{
	DDS_CS(0);

	dds_send(0x00);	/* CSR's address is 0 */
	dds_send(u8Data);

	DDS_CS(1);
}

/**
  * @brief  Write Function Register 1 register
  *
  * @param  u8Data1	 MSB
  * @param  u8Data2
  * @param  u8Data3	 LSB
  * @retval None
  */
static void WR_FR1(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3)
{
	DDS_CS(0);

	dds_send(0x01);	/* FR1's address is 1 */
	dds_send(u8Data1);
	dds_send(u8Data2);
	dds_send(u8Data3);

	DDS_CS(1);
}

/**
  * @brief  Write Function Register 2 register
  *
  * @param  u8Data1		MSB
  * @param  u8Data2	 	LSB
  * @retval None
  */
static void WR_FR2(uint8_t u8Data1,uint8_t u8Data2)
{
	DDS_CS(0);

	dds_send(0x02);	/* FR1's address is 2 */
	dds_send(u8Data1);
	dds_send(u8Data2);

	DDS_CS(1);
}

/**
  * @brief  Write Channel Function Register1 register
  *
  * @param  u8Data1 	MSB
  * @param  u8Data2
  * @param  u8Data3 	LSB
  * @retval None
  */
static inline void WR_CFR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3)
{
	DDS_CS(0);

	dds_send(0x03);	/* CFR's address is 3 */
	dds_send(u8Data1);
	dds_send(u8Data2);
	dds_send(u8Data3);

	DDS_CS(1);
}
/**
  * @brief  Write Channel Frequency Tuning Word 01 register
  *
  * @param  u32ftWord 	freq tuning word
  * @retval None
  */
static void WR_CFTW0(uint32_t u32ftWord)
{
	DDS_CS(0);

	dds_send(0x04);	/* CFTW0's address is 4 */
	dds_send(((uint8_t *)(&u32ftWord))[3]);
	dds_send(((uint8_t *)(&u32ftWord))[2]);
	dds_send(((uint8_t *)(&u32ftWord))[1]);
	dds_send(((uint8_t *)(&u32ftWord))[0]);

	DDS_CS(1);
}

/**
  * @brief  Write Channel Phase Offset Word 01
  *
  * @param  u32CpWord: phase offset word
  * @retval None
  */
static void WR_CPOW0(uint32_t u32CpWord)
{
	DDS_CS(0);

	dds_send(0x05);	/* CPW0's address is 5 */
	dds_send((((uint8_t *)(&u32CpWord))[1])&0x3F);
	dds_send(((uint8_t *)(&u32CpWord))[0]);

	DDS_CS(1);
}

/**
  * @brief  Write Amplitude Control Register
  *
  * @param  u8Data1 	MSB
  * @param  u8Data2
  * @param  u8Data3 	LSB
  * @retval None
  */
static void WR_ACR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3)
{
	DDS_CS(0);

	dds_send(0x06);	/* ACR's address is 3 */
	dds_send(u8Data1);
	dds_send(u8Data2);
	dds_send(u8Data3);

	DDS_CS(1);
}

static void dds_send(uint8_t data)
{
    for(uint8_t i=0; i<8; i++)
    {
        DDS_SDIO_0( (data & 0x80)? 1:0 );
        data = data<<1;

        DelayTick();
        DDS_SCLK(1);

        DelayTick();
        DDS_SCLK(0);
        DelayTick();
    }
}

/**
  * @brief  Send pulse on IO_UPDT to transfer registers
  * @param  None
  * @retval None
  */
static void IOUpdatePulse(void)
{
    DDS_IO_UPD(0);
    DelayTick();
    DDS_IO_UPD(1);
}

void AD9958_Set_Bad_PLL_MUL()
{
    int pll_mul = 8;
    WR_FR1(0x80+(pll_mul*4),0x00,0x00);
}
