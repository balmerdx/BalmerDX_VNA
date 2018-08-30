#pragma once
typedef enum M25P16_RW_TYPE
{
    M25P16_RW_WRITE = 0,
    M25P16_RW_READ = 1,
} M25P16_RW_TYPE;

//callback для функции m25p16_check
//idx - индекс читаемого элемента
//flash_byte - байт, прочитанный из flash
//mem_ptr - указатель, передаваемый в m25p16_check
typedef void (*m25p16_check_callback)(uint16_t idx, uint8_t flash_byte, uint8_t* mem_ptr);

void m25p16_init();


/** \brief		Read 3 bytes of ID from the M26P16. These should always be 0x20,0x20,0x15
 *	\param		*mem_ptr	    Pointer to location to store values returned.
 *
 */
void m25p16_read_ram_id(uint8_t* mem_ptr);


//Read ID and check validity
bool m25p16_read_ram_id_and_check();

#define m25p16_sectors_count() 32
#define m25p16_sector_size() 0x10000



/** \brief		Select the M25P16 and return 1 byte from the Status register.
 *	\return					returns device status byte.
 *
 */
uint8_t m25p16_read_ram_status(void);



/** \brief		Erase the M25P16.
 *
 *				This function issues an erase command, then blocks until the command is complete as shown by
 *				 the status register being zero.
 *				Note that the erase actually sets all bits to 1. The page program can set bits to 0, but NOT to 1.
 *				Therefore each page should be considered 'write once' between erase cycles.
 */
void m25p16_ram_bulk_erase(void);



/** \brief		Erase 1 sector of the M25P16.
 *	\param		sector	    The sector number (0-31) to be erased.
 *
 *				This function issues a sector erase command, then blocks until the command is complete as shown by
 *				 the status register being zero
 *				For future expansion, no masking of the sector byte to ensure that it does not contain values >31 takes place
 *				Note that the erase actually sets all bits to 1. The page program can set bits to 0, but NOT to 1.
 *				Therefore each page should be considered 'write once' between erase cycles.
 */
void m25p16_ram_sector_erase(uint8_t sector);


/** \brief		Perform an arbirary read/write from/to the M26P16
 *	\param		rw_type
        M25P16_RW_WRITE - write mem_ptr
        M25P16_RW_READ - read to mem_ptr

 *	\param		bytes_to_readwrite		uint16_t Number of bytes to read or write. Values will typically be less than or exactly 256. See below for why.
 *	\param		flash_sector			Sector number in the M25P16, range 0..31.
 *	\param		flash_page				Page number in the sector.
 *	\param		offset					Position in the page at which to start the read/write
 *	\param		*mem_ptr				Pointer to memory location for read/write data
 *  \param      callback                Вызываеся вместо записи в mem_ptr при операции чтения.
 *
 *				This function would typically only be used for reads or writes of <256 bytes.
 *
 *				IMPORTANT   	The M25P16 is a block device. It deals in 256 byte pages.
 *								Writes only every take place to a single 256 byte page.
 *								If writing >256 bytes, anything other than the last 256 bytes will be overwritten and ignored.
 *								If offset is non-zero, then be aware that if offset+bytes_to_readwrite > 255, then any write
 *								  will wrap back to the beginning of the page. This is unlikely to be what you want.
 */
void m25p16_read_write_flash_ram(M25P16_RW_TYPE rw_type, uint16_t bytes_to_readwrite,uint8_t flash_sector,uint8_t flash_page,uint8_t offset,
                                 uint8_t* mem_ptr, m25p16_check_callback callback);


//Более удобный функции чтения/записи в сектор данных.
//Разбиваем на flash_page команды внутри
//offset - смещение относительно начала flash_sector offset=(0-65535)
//size - количество данных для записи size=(0-65535)-offset
void m25p16_read(uint8_t flash_sector, uint16_t offset, uint16_t size, void* mem_ptr);
void m25p16_write(uint8_t flash_sector, uint16_t offset, uint16_t size, void* mem_ptr);

//Вызывает для всех прочитанных байтов функцию m25p16_check_callback
void m25p16_check(uint8_t flash_sector, uint16_t offset, uint16_t size, void* mem_ptr, m25p16_check_callback callback);


/** \brief		Write to the status register on the M25P16.
 *	\param		status		Value to write
 *
 *				This function blocks until the bottom bit of the status register is clear = device ready.
 */
void m25p16_write_ram_status(uint8_t status);



/** \brief		Issue the command to bring the M25P16 out of power down mode.
 *
 *				This function has no effect if the device is currently in one of the erase modes.
 *				At power up the deice will be in standby mode, there is no need to issue the power_up_flash_ram() command after a power up.
 */
void m25p16_power_up_flash_ram(void);



/** \brief		Issue the command to put the M25P16 into power down mode.
 *
 *				In Power down mode the device ignores all erase and program instructions.
 *
 *				In this mode the device draws 1uA typically.
 *				Use the power_up_flash_ram() command to bring the device out of power down mode.
 *				Removing power completely will also cancel the Deep power down mode - it will power up again in standby mode.
 */
void m25p16_power_down_flash_ram(void);

