#include "dac.h"
#include "cs4272.h"
#include "process_sound.h"
#include "arm_math.h"
#include "delay.h"

uint16_t* DacGetBuffer();
uint16_t DacGetBufferSize();
void CalculateFft();

static uint16_t g_cur_pos = DAC_BUFFER_SIZE/2;
static uint16_t g_dma_cur_pos = 0;

uint16_t g_sound_quant_time = 0;

int32_t g_samplesQ[SAMPLE_BUFFER_SIZE];
int32_t g_samplesI[SAMPLE_BUFFER_SIZE];
static uint32_t samplesCurPos = SAMPLE_BUFFER_SIZE;

static SoundCallback g_sound_callback = NULL;

void OnSoundDataFft(int32_t sampleQ, int32_t sampleI);

uint16_t DacGetWritePos()
{
	return g_cur_pos;
}

uint16_t DacGetDeltaPos()
{
	uint16_t pos_in = DacGetWritePos();
	uint16_t pos_out = DacGetReadPos();
	uint16_t pos_delta;
	if(pos_out>pos_in)
	{
		pos_delta = pos_out-pos_in;
	} else
	{
		pos_delta = DAC_BUFFER_SIZE+pos_out-pos_in;
	}
	return pos_delta;
	//return (uint16_t)summary_adc_samples-(uint16_t)summary_dac_samples;
}

void OnSoundData(int32_t sampleQ, int32_t sampleI)
{
    if(g_sound_callback)
    {
        g_sound_callback(sampleQ, sampleI);
        return;
    }

    if(samplesCurPos<SAMPLE_BUFFER_SIZE)
	{
		g_samplesQ[samplesCurPos] = sampleQ;
		g_samplesI[samplesCurPos] = sampleI;
		samplesCurPos++;
		return;
	}

	if(0)//Write to DAC
	{
		uint16_t* out_buffer = DacGetBuffer();

		int s;
		s = (sampleQ>>10)+DAC_ZERO;
		//s = (sampleQ>>(14))+DAC_ZERO;
		//s = (sampleQ>>16)+DAC_ZERO;
		//s = (sampleQ>>20)+DAC_ZERO;

		if(s<0)
			s = 0;
		if(s>4095)
			s=4095;
		out_buffer[g_cur_pos] = s;

		g_cur_pos = (g_cur_pos+1)%DAC_BUFFER_SIZE;
	}
}

void CopySoundData(uint16_t start, uint16_t count)
{
	uint16_t* data4 = sound_buffer+start;
	for(int idx=0; idx<count; idx+=4, data4+=4)
	{
        int32_t channel0 =(((int32_t)data4[0])<<16)+data4[1];
        int32_t channel1 =(((int32_t)data4[2])<<16)+data4[3];

#ifdef DISPLAY_ILI9481
        OnSoundData(channel1, channel0);
#else
        OnSoundData(channel0, channel1);
#endif
	}
}

void SoundQuant()
{
	if(!g_i2s_dma)
		return;
	uint16_t start = TimeUs();

	uint16_t pos = cs4272_getPos();

	if(g_dma_cur_pos==pos)
	{
	} else
	if(g_dma_cur_pos<pos)
	{
		//pos = (pos+SOUND_BUFFER_SIZE-4)%SOUND_BUFFER_SIZE;
		if(g_dma_cur_pos<pos)
		{
			CopySoundData(g_dma_cur_pos, pos-g_dma_cur_pos);
			g_dma_cur_pos = pos;
		}
	} else
	{
		//pos = (pos+SOUND_BUFFER_SIZE-4)%SOUND_BUFFER_SIZE;
		if(pos<g_dma_cur_pos)
		{
			CopySoundData(g_dma_cur_pos, SOUND_BUFFER_SIZE-g_dma_cur_pos);
			CopySoundData(0, pos);
			g_dma_cur_pos = pos;
		}
	}

	uint16_t quant_time = TimeUs()-start;
	if(quant_time>5)
		g_sound_quant_time = quant_time;
}

void SamplingStart()
{
	samplesCurPos = 0;	
}

bool SamplingCompleted()
{
	return samplesCurPos>=SAMPLE_BUFFER_SIZE;
}

void SetOnSouncCallback(SoundCallback callback)
{
    g_sound_callback = callback;
}
