#include "main.h"
#include "job.h"
#include "process_sound.h"
#include "smath.h"
#include "commands.h"
#include "delay.h"


static JOB_STATE g_state = JOB_NONE;

//Ожидаем WAIT_TIME_US микросекунд, прежде чем начать сэмплирование.
#define WAIT_TIME_US 50000

static uint16_t g_start_wait_time;

static float result_freq = 0;
static float result_q_cconst = 0;
static float result_q_csin = 0;
static float result_q_ccos = 0;
static float result_q_sqr = 0;
static float result_i_cconst = 0;
static float result_i_csin = 0;
static float result_i_ccos = 0;
static float result_i_sqr = 0;
static float result_time = 1234; //Время выполнения

static int g_cur_raw_data_index = 0;
static int g_i_mean = 0;

void CalculateAll();
void UsbSetFreq(uint32_t freq, int32_t level0, int32_t level1);
void JobStartSamplingInternal();

JOB_STATE JobState()
{
	return g_state;
}

void JobSetState(JOB_STATE state)
{
	g_state = state;
}

void JobQuant()
{
    if(g_state==JOB_WAIT_BEFORE_SAMPLING)
    {
        uint16_t delta_us = TimeUs()-g_start_wait_time;
        if(delta_us>WAIT_TIME_US)
        {
            g_state = JOB_SAMPLING;
            SamplingStart();
        }
    }

    if(g_state==JOB_SAMPLING)
	{
		if(SamplingCompleted())
		{
			JobSetState(JOB_CALCULATING);
			CalculateAll();
            g_i_mean++;
            if(g_i_mean < raw.n_mean)
            {
                g_state = JOB_SAMPLING;
                SamplingStart();
            } else
            {
                g_i_mean = 0;
                g_cur_raw_data_index++;
                if(g_cur_raw_data_index<raw.s_param_count)
                {
                    JobStartSamplingInternal();
                } else
                {
                    if(isTX())
                        raw.s21_valid = true;
                    else
                        raw.s11_valid = true;
                    JobSetState(JOB_CALCULATING_COMPLETE);
                }
            }
		}
	}
}

void CalculateAll()
{
	uint16_t startTime = TimeMs();
    float step = 1.0f/SAMPLE_FREQUENCY;

    //old values
    //float freqMin = 999.3f;
    //float freqMax = 999.8f;
    //float freq = 999.6f;

    //new values
    //float freqMin = 999.4f;
    //float freqMax = 1000.1f;
    //int find_count = 11;

    //float freqMin = 998.9f;
    //float freqMax = 1000.0f;
    //int find_count = 11;
    float freq = 1000.0f;

	float q_cconst = 1, q_csin = 2, q_ccos = 3;
	float i_cconst = 4, i_csin = 5, i_ccos = 6;
	float q_sqr = 7;
	float i_sqr = 8;
    //freq = findFreqMax(g_samplesQ, SAMPLE_BUFFER_SIZE, step, freqMin, freqMax, find_count);
    //freq = 999.7747f; //first
    //freq = 999.8105f; //second
    freq = GetIntermediateFrequency();
	
	calcSinCosMatrix(g_samplesQ, SAMPLE_BUFFER_SIZE, freq, step, &q_cconst, &q_csin, &q_ccos);
	q_sqr = squareMean(g_samplesQ, SAMPLE_BUFFER_SIZE, freq, step, q_cconst, q_csin, q_ccos);

	calcSinCosMatrix(g_samplesI, SAMPLE_BUFFER_SIZE, freq, step, &i_cconst, &i_csin, &i_ccos);
	i_sqr = squareMean(g_samplesI, SAMPLE_BUFFER_SIZE, freq, step, i_cconst, i_csin, i_ccos);

	result_freq = freq;
	result_q_cconst = q_cconst;
	result_q_csin = q_csin;
	result_q_ccos = q_ccos;
	result_q_sqr = q_sqr;
	result_i_cconst = i_cconst;
	result_i_csin = i_csin;
	result_i_ccos = i_ccos;
	result_i_sqr = i_sqr;

    complexf Zi = i_ccos + i_csin*I;
    complexf Zq = q_ccos + q_csin*I;
    complexf Z = Zi/Zq;

    float qabs = cabs(Zq);
    float err = i_sqr/qabs;

    SParam* param = raw.s_param + g_cur_raw_data_index;

    Z *= 1.0f/raw.n_mean;
    err *= 1.0f/raw.n_mean;

    if(isTX())
    {
        if(g_i_mean==0)
        {
            param->S21 = Z;
            param->S21err = err;
        } else
        {
            param->S21 += Z;
            param->S21err += err;
        }
    } else
    {
        if(g_i_mean==0)
        {
            param->S11 = Z;
            param->S11err = err;
        } else
        {
            param->S11 += Z;
            param->S11err += err;
        }
    }

    param->ref_amplitude = qabs;

    result_time = TimeMs() - startTime;
}

void JobSendCalculated()
{
    DataAdd8(COMMAND_GET_CALCULATED);
    DataAdd8(g_state);

    if(g_state==JOB_CALCULATING_COMPLETE)
    {
        DataAddF(result_freq);
        DataAddF(result_q_cconst);
        DataAddF(result_q_csin);
        DataAddF(result_q_ccos);
        DataAddF(result_q_sqr);
        DataAddF(result_i_cconst);
        DataAddF(result_i_csin);
        DataAddF(result_i_ccos);
        DataAddF(result_i_sqr);
        DataAdd16(result_time);
        DataAdd16(0);
    }
    DataSend();
}

//levelRF - уровень, подаваемый на деталь
//levelLO - уровень сигнала для смесителя
void SetFreqWithLevel(uint32_t freq, int32_t levelRF, int32_t levelLO)
{
    uint32_t freqWord = AD9958_Calc_FrequencyWord(freq);
    uint32_t freqWordAdd = AD9958_Calc_FrequencyWord(1000);

#ifdef DISPLAY_ILI9481
    const int channelF = 1;
    const int channelF_1000 = 0;
#else
    const int channelF = 0;
    const int channelF_1000 = 1;
#endif

    AD9958_Set_FrequencyWord(channelF, freqWord);
    AD9958_Set_Level(channelF, levelRF);
    AD9958_Set_FrequencyWord(channelF_1000, freqWord+freqWordAdd);
    AD9958_Set_Level(channelF_1000, levelLO);
}

void SetFreqWithCalibration(freq_type freq)
{
#ifdef DISPLAY_ILI9481
    float fmin = 80e6;
    float fmax = 150e6;
    uint32_t level_min = 250;
    uint32_t level_max = 511;
#else
    //На частоте 10 МГц 300, на частоте 100 МГц 511
    float fmin = 10e6;
    float fmax = 100e6;
    uint32_t level_min = 300;
    uint32_t level_max = 511;
#endif
    float a = (freq - fmin)/(fmax - fmin);
    int32_t levelRF = level_min;
    if(freq<=fmin)
    {
        levelRF = (level_min*g_relative_level_rf)/MAX_RELATIVE_LEVEL_RF;
    } else
    if(freq>=fmax)
    {
        levelRF = (level_max*g_relative_level_rf)/MAX_RELATIVE_LEVEL_RF;;
    } else
    {
        levelRF = ((level_max-level_min)*a + level_min)*g_relative_level_rf/MAX_RELATIVE_LEVEL_RF;;
    }

    SetFreqWithLevel(freq, levelRF, 511);
}

void JobSetFreq()
{
    if(g_cur_raw_data_index>=MAX_FREQUENCIES)
        return;

    freq_type freq = raw.s_param[g_cur_raw_data_index].freq;
    SetFreqWithCalibration(freq);
}

void JobStartSamplingInternal()
{
    g_state = JOB_WAIT_BEFORE_SAMPLING;
    g_start_wait_time = TimeUs();
    JobSetFreq();
}

void JobStartSampling()
{
    g_cur_raw_data_index = 0;
    g_i_mean = 0;
    if(isTX())
        raw.s21_valid = false;
    else
        raw.s11_valid = false;
    JobStartSamplingInternal();
}

int JobCurrentDataIndex()
{
    return g_cur_raw_data_index;
}

float JobResultFreq()
{
    return result_freq;
}
