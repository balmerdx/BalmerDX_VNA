RM := rm -rf
TC=${ARM_NONE_EABI}/arm-none-eabi
CC=$(TC)-gcc
LD=$(TC)-ld -v
OBJCOPY=$(TC)-objcopy
AR=$(TC)-ar
GDB=$(TC)-gdb

STM_LIB_DIR = $(CURDIR)/../std_lib

C_INCLIDES= -I$(STM_LIB_DIR)/CMSIS/ST/STM32F4xx/Include
C_INCLIDES+= -I$(STM_LIB_DIR)/STM32F4xx_StdPeriph_Driver/inc
C_INCLIDES+= -I$(STM_LIB_DIR)/CMSIS/Include
C_INCLIDES+= -I$(STM_LIB_DIR)/CMSIS/Device/ST/STM32F4xx/Include
C_INCLIDES+= -Isrc
C_INCLIDES+= -includesrc/stm32f4xx_conf.h
C_INCLIDES+= -Ilib/USB_OTG/inc
C_INCLIDES+= -Ilib/Conf
C_INCLIDES+= -Ilib/USB_Device/Core/inc
C_INCLIDES+= -Ilib/USB_Device/Class/cdc/inc
C_INCLIDES+= -I../shared_code

ifeq ($(MAKECMDGOALS),debug)
C_DEFINES= -DDEBUG
C_FLAGS= -O0 -g3
else
C_DEFINES= -DNDEBUG
C_FLAGS= -O3
endif

C_DEFINES+= -DHSE_VALUE=8000000
C_DEFINES+= -DSTM32F4XX
C_DEFINES+= -DARM_MATH_CM4
include defs.mk


C_FLAGS+= -std=c99
C_FLAGS+= -Wall -c -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD $(C_INCLIDES) $(C_DEFINES) 
#C_FLAGS+= -g -Wa,-adhls #asm listing

LDFLAGS = -nostartfiles -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

ifeq ($(MAKECMDGOALS),rboot)
LDFLAGS += -T "stm32_flash_bootload.ld"
else
LDFLAGS += -T "stm32_flash.ld"
endif

#LDFLAGS += -Wl,-Map=linker.map -Wl,-cref
LDFLAGS += -Wl,--gc-sections 
ASM_FLAGS = -x assembler-with-cpp -c -mthumb -mcpu=cortex-m4

TARGET=output/program

# All of the sources participating in the build are defined here
include sources.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

# All Target
#all: executable

#release: executable

#release for bootloader
rboot: executable

#debug: executable

# Tool invocations
executable: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	@$(TC)-g++ $(LDFLAGS) -o "$(TARGET)_$(MAKECMDGOALS)" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo 'Create Binary'
	$(OBJCOPY) -S  -O binary  "$(TARGET)_$(MAKECMDGOALS)" "$(TARGET)_$(MAKECMDGOALS).bin"
	@echo ' '

# Other Targets
clean:
	-@echo 'Clean files'	
	-@$(RM) $(OBJS)$(DEPS) $(TARGET)
	-@echo ' '

flashd:
	python /home/balmer/radio/stm32/tools/stm32_flash.py "${CURDIR}/$(TARGET)_debug.bin"

flashr:
	python /home/balmer/radio/stm32/tools/stm32_flash.py "${CURDIR}/$(TARGET)_release.bin"

.PHONY: all clean dependents
