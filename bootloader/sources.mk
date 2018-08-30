################################################################################
# Automatically-generated file. Do not edit!
################################################################################


DEPS += \
	output/CMSIS/startup_stm32f4xx.d \
	output/base/src/font_condensed30.d \
	output/base/src/main.d \
	output/base/src/packet_receive.d \
	output/base/src/stm32f4xx_it.d \
	output/base/src/system_stm32f4xx.d \
	output/base/src/write_flash.d \
	output/4code/src/delay.d \
	output/4code/src/quadrature_encoder.d \
	output/4code/src/jump_to_app.d \
	output/4code/src/ili/DefaultFonts.d \
	output/4code/src/ili/UTFT.d \
	output/4code/src/ili/utf_font.d \
	output/4code/src/ili/float_to_string.d \
	output/4code/src/hardware/hw_ili9481.d \
	output/4code/src/usart/usart.d \
	output/shared_code/7bit.d \
	output/shared_code/md5.d \
	output/shared_code/ring_buffer.d \
	output/StdPeriph_Driver/misc.d \
	output/StdPeriph_Driver/stm32f4xx_dma.d \
	output/StdPeriph_Driver/stm32f4xx_flash.d \
	output/StdPeriph_Driver/stm32f4xx_gpio.d \
	output/StdPeriph_Driver/stm32f4xx_rcc.d \
	output/StdPeriph_Driver/stm32f4xx_spi.d \
	output/StdPeriph_Driver/stm32f4xx_syscfg.d \
	output/StdPeriph_Driver/stm32f4xx_tim.d \
	output/StdPeriph_Driver/stm32f4xx_usart.d \


OBJS += \
	output/CMSIS/startup_stm32f4xx.o \
	output/base/src/font_condensed30.o \
	output/base/src/main.o \
	output/base/src/packet_receive.o \
	output/base/src/stm32f4xx_it.o \
	output/base/src/system_stm32f4xx.o \
	output/base/src/write_flash.o \
	output/4code/src/delay.o \
	output/4code/src/quadrature_encoder.o \
	output/4code/src/jump_to_app.o \
	output/4code/src/ili/DefaultFonts.o \
	output/4code/src/ili/UTFT.o \
	output/4code/src/ili/utf_font.o \
	output/4code/src/ili/float_to_string.o \
	output/4code/src/hardware/hw_ili9481.o \
	output/4code/src/usart/usart.o \
	output/shared_code/7bit.o \
	output/shared_code/md5.o \
	output/shared_code/ring_buffer.o \
	output/StdPeriph_Driver/misc.o \
	output/StdPeriph_Driver/stm32f4xx_dma.o \
	output/StdPeriph_Driver/stm32f4xx_flash.o \
	output/StdPeriph_Driver/stm32f4xx_gpio.o \
	output/StdPeriph_Driver/stm32f4xx_rcc.o \
	output/StdPeriph_Driver/stm32f4xx_spi.o \
	output/StdPeriph_Driver/stm32f4xx_syscfg.o \
	output/StdPeriph_Driver/stm32f4xx_tim.o \
	output/StdPeriph_Driver/stm32f4xx_usart.o \


output/CMSIS/startup_stm32f4xx.o: ../std_lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f4xx.S
	@echo 'Building target: startup_stm32f4xx.S'
	@$(CC) $(ASM_FLAGS) -o "$@" "$<"

output/base/src/font_condensed30.o: ./src/font_condensed30.c
	@echo 'Building target: font_condensed30.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/main.o: ./src/main.c
	@echo 'Building target: main.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/packet_receive.o: ./src/packet_receive.c
	@echo 'Building target: packet_receive.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/stm32f4xx_it.o: ./src/stm32f4xx_it.c
	@echo 'Building target: stm32f4xx_it.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/system_stm32f4xx.o: ./src/system_stm32f4xx.c
	@echo 'Building target: system_stm32f4xx.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/write_flash.o: ./src/write_flash.c
	@echo 'Building target: write_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/delay.o: ../4code/src/delay.c
	@echo 'Building target: delay.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/quadrature_encoder.o: ../4code/src/quadrature_encoder.c
	@echo 'Building target: quadrature_encoder.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/jump_to_app.o: ../4code/src/jump_to_app.c
	@echo 'Building target: jump_to_app.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/ili/DefaultFonts.o: ../4code/src/ili/DefaultFonts.c
	@echo 'Building target: DefaultFonts.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/ili/UTFT.o: ../4code/src/ili/UTFT.c
	@echo 'Building target: UTFT.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/ili/utf_font.o: ../4code/src/ili/utf_font.c
	@echo 'Building target: utf_font.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/ili/float_to_string.o: ../4code/src/ili/float_to_string.c
	@echo 'Building target: float_to_string.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/hardware/hw_ili9481.o: ../4code/src/hardware/hw_ili9481.c
	@echo 'Building target: hw_ili9481.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/4code/src/usart/usart.o: ../4code/src/usart/usart.c
	@echo 'Building target: usart.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/shared_code/7bit.o: ../shared_code/7bit.c
	@echo 'Building target: 7bit.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/shared_code/md5.o: ../shared_code/md5.c
	@echo 'Building target: md5.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/shared_code/ring_buffer.o: ../shared_code/ring_buffer.c
	@echo 'Building target: ring_buffer.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/misc.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/misc.c
	@echo 'Building target: misc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_dma.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
	@echo 'Building target: stm32f4xx_dma.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_flash.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
	@echo 'Building target: stm32f4xx_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_gpio.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
	@echo 'Building target: stm32f4xx_gpio.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_rcc.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
	@echo 'Building target: stm32f4xx_rcc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_spi.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
	@echo 'Building target: stm32f4xx_spi.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_syscfg.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
	@echo 'Building target: stm32f4xx_syscfg.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_tim.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
	@echo 'Building target: stm32f4xx_tim.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_usart.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
	@echo 'Building target: stm32f4xx_usart.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"



