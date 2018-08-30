#
# "base" - base directory
# "dirs" - directiry with sources, all source files in this directory added (without recursion)
# "files" - raw list of source files
# "output" - output path prefix

import sys
import os
from os.path import join
sys.path.insert(0, join(os.getcwd(), "../make_py"))
import make_sources

STM_LIB_DIR = "../std_lib"
QT_PROJECT_NAME = "BootLoader"

sources = [
{
	"base" : STM_LIB_DIR+"/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7",
	"output" : "CMSIS",
	"files" : [
		"startup_stm32f4xx.S"
		]

},
{
	"base" : ".",
	"output" : "base",
	"dirs" : [
		"src",
	]
},
{
	"base" : "../4code/src",
	"output" : "4code/src",
	"files" : [
		"delay.c",
		"quadrature_encoder.c",
		"jump_to_app.c",
	]
},
{
	"base" : "../4code/src/ili",
	"output" : "4code/src/ili",
	"files" : [
		"DefaultFonts.c",
		"UTFT.c",
		"utf_font.c",
		"float_to_string.c"
	]
},
{
	"base" : "../4code/src/hardware",
	"output" : "4code/src/hardware",
	"files" : [
		"hw_ili9481.c",
	]
},
{
	"base" : "../4code/src",
	"output" : "4code/src",
	"dirs" : [
		"usart",
	]
},
{
	"base" : "../shared_code",
	"output" : "shared_code",
	"files" : [
		"7bit.c",
		"md5.c",
		"ring_buffer.c",
	]
},
{
	"base" : STM_LIB_DIR+"/STM32F4xx_StdPeriph_Driver/src",
	"output" : "StdPeriph_Driver",
	"files" : [
		"misc.c",
		"stm32f4xx_dma.c",
		"stm32f4xx_flash.c",
		"stm32f4xx_gpio.c",
		"stm32f4xx_rcc.c",
		"stm32f4xx_spi.c",
		"stm32f4xx_syscfg.c",
		"stm32f4xx_tim.c",
		"stm32f4xx_usart.c",
		]
}
]

if __name__ == "__main__":    
	make_sources.makeProject(sources, QT_PROJECT_NAME)
