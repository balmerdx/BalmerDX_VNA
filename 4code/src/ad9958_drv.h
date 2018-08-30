/**
 reworked driver from Melchor Varela - EA4FRB / SARK110
 reborked by BalmerVNA
*/

#ifndef __AD9958_DRV_H__
#define __AD9958_DRV_H__
#include "stm32f4xx_conf.h"

#define MAX_FREQUENCY   230000000 /*!< Maximum DDS frequency: 230Mhz */

//If defined DDS_AD9959
//4 channel enabled
//else 2 channel AD9958
//#define DDS_AD9959

/**
  * @brief  Initializes the AD9958 device.
  *
  * @retval None
  */
void AD9958_Init(void);

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
bool AD9958_Set_Frequency(int iChannel, double dfFreq);

uint32_t AD9958_Calc_FrequencyWord(double dfFreq);
bool AD9958_Set_FrequencyWord(int iChannel, uint32_t freqWord);

/**
  * @brief  Enter or resumes from low power mode
  *
  * @param  u8Enable 	Action:
  *						@li TRUE: resume from power down
  *						@li FALSE: enter power down
  * @retval None
  */
void AD9958_Power_Control(uint8_t u8Enable);

/**
  * @brief  Sets the amplitude level for the selected channel
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  u16Level 	Amplitude word
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
bool AD9958_Set_Level(int iChannel,uint16_t u16Level);

//!!! For testin only!!!!
void AD9958_Set_Bad_PLL_MUL();

#endif	 /* __AD9958_DRV_H__ */

