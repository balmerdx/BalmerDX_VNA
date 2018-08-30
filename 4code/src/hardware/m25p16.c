#include "stm32f4xx_conf.h"
#include "m25p16.h"
#include "delay.h"

/* Support for M25P16 2Mbyte flash RAM 


BEWARE  Programming can only set bits to zero. IT CANNOT CHANGE A ZERO TO A ONE
You must use the Bulk or sector erase to set bits back to a one


	RAM_PORT		PORT for the RAM CS signal
	RAM_CS


and RAM registers

	RAM_RDID
	RAM_WREN
	RAM_BE
	RAM_PP
	RAM_RDSR

Additionally, RAM_CS must be defined as an output on the appropriate DDR

*/

// Register definitions for M25P16 flash ram
#define RAM_WREN 0x06	// write enable
#define RAM_WRDI 0x04	// write disable
#define RAM_RDID 0x9F	// read id
#define RAM_RDSR 0x05	// read status
#define RAM_WRSR 0x01	// write status
#define RAM_READ 0x03	// read data
#define RAM_FASTREAD 0x0B
#define RAM_PP 0x02		// page program
#define RAM_SE 0xD8		// sector erase
#define RAM_BE 0xC7		// bulk erase
#define RAM_DP 0xB9		// deep power down
#define RAM_RES 0xAB	// release from power down, read electronic signature


#define FLASH_SPI		SPI3
#define FLASH_AF_SPI GPIO_AF_SPI3
#define FLASH_PORT_SPI	GPIOC
#define PIN_SOURCE_SCK	GPIO_PinSource10
#define PIN_SOURCE_MISO	GPIO_PinSource11
#define PIN_SOURCE_MOSI	GPIO_PinSource12
#define PIN_SCK		(1<<PIN_SOURCE_SCK)
#define PIN_MISO	(1<<PIN_SOURCE_MISO)
#define PIN_MOSI	(1<<PIN_SOURCE_MOSI)

#define PIN_CS GPIO_Pin_15
#define PORT_CS GPIOA
#define CS_ONE  GPIO_WriteBit(PORT_CS, PIN_CS, 1);//set cs pin to one
#define CS_ZERO GPIO_WriteBit(PORT_CS, PIN_CS, 0);//set cs pin to zero

static void xmit_spi(uint8_t data)
{
    FLASH_SPI->DR = data;
    while( !(FLASH_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
    while( FLASH_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
    volatile uint8_t d = FLASH_SPI->DR;
    (void)d;
}

static uint8_t recv_spi()
{
    FLASH_SPI->DR = 0;
    while( !(FLASH_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
    while( FLASH_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
    return FLASH_SPI->DR;
}


void m25p16_init()
{
    GPIO_InitTypeDef gpio;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);

    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Pin = PIN_CS;
    GPIO_Init(PORT_CS, &gpio);

    gpio.GPIO_Pin = PIN_SCK | PIN_MOSI | PIN_MISO;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(FLASH_PORT_SPI, &gpio);

    // connect SPI pins to SPI alternate function
    GPIO_PinAFConfig(FLASH_PORT_SPI, PIN_SOURCE_SCK, FLASH_AF_SPI);
    GPIO_PinAFConfig(FLASH_PORT_SPI, PIN_SOURCE_MISO, FLASH_AF_SPI);
    GPIO_PinAFConfig(FLASH_PORT_SPI, PIN_SOURCE_MOSI, FLASH_AF_SPI);

    SPI_InitTypeDef spi;
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_1Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(FLASH_SPI, &spi);
    SPI_Cmd(FLASH_SPI, ENABLE);
    SPI_NSSInternalSoftwareConfig(FLASH_SPI, SPI_NSSInternalSoft_Set);
}


void m25p16_read_ram_id(uint8_t* mem_ptr) {

    CS_ZERO
	xmit_spi(RAM_RDID);
	mem_ptr[0] = recv_spi();
	mem_ptr[1] = recv_spi();
	mem_ptr[2] = recv_spi();
    CS_ONE
}

bool m25p16_read_ram_id_and_check()
{
    uint8_t data[3];
    m25p16_read_ram_id(data);
    return data[0]==0x20 && data[1]==0x20 && data[2]==0x15;
}


uint8_t m25p16_read_ram_status(void) {

	uint8_t status;

    CS_ZERO
	xmit_spi(RAM_RDSR);
	status = recv_spi();
    CS_ONE
	return status;
}

bool m25p16_write_in_progress(void) {

    uint8_t status;

    CS_ZERO
    xmit_spi(RAM_RDSR);
    status = recv_spi();
    CS_ONE
    return (status&1)?true:false;
}

void m25p16_ram_bulk_erase(void) {
		
    CS_ZERO
	xmit_spi(RAM_WREN);					// write enable instruction 	
    CS_ONE
    DelayUs(2);
    CS_ZERO
	xmit_spi(RAM_BE);					// bulk erase instruction 
    CS_ONE
    while (m25p16_write_in_progress())
		;
}


void m25p16_ram_sector_erase(uint8_t sector) {
		

    CS_ZERO
	xmit_spi(RAM_WREN);					// write enable instruction 	
    CS_ONE
    DelayUs(2);
    CS_ZERO
	xmit_spi(RAM_SE);					// sector erase instruction 
	xmit_spi(sector);					// sector erase instruction 
	xmit_spi(0x00);						// sector erase instruction 
	xmit_spi(0x00);						// sector erase instruction 
    CS_ONE
    while (m25p16_write_in_progress())
		;
}

void m25p16_read_write_flash_ram(M25P16_RW_TYPE rw_type, uint16_t bytes_to_readwrite, uint8_t flash_sector, uint8_t flash_page, uint8_t offset,
                                 uint8_t* mem_ptr, m25p16_check_callback callback) {

// NB CAUTION page writes which cross page boundaries will wrap 


// parameters 

// one_read_zero_write = 1 for read, 0 for write 
// bytes_to_readwrite to read or write 
// flash sector within device 
// flash page within device 
// offset for first byte to transfer 
// POINTER TO ram address for first byte to transfer 

// for ram device, enter and leave with SCK low 


    CS_ZERO
    if (rw_type) {
		xmit_spi(RAM_READ);
	} else {
		xmit_spi(RAM_WREN);				// write enable instruction 
        CS_ONE
        DelayUs(2);
        CS_ZERO
		xmit_spi(RAM_PP);
	}
	xmit_spi(flash_sector);
	xmit_spi(flash_page);
	xmit_spi(offset);
    for (uint16_t i=0;i<bytes_to_readwrite;i++) {
        if (rw_type) {
            uint8_t data = recv_spi();

            if(callback)
            {
                callback(i, data, mem_ptr);
            } else
            {
                mem_ptr[i] = data;
            }

		} else {
			xmit_spi(mem_ptr[i]);
		}
	}	
    CS_ONE

    if(rw_type==M25P16_RW_WRITE)
    {
        DelayMs(2);

        while (m25p16_write_in_progress()) {
            DelayMs(2);
        }
    } else
    {
        DelayUs(2);
    }

}


// write to the RAM status byte. 0 in bottom bit position = ready 
void m25p16_write_ram_status(uint8_t status) {

    CS_ZERO
	xmit_spi(RAM_WREN);					// write enable instruction 	
    CS_ONE
    DelayUs(2);
    CS_ZERO
	xmit_spi(RAM_WRSR);
	xmit_spi(status);
    CS_ONE
    DelayUs(2);
    while (m25p16_read_ram_status() & 0x01)
		;	
}


void m25p16_power_up_flash_ram(void) {

    CS_ZERO
	xmit_spi(RAM_RES);
    CS_ONE
    DelayUs(30);
}


void m25p16_power_down_flash_ram(void) {

    CS_ZERO
	xmit_spi(RAM_DP);
    CS_ONE
}

static void m25p16_read_or_write_flash(uint8_t one_read_zero_write, uint8_t flash_sector, uint16_t offset, uint16_t size,
                                       uint8_t* mem_ptr, m25p16_check_callback callback)
{
    if(size==0)
        return;
    int page_size = 256;
    uint8_t flash_page = offset/page_size;
    uint8_t offset_first = offset%page_size;
    uint16_t bytes_to_readwrite;

    //Первая страница может быть как неполной вначале, так и в конце
    bytes_to_readwrite = page_size-offset_first;
    if(bytes_to_readwrite>size)
        bytes_to_readwrite = size;
    m25p16_read_write_flash_ram(one_read_zero_write, bytes_to_readwrite, flash_sector, flash_page, offset_first, mem_ptr, callback);
    size -= bytes_to_readwrite;
    mem_ptr+= bytes_to_readwrite;
    flash_page++;

    //Остальные страницы всегда начинаются сначала
    while(size>0)
    {
        bytes_to_readwrite = 256;
        if(bytes_to_readwrite>size)
            bytes_to_readwrite = size;
        m25p16_read_write_flash_ram(one_read_zero_write, bytes_to_readwrite, flash_sector, flash_page, 0, mem_ptr, callback);
        size -= bytes_to_readwrite;
        mem_ptr+= bytes_to_readwrite;
        flash_page++;
    }

}

void m25p16_read(uint8_t flash_sector, uint16_t offset, uint16_t size, void* mem_ptr)
{
    m25p16_read_or_write_flash(M25P16_RW_READ, flash_sector, offset, size, (uint8_t*)mem_ptr, 0);
}

void m25p16_write(uint8_t flash_sector, uint16_t offset, uint16_t size, void* mem_ptr)
{
    m25p16_read_or_write_flash(M25P16_RW_WRITE, flash_sector, offset, size, (uint8_t*)mem_ptr, 0);
}

void m25p16_check(uint8_t flash_sector, uint16_t offset, uint16_t size, void* mem_ptr, m25p16_check_callback callback)
{
    m25p16_read_or_write_flash(M25P16_RW_READ, flash_sector, offset, size, mem_ptr, callback);
}
