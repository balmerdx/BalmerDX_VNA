#include "measure_freq.h"
#include "process_sound.h"

float MeasureFrequency(int32_t* samples, uint32_t samples_count, uint32_t sample_frequency)
{
    int64_t ysum = 0;
    int32_t ymax = 0;
    int32_t ymin = 0;

    ymin = ymax = samples[0];

    for(uint32_t i=0; i<samples_count; i++)
    {
        int32_t y = samples[i];
        ysum += y;
        if(y<ymin)
            ymin = y;
        if(y>ymax)
            ymax = y;
    }

    int32_t ymid = (ysum+samples_count/2)/samples_count;

    //считаем количество пересечений нуля с гистерезисом
    int tstart = -1;
    int tstop = -1;
    int tcount = 0;

    float delta = 0.1;
    bool prev_is_inited = false;
    bool prev_is_plus = false;

    int32_t deltam = (int32_t)((ymax-ymin)*-delta)+ymid;
    int32_t deltap = (int32_t)((ymax-ymin)*delta)+ymid;

    for(uint32_t i=0; i<samples_count; i++)
    {
        int32_t y = samples[i];
        bool is_plus = (y+ymid)>0;
        if(y>deltam && y<deltap)
            continue;

        if(!prev_is_inited)
        {
            prev_is_inited = true;
            prev_is_plus = is_plus;
            continue;
        }

        if(prev_is_plus == is_plus)
            continue;

        if(is_plus)
        {
            if(tstart==-1)
            {
                tstart = i;
            } else
            {
                tstop = i;
                tcount++;
            }
        }

        prev_is_plus = is_plus;
    }

    if(tcount==0)
        return 0;

    return (tcount * sample_frequency)/(float)(tstop-tstart);
}

float MeasureFrequencyRef()
{
    return MeasureFrequency(g_samplesQ, SAMPLE_BUFFER_SIZE, SAMPLE_FREQUENCY);
}


void MeasureFrequencyStart(MeasureFreqData* data, int32_t count_calculate_mid, uint32_t sample_frequency)
{
    data->count_calculate_mid = count_calculate_mid;
    data->sample_frequency = sample_frequency;
    data->ysum = 0;
    data->ymax = 0;
    data->ymin = 0;
    data->ymid = 0;

    data->count_received_samples = 0;

    data->tstart = -1;
    data->tstop = -1;
    data->tcount = 0;

    data->prev_is_inited = false;
    data->prev_is_plus = false;
}


void MeasureFrequencyQuant(MeasureFreqData* data, int32_t sample)
{
    if(data->count_received_samples<data->count_calculate_mid)
    {
        if(data->count_received_samples==0)
            data->ymin = data->ymax = sample;
        int32_t y = sample;
        data->ysum += y;
        if(y<data->ymin)
            data->ymin = y;
        if(y>data->ymax)
            data->ymax = y;

        if(data->count_received_samples+1==data->count_calculate_mid)
        {
            data->ymid = (data->ysum+data->count_calculate_mid/2)/data->count_calculate_mid;
            float delta = 0.1;
            data->deltam = (int32_t)((data->ymax-data->ymin)*-delta)+data->ymid;
            data->deltap = (int32_t)((data->ymax-data->ymin)*delta)+data->ymid;
        }
    } else
    {
        int32_t y = sample;
        bool is_plus = (y+data->ymid)>0;
        if(y>data->deltam && y<data->deltap)
            goto End;

        if(!data->prev_is_inited)
        {
            data->prev_is_inited = true;
            data->prev_is_plus = is_plus;
            goto End;
        }

        if(data->prev_is_plus == is_plus)
            goto End;

        if(is_plus)
        {
            if(data->tstart==-1)
            {
                data->tstart = data->count_received_samples;
            } else
            {
                data->tstop = data->count_received_samples;
                data->tcount++;
            }
        }

        data->prev_is_plus = is_plus;
    }
End:;
    data->count_received_samples++;
}

float MeasureFrequencyValue(MeasureFreqData* data)
{
    if(data->tcount==0)
        return 0;
    return ((int64_t)data->tcount * (int64_t)data->sample_frequency)/(float)(data->tstop-data->tstart);
}
