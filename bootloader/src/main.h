//balmer@inbox.ru
//STM32F405+CS4272 DSP module
  
#pragma once

#include <stdio.h>
#include <math.h>

#include "stm32f4xx_conf.h"
#include "ili/UTFT.h"
#include "ili/utf_font.h"
#include "ili/DefaultFonts.h"
//#include "interface/font_condensed30.h"
#include "delay.h"

bool checkProgramMd5();

void DrawStartProgram(bool ok);
void DrawWriteProgress(uint32_t offset, uint32_t program_size);
void DrawEraseFlash(uint32_t program_size);
