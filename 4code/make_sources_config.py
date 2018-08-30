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
QT_PROJECT_NAME = "BalmerVNA"

use_DISPLAY_ILI9481 = True

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
		"inc",
		"src/ili",
		"src/interface",
		"src/hardware",
	]
},
{
	"base" : "../shared_code",
	"output" : "shared",
	"files" : [
		"7bit.c",
		"ring_buffer.c",
	]
},
{
	"base" : STM_LIB_DIR+"/STM32F4xx_StdPeriph_Driver/src",
	"output" : "StdPeriph_Driver",
	"files" : [
		"misc.c",
		"stm32f4xx_dac.c",
		"stm32f4xx_dma.c",
		"stm32f4xx_exti.c",
		"stm32f4xx_flash.c",
		"stm32f4xx_gpio.c",
		"stm32f4xx_i2c.c",
		"stm32f4xx_rcc.c",
		"stm32f4xx_spi.c",
		"stm32f4xx_syscfg.c",
		"stm32f4xx_tim.c"
		]
},
{
	"base" : STM_LIB_DIR+"/CMSIS/DSP_Lib/Source",
	"output" : "FFT",
	"files" : [
		"CommonTables/arm_common_tables.c",
		"ComplexMathFunctions/arm_cmplx_mag_f32.c",
		"TransformFunctions/arm_cfft_radix4_f32.c",
		"TransformFunctions/arm_cfft_radix4_init_f32.c",
		"TransformFunctions/arm_rfft_init_f32.c",
		"TransformFunctions/arm_rfft_f32.c"
		]
}
]

usb_sources = [
{
	"base" : ".",
	"output" : "base",
	"dirs" : [
		"src/usb",
		"lib/USB_OTG/src",
		"lib/USB_Device/Core/src",
		"lib/USB_Device/Class/cdc/src"
	]
},
]

usart_sources = [
{
	"base" : ".",
	"output" : "base",
	"dirs" : [
		"src/usart",
	]
},
{
	"base" : STM_LIB_DIR+"/STM32F4xx_StdPeriph_Driver/src",
	"output" : "StdPeriph_Driver",
	"files" : [
		"stm32f4xx_usart.c",
		]
},
]


if __name__ == "__main__":
	sum_sources = sources
	if use_DISPLAY_ILI9481:
		sum_sources = sources + usart_sources
	else:
		sum_sources = sources + usb_sources

	makeDefFile = open("defs.mk", "w")

	if use_DISPLAY_ILI9481:
		makeDefFile.write("C_DEFINES+= -DDISPLAY_ILI9481\n")
	else:
		pass

	makeDefFile.close()

	make_sources.makeProject(sum_sources, QT_PROJECT_NAME)
