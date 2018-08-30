#include "main.h"
#include "cs4272.h"
#include "stm32f4xx_conf.h"
#include "delay.h"
#include "process_sound.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"

#ifdef DISPLAY_ILI9481

#define SOUND_DMA_STREAM DMA1_Stream3
#define SOUND_DMA_CHANNEL DMA_Channel_0

#define CS_I2C I2C2
#define CS_SPI SPI2

#define RST_HIGH		GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define RST_LOW			GPIO_ResetBits(GPIOB, GPIO_Pin_14)

//  RST - PB14

// SPI2_SCL PB10
// SPI2_SDA PB11
// SPI2_NSS PB12
// SPI2_SCK PB13
// I2S2_SD  PB15

#else

#define SOUND_DMA_STREAM DMA1_Stream0
#define SOUND_DMA_CHANNEL DMA_Channel_0

//	CCLK - PB6
//	CDIN - PB7
//  RST - PB8

// SPI3_NSS PA15
// SPI3_SCK PC10
// I2S3_SD  PC12

#define CS_I2C I2C1
#define CS_SPI SPI3

#define RST_HIGH		GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define RST_LOW			GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#endif


#define SLAVE_ADDRESS 0x20

static __IO uint8_t x4count = 0;
uint16_t sound_buffer[SOUND_BUFFER_SIZE];

void cs4272_start();

#ifdef DISPLAY_ILI9481
void init_I2C(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    //deinit i2c for bootloader
    I2C_Cmd(CS_I2C, DISABLE);
    I2C_DeInit(CS_I2C);

    DelayMs(2);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;			// set pins to alternate function
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//GPIO_Speed_50MHz;		// set GPIO speed
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// enable pull up resistors
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Connect I2C2 pins to AF
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);// SCL
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2); // SDA

    // configure I2C2
    I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// own address, not relevant in master mode
    I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;		// disable acknowledge when reading (can be changed later on)
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
    I2C_Init(CS_I2C, &I2C_InitStruct);				// init I2C2

    I2C_Cmd(CS_I2C, ENABLE);
}

#else
void init_I2C(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* setup SCL and SDA pins
	 * You can connect I2C1 to two different
	 * pairs of pins:
	 * 1. SCL on PB6 and SDA on PB7
	 * 2. SCL on PB8 and SDA on PB9
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;			// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

	// Connect I2C1 pins to AF
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA

	// configure I2C1
	I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
    I2C_Init(CS_I2C, &I2C_InitStruct);				// init I2C1

    I2C_Cmd(CS_I2C, ENABLE);
}
#endif

/* This function issues a start condition and
 * transmits the slave address + R/W bit
 *
 * Parameters:
 * 		I2Cx --> the I2C peripheral e.g. I2C1
 * 		address --> the 7 bit slave address
 * 		direction --> the tranmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */
static int I2C_start_error = 0;

bool I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
    I2C_start_error = 1;
    uint16_t start_time = TimeUs();
    const uint16_t max_delta = 10000;
	// wait until I2C1 is not busy anymore
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
         if((uint16_t)(TimeUs()-start_time) > max_delta)
             return false;
    }

	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);

    I2C_start_error = 2;
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((uint16_t)(TimeUs()-start_time) > max_delta)
            return false;
    }

	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
    I2C_start_error = 3;
	if(direction == I2C_Direction_Transmitter){
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if((uint16_t)(TimeUs()-start_time) > max_delta)
                return false;
        }
	}
	else if(direction == I2C_Direction_Receiver){
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if((uint16_t)(TimeUs()-start_time) > max_delta)
                return false;
        }
	}

    I2C_start_error = 0;
    return true;
}

/* This function transmits one byte to the slave device
 * Parameters:
 *		I2Cx --> the I2C peripheral e.g. I2C1
 *		data --> the data byte to be transmitted
 */
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data){
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/* This function reads one byte from the slave device
 * and acknowledges the byte (requests another byte)
 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data
 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
	// disabe acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This funtion issues a stop condition and therefore
 * releases the bus
 */
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

bool cs4272_i2c_write_reg(uint8_t reg, uint8_t data)
{
    // start a transmission in Master transmitter mode
    if(!I2C_start(CS_I2C, SLAVE_ADDRESS, I2C_Direction_Transmitter))
        return false;
    I2C_write(CS_I2C, reg); // write one byte to the slave
    I2C_write(CS_I2C, data); // write another byte to the slave
    I2C_stop(CS_I2C);
    return true;
}

uint8_t cs4272_i2c_read_reg(uint8_t reg)
{
    I2C_start(CS_I2C, SLAVE_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
    I2C_write(CS_I2C, reg); // write one byte to the slave
    I2C_stop(CS_I2C);

    I2C_start(CS_I2C, SLAVE_ADDRESS, I2C_Direction_Receiver); // start a transmission in Master receiver mode
    uint8_t data = I2C_read_nack(CS_I2C); // read one byte and don't request another byte
    I2C_stop(CS_I2C);
	return data;
}


static void init_GPIO_I2S()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2S_InitTypeDef I2S_InitStruct;

#ifdef DISPLAY_ILI9481
    //I2S2 used
    //SPI3_NSS PB12
    //SPI3_SCK PB13
    //I2S3_SD  PB15
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    I2S_InitStruct.I2S_Mode = I2S_Mode_SlaveRx;
    I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_24b;
    I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(CS_SPI, &I2S_InitStruct);
#else
    //I2S3 used
	//PA15 - NSS
	//PC10 - SCK
	//PC12 - MOSI
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

	I2S_InitStruct.I2S_Mode = I2S_Mode_SlaveRx;
	I2S_InitStruct.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_24b;
	I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(CS_SPI, &I2S_InitStruct);
#endif

	NVIC_InitTypeDef NVIC_InitStructure;

	if(g_i2s_dma)
	{//use dma
        SPI_I2S_ITConfig(CS_SPI, SPI_I2S_IT_RXNE, DISABLE);

        DMA_Cmd(SOUND_DMA_STREAM, DISABLE);
        DMA_DeInit(SOUND_DMA_STREAM);

    	DMA_InitTypeDef dma_init;
        dma_init.DMA_Channel = SOUND_DMA_CHANNEL;
        dma_init.DMA_PeripheralBaseAddr = (uint32_t)&(CS_SPI->DR);
		dma_init.DMA_Memory0BaseAddr = (uint32_t)sound_buffer;
		dma_init.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma_init.DMA_BufferSize = (uint32_t)SOUND_BUFFER_SIZE;
		dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		dma_init.DMA_Mode = DMA_Mode_Circular;
		dma_init.DMA_Priority = DMA_Priority_High;
		dma_init.DMA_FIFOMode = DMA_FIFOMode_Disable;        
		dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
        DMA_Init(SOUND_DMA_STREAM, &dma_init);
        SPI_I2S_DMACmd(CS_SPI, SPI_I2S_DMAReq_Rx, ENABLE);
	} else
	{//use interrupt
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); 
		/* Configure the SPI interrupt priority */
#ifdef DISPLAY_ILI9481
        NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
#else
        NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
#endif
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}

bool cs4272_Init()
{
	GPIO_InitTypeDef gpio;

#ifdef DISPLAY_ILI9481
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    //RST_
    gpio.GPIO_Pin = GPIO_Pin_14;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &gpio);
#else
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
    //RST_
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio);

    //????Unused bad code??????????
	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &gpio);
#endif

    UTF_DrawString(210, 120, "0");
	RST_LOW;
	DelayMs(20);
    UTF_DrawString(210, 120, "1");

    init_I2C();
    UTF_DrawString(210, 120, "2");
    init_GPIO_I2S();
    UTF_DrawString(210, 120, "3");
    cs4272_start();
    UTF_DrawString(210, 120, "4");

	DelayMs(10);
	RST_HIGH;

    if(0)
    for(int address=2; address<256; address+=2)
    {
        // start a transmission in Master transmitter mode

        I2C_DeInit(CS_I2C);
        I2C_Cmd(CS_I2C, DISABLE);

        DelayMs(10);
        init_I2C();
        DelayMs(10);

        UTF_printNumI(address, 0, 120, 200, UTF_CENTER);
        bool ok = I2C_start(CS_I2C, address, I2C_Direction_Transmitter);
        I2C_stop(CS_I2C);
        UTF_printNumI(I2C_start_error, 200, 120, 200, UTF_CENTER);

        if(ok)
        {
            UTF_DrawString(210, 120, "SUCCESS");
            while(1);
        }
    }


    DelayMs(1);
    UTF_printNumI(CS_I2C->SR1, 210, 100, 100, UTF_LEFT);
    UTF_DrawString(210, 120, "5");
    if(!cs4272_i2c_write_reg(0x7, 0x3)) //Control Port Enable + Power Down
    {
        UTF_DrawString(210, 132, "Error I2C CS4272");
        while(1);
    }

    UTF_DrawString(210, 120, "6");
	if(cs4272_i2c_read_reg(0x7)!=0x3)
    {
        UTF_DrawString(210, 120, "7");
		return false;
    }

    UTF_DrawString(210, 120, "8");

	cs4272_i2c_write_reg(0x1, (1<<3)); //Master mode 48 KHz

	cs4272_i2c_write_reg(0x3, 0);//Mute DAC
	//cs4272_i2c_write_reg(0x6, (1<<5)|(1<<4));//16 bit dither + I2S format
	cs4272_i2c_write_reg(0x6, (1<<4));//I2S format

	cs4272_i2c_write_reg(0x7, 0x2); //Clear Power Down

	return true;
}

void cs4272_start()
{
	x4count = 0;

    if(g_i2s_dma)
    {
        DMA_Cmd(SOUND_DMA_STREAM, ENABLE);
    } else
    {
        SPI_I2S_ITConfig(CS_SPI, SPI_I2S_IT_RXNE, ENABLE);
    }

    I2S_Cmd(CS_SPI, ENABLE);
}

void cs4272_stop()
{
    I2S_Cmd(CS_SPI, DISABLE);
    SPI_I2S_DeInit(CS_SPI);

    SPI_I2S_ITConfig(CS_SPI, SPI_I2S_IT_RXNE, DISABLE);

    DMA_Cmd(SOUND_DMA_STREAM, DISABLE);
    DMA_DeInit(SOUND_DMA_STREAM);

    I2C_DeInit(CS_I2C);
    I2C_Cmd(CS_I2C, DISABLE);

    RST_LOW;
    DelayMs(20);
    RST_HIGH;
}


void OnSoundReceive()
{
	/* Check if data are available in SPI Data register */
	if(!g_i2s_dma)
    if(SPI_GetITStatus(CS_SPI, SPI_I2S_IT_RXNE) != RESET)
	{
        uint16_t app = SPI_I2S_ReceiveData(CS_SPI);
		static uint16_t data4[4];
		data4[x4count++] = app;
		if(x4count==4)
		{
			int32_t sampleQ = (((int32_t)data4[0])<<16)+data4[1];
			int32_t sampleI = (((int32_t)data4[2])<<16)+data4[3];
			OnSoundData(sampleQ, sampleI);
			x4count = 0;
		}
	}
}

uint16_t cs4272_getPos()
{
    uint16_t ndtr = (uint16_t)SOUND_DMA_STREAM->NDTR;
	if(ndtr>SOUND_BUFFER_SIZE)
		return 0;
	uint16_t pos = SOUND_BUFFER_SIZE - ndtr;
	pos = pos & ~(uint16_t)3;
	return pos;
}
