RM := rm -rf
TC=${ARM_NONE_EABI}/arm-none-eabi
CC=$(TC)-gcc
LD=$(TC)-ld -v
OBJCOPY=$(TC)-objcopy
AR=$(TC)-ar
GDB=$(TC)-gdb

STM_LIB_DIR = "../std_lib"
C_INCLIDES= -I$(STM_LIB_DIR)/CMSIS/ST/STM32F4xx/Include
C_INCLIDES+= -I$(STM_LIB_DIR)/STM32F4xx_StdPeriph_Driver/inc
C_INCLIDES+= -I$(STM_LIB_DIR)/CMSIS/Include
C_INCLIDES+= -I$(STM_LIB_DIR)/CMSIS/Device/ST/STM32F4xx/Include
C_INCLIDES+= -Isrc
C_INCLIDES+= -I../4code/src
C_INCLIDES+= -includesrc/stm32f4xx_conf.h
C_INCLIDES+= -I../shared_code

ifeq ($(MAKECMDGOALS),debug)
C_DEFINES= -DDEBUG
C_FLAGS= -O0 -g3
endif

ifeq ($(MAKECMDGOALS),release)
C_DEFINES= -DNDEBUG
C_FLAGS= -O3
endif

C_DEFINES+= -DHSE_VALUE=8000000
C_DEFINES+= -std=c99

C_DEFINES+= -DSTM32F4XX
C_DEFINES+= -DARM_MATH_CM4
C_DEFINES+= -DDISPLAY_ILI9481

C_FLAGS+= -Wall -c -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD $(C_INCLIDES) $(C_DEFINES) 
C_FLAGS+= -ffunction-sections
C_FLAGS+= -fdata-sections

LDFLAGS = -nostartfiles -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
LDFLAGS += -T "stm32_flash.ld"  -Wl,--gc-sections 
#LDFLAGS += -Wl,-Map=linker.map -Wl,-cref 
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

release: executable

debug: executable

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
