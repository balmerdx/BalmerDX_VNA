################################################################################
# Automatically-generated file. Do not edit!
################################################################################


DEPS += \
	output/CMSIS/startup_stm32f4xx.d \
	output/base/src/ad9958_drv.d \
	output/base/src/bootloader_version.d \
	output/base/src/calculate_lc.d \
	output/base/src/calibration_solt.d \
	output/base/src/cs4272.d \
	output/base/src/dac.d \
	output/base/src/data.d \
	output/base/src/data_draw.d \
	output/base/src/delay.d \
	output/base/src/job.d \
	output/base/src/jump_to_app.d \
	output/base/src/main.d \
	output/base/src/measure_freq.d \
	output/base/src/packet_receive.d \
	output/base/src/process_sound.d \
	output/base/src/quadrature_encoder.d \
	output/base/src/smath.d \
	output/base/src/stm32f4xx_it.d \
	output/base/src/store_user_settings.d \
	output/base/src/system_stm32f4xx.d \
	output/base/src/ili/DefaultFonts.d \
	output/base/src/ili/UTFT.d \
	output/base/src/ili/float_to_string.d \
	output/base/src/ili/utf_font.d \
	output/base/src/interface/edit_int.d \
	output/base/src/interface/font_condensed30.d \
	output/base/src/interface/font_condensed59.d \
	output/base/src/interface/interface.d \
	output/base/src/interface/menu.d \
	output/base/src/interface/plot.d \
	output/base/src/interface/progress_bar.d \
	output/base/src/interface/scene_about.d \
	output/base/src/interface/scene_check_intermediate_frequency.d \
	output/base/src/interface/scene_debug_menu.d \
	output/base/src/interface/scene_graph.d \
	output/base/src/interface/scene_graph_params.d \
	output/base/src/interface/scene_graph_select.d \
	output/base/src/interface/scene_if_calibration.d \
	output/base/src/interface/scene_measure_freq.d \
	output/base/src/interface/scene_menu.d \
	output/base/src/interface/scene_name.d \
	output/base/src/interface/scene_select_calibration.d \
	output/base/src/interface/scene_settings.d \
	output/base/src/interface/scene_single_freq.d \
	output/base/src/interface/scene_solt_calibration.d \
	output/base/src/interface/srlc_format.d \
	output/base/src/interface/statusbar.d \
	output/base/src/hardware/hw_ili9341.d \
	output/base/src/hardware/hw_ili9481.d \
	output/base/src/hardware/m25p16.d \
	output/base/src/hardware/store_to_spi_flash.d \
	output/base/src/hardware/store_to_stm32_flash.d \
	output/shared/7bit.d \
	output/shared/ring_buffer.d \
	output/StdPeriph_Driver/misc.d \
	output/StdPeriph_Driver/stm32f4xx_dac.d \
	output/StdPeriph_Driver/stm32f4xx_dma.d \
	output/StdPeriph_Driver/stm32f4xx_exti.d \
	output/StdPeriph_Driver/stm32f4xx_flash.d \
	output/StdPeriph_Driver/stm32f4xx_gpio.d \
	output/StdPeriph_Driver/stm32f4xx_i2c.d \
	output/StdPeriph_Driver/stm32f4xx_rcc.d \
	output/StdPeriph_Driver/stm32f4xx_spi.d \
	output/StdPeriph_Driver/stm32f4xx_syscfg.d \
	output/StdPeriph_Driver/stm32f4xx_tim.d \
	output/FFT/CommonTables/arm_common_tables.d \
	output/FFT/ComplexMathFunctions/arm_cmplx_mag_f32.d \
	output/FFT/TransformFunctions/arm_cfft_radix4_f32.d \
	output/FFT/TransformFunctions/arm_cfft_radix4_init_f32.d \
	output/FFT/TransformFunctions/arm_rfft_init_f32.d \
	output/FFT/TransformFunctions/arm_rfft_f32.d \
	output/base/src/usart/usart.d \
	output/StdPeriph_Driver/stm32f4xx_usart.d \


OBJS += \
	output/CMSIS/startup_stm32f4xx.o \
	output/base/src/ad9958_drv.o \
	output/base/src/bootloader_version.o \
	output/base/src/calculate_lc.o \
	output/base/src/calibration_solt.o \
	output/base/src/cs4272.o \
	output/base/src/dac.o \
	output/base/src/data.o \
	output/base/src/data_draw.o \
	output/base/src/delay.o \
	output/base/src/job.o \
	output/base/src/jump_to_app.o \
	output/base/src/main.o \
	output/base/src/measure_freq.o \
	output/base/src/packet_receive.o \
	output/base/src/process_sound.o \
	output/base/src/quadrature_encoder.o \
	output/base/src/smath.o \
	output/base/src/stm32f4xx_it.o \
	output/base/src/store_user_settings.o \
	output/base/src/system_stm32f4xx.o \
	output/base/src/ili/DefaultFonts.o \
	output/base/src/ili/UTFT.o \
	output/base/src/ili/float_to_string.o \
	output/base/src/ili/utf_font.o \
	output/base/src/interface/edit_int.o \
	output/base/src/interface/font_condensed30.o \
	output/base/src/interface/font_condensed59.o \
	output/base/src/interface/interface.o \
	output/base/src/interface/menu.o \
	output/base/src/interface/plot.o \
	output/base/src/interface/progress_bar.o \
	output/base/src/interface/scene_about.o \
	output/base/src/interface/scene_check_intermediate_frequency.o \
	output/base/src/interface/scene_debug_menu.o \
	output/base/src/interface/scene_graph.o \
	output/base/src/interface/scene_graph_params.o \
	output/base/src/interface/scene_graph_select.o \
	output/base/src/interface/scene_if_calibration.o \
	output/base/src/interface/scene_measure_freq.o \
	output/base/src/interface/scene_menu.o \
	output/base/src/interface/scene_name.o \
	output/base/src/interface/scene_select_calibration.o \
	output/base/src/interface/scene_settings.o \
	output/base/src/interface/scene_single_freq.o \
	output/base/src/interface/scene_solt_calibration.o \
	output/base/src/interface/srlc_format.o \
	output/base/src/interface/statusbar.o \
	output/base/src/hardware/hw_ili9341.o \
	output/base/src/hardware/hw_ili9481.o \
	output/base/src/hardware/m25p16.o \
	output/base/src/hardware/store_to_spi_flash.o \
	output/base/src/hardware/store_to_stm32_flash.o \
	output/shared/7bit.o \
	output/shared/ring_buffer.o \
	output/StdPeriph_Driver/misc.o \
	output/StdPeriph_Driver/stm32f4xx_dac.o \
	output/StdPeriph_Driver/stm32f4xx_dma.o \
	output/StdPeriph_Driver/stm32f4xx_exti.o \
	output/StdPeriph_Driver/stm32f4xx_flash.o \
	output/StdPeriph_Driver/stm32f4xx_gpio.o \
	output/StdPeriph_Driver/stm32f4xx_i2c.o \
	output/StdPeriph_Driver/stm32f4xx_rcc.o \
	output/StdPeriph_Driver/stm32f4xx_spi.o \
	output/StdPeriph_Driver/stm32f4xx_syscfg.o \
	output/StdPeriph_Driver/stm32f4xx_tim.o \
	output/FFT/CommonTables/arm_common_tables.o \
	output/FFT/ComplexMathFunctions/arm_cmplx_mag_f32.o \
	output/FFT/TransformFunctions/arm_cfft_radix4_f32.o \
	output/FFT/TransformFunctions/arm_cfft_radix4_init_f32.o \
	output/FFT/TransformFunctions/arm_rfft_init_f32.o \
	output/FFT/TransformFunctions/arm_rfft_f32.o \
	output/base/src/usart/usart.o \
	output/StdPeriph_Driver/stm32f4xx_usart.o \


output/CMSIS/startup_stm32f4xx.o: ../std_lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f4xx.S
	@echo 'Building target: startup_stm32f4xx.S'
	@$(CC) $(ASM_FLAGS) -o "$@" "$<"

output/base/src/ad9958_drv.o: ./src/ad9958_drv.c
	@echo 'Building target: ad9958_drv.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/bootloader_version.o: ./src/bootloader_version.c
	@echo 'Building target: bootloader_version.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/calculate_lc.o: ./src/calculate_lc.c
	@echo 'Building target: calculate_lc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/calibration_solt.o: ./src/calibration_solt.c
	@echo 'Building target: calibration_solt.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/cs4272.o: ./src/cs4272.c
	@echo 'Building target: cs4272.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/dac.o: ./src/dac.c
	@echo 'Building target: dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/data.o: ./src/data.c
	@echo 'Building target: data.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/data_draw.o: ./src/data_draw.c
	@echo 'Building target: data_draw.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/delay.o: ./src/delay.c
	@echo 'Building target: delay.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/job.o: ./src/job.c
	@echo 'Building target: job.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/jump_to_app.o: ./src/jump_to_app.c
	@echo 'Building target: jump_to_app.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/main.o: ./src/main.c
	@echo 'Building target: main.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/measure_freq.o: ./src/measure_freq.c
	@echo 'Building target: measure_freq.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/packet_receive.o: ./src/packet_receive.c
	@echo 'Building target: packet_receive.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/process_sound.o: ./src/process_sound.c
	@echo 'Building target: process_sound.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/quadrature_encoder.o: ./src/quadrature_encoder.c
	@echo 'Building target: quadrature_encoder.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/smath.o: ./src/smath.c
	@echo 'Building target: smath.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/stm32f4xx_it.o: ./src/stm32f4xx_it.c
	@echo 'Building target: stm32f4xx_it.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/store_user_settings.o: ./src/store_user_settings.c
	@echo 'Building target: store_user_settings.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/system_stm32f4xx.o: ./src/system_stm32f4xx.c
	@echo 'Building target: system_stm32f4xx.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/DefaultFonts.o: ./src/ili/DefaultFonts.c
	@echo 'Building target: DefaultFonts.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/UTFT.o: ./src/ili/UTFT.c
	@echo 'Building target: UTFT.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/float_to_string.o: ./src/ili/float_to_string.c
	@echo 'Building target: float_to_string.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/ili/utf_font.o: ./src/ili/utf_font.c
	@echo 'Building target: utf_font.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/edit_int.o: ./src/interface/edit_int.c
	@echo 'Building target: edit_int.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/font_condensed30.o: ./src/interface/font_condensed30.c
	@echo 'Building target: font_condensed30.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/font_condensed59.o: ./src/interface/font_condensed59.c
	@echo 'Building target: font_condensed59.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/interface.o: ./src/interface/interface.c
	@echo 'Building target: interface.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/menu.o: ./src/interface/menu.c
	@echo 'Building target: menu.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/plot.o: ./src/interface/plot.c
	@echo 'Building target: plot.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/progress_bar.o: ./src/interface/progress_bar.c
	@echo 'Building target: progress_bar.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_about.o: ./src/interface/scene_about.c
	@echo 'Building target: scene_about.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_check_intermediate_frequency.o: ./src/interface/scene_check_intermediate_frequency.c
	@echo 'Building target: scene_check_intermediate_frequency.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_debug_menu.o: ./src/interface/scene_debug_menu.c
	@echo 'Building target: scene_debug_menu.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_graph.o: ./src/interface/scene_graph.c
	@echo 'Building target: scene_graph.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_graph_params.o: ./src/interface/scene_graph_params.c
	@echo 'Building target: scene_graph_params.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_graph_select.o: ./src/interface/scene_graph_select.c
	@echo 'Building target: scene_graph_select.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_if_calibration.o: ./src/interface/scene_if_calibration.c
	@echo 'Building target: scene_if_calibration.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_measure_freq.o: ./src/interface/scene_measure_freq.c
	@echo 'Building target: scene_measure_freq.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_menu.o: ./src/interface/scene_menu.c
	@echo 'Building target: scene_menu.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_name.o: ./src/interface/scene_name.c
	@echo 'Building target: scene_name.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_select_calibration.o: ./src/interface/scene_select_calibration.c
	@echo 'Building target: scene_select_calibration.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_settings.o: ./src/interface/scene_settings.c
	@echo 'Building target: scene_settings.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_single_freq.o: ./src/interface/scene_single_freq.c
	@echo 'Building target: scene_single_freq.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/scene_solt_calibration.o: ./src/interface/scene_solt_calibration.c
	@echo 'Building target: scene_solt_calibration.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/srlc_format.o: ./src/interface/srlc_format.c
	@echo 'Building target: srlc_format.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/interface/statusbar.o: ./src/interface/statusbar.c
	@echo 'Building target: statusbar.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hardware/hw_ili9341.o: ./src/hardware/hw_ili9341.c
	@echo 'Building target: hw_ili9341.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hardware/hw_ili9481.o: ./src/hardware/hw_ili9481.c
	@echo 'Building target: hw_ili9481.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hardware/m25p16.o: ./src/hardware/m25p16.c
	@echo 'Building target: m25p16.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hardware/store_to_spi_flash.o: ./src/hardware/store_to_spi_flash.c
	@echo 'Building target: store_to_spi_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/hardware/store_to_stm32_flash.o: ./src/hardware/store_to_stm32_flash.c
	@echo 'Building target: store_to_stm32_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/shared/7bit.o: ../shared_code/7bit.c
	@echo 'Building target: 7bit.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/shared/ring_buffer.o: ../shared_code/ring_buffer.c
	@echo 'Building target: ring_buffer.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/misc.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/misc.c
	@echo 'Building target: misc.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_dac.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
	@echo 'Building target: stm32f4xx_dac.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_dma.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
	@echo 'Building target: stm32f4xx_dma.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_exti.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
	@echo 'Building target: stm32f4xx_exti.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_flash.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
	@echo 'Building target: stm32f4xx_flash.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_gpio.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
	@echo 'Building target: stm32f4xx_gpio.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_i2c.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
	@echo 'Building target: stm32f4xx_i2c.c'
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

output/FFT/CommonTables/arm_common_tables.o: ../std_lib/CMSIS/DSP_Lib/Source/CommonTables/arm_common_tables.c
	@echo 'Building target: arm_common_tables.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/ComplexMathFunctions/arm_cmplx_mag_f32.o: ../std_lib/CMSIS/DSP_Lib/Source/ComplexMathFunctions/arm_cmplx_mag_f32.c
	@echo 'Building target: arm_cmplx_mag_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_cfft_radix4_f32.o: ../std_lib/CMSIS/DSP_Lib/Source/TransformFunctions/arm_cfft_radix4_f32.c
	@echo 'Building target: arm_cfft_radix4_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_cfft_radix4_init_f32.o: ../std_lib/CMSIS/DSP_Lib/Source/TransformFunctions/arm_cfft_radix4_init_f32.c
	@echo 'Building target: arm_cfft_radix4_init_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_rfft_init_f32.o: ../std_lib/CMSIS/DSP_Lib/Source/TransformFunctions/arm_rfft_init_f32.c
	@echo 'Building target: arm_rfft_init_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/FFT/TransformFunctions/arm_rfft_f32.o: ../std_lib/CMSIS/DSP_Lib/Source/TransformFunctions/arm_rfft_f32.c
	@echo 'Building target: arm_rfft_f32.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/base/src/usart/usart.o: ./src/usart/usart.c
	@echo 'Building target: usart.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"

output/StdPeriph_Driver/stm32f4xx_usart.o: ../std_lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
	@echo 'Building target: stm32f4xx_usart.c'
	@$(CC) $(C_FLAGS) -o "$@" "$<"



