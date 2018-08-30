#include "main.h"
#include "delay.h"

#include "ad9958_drv.h"
#include "process_sound.h"
#include "commands.h"
#include "process_sound.h"
#include "job.h"
#include "cs4272.h"
#include "data.h"

uint32_t pingIdx = 0;

void PacketReceive(volatile uint8_t* data, uint32_t size);
void OnGetCalibration(GET_CALIBRATION_ENUM type);

void UsartReceive(uint8_t* in_data, uint32_t in_size)
{
    PacketReceive(in_data, in_size);
}

void PacketReceive(volatile uint8_t* data, uint32_t size)
{
    if(size==0)
        return;
    uint8_t command = data[0];
    data++;
    size--;

    UTFT_setFont(FONT8x15);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);
    UTFT_printNumI(command, 0, 0, 6, ' ');
    UTFT_printNumI(size, 0, 16, 6, ' ');

    switch(command)
    {
    default:
        DataAdd8(command);
        //USBAdd8(size);
        DataAdd((uint8_t*)data, size);
        DataSend();
        break;
    case COMMAND_PING:
        pingIdx = *(uint32_t*)data;
        DataAdd8(command);
        DataAdd32(*(uint32_t*)data);
        DataAdd32(SELF_ID_VNA);
        DataSend();
        break;
    case COMMAND_BIG_DATA:
        {
            DataAdd8(command);
            uint16_t amin = ((uint16_t*)data)[0];
            uint16_t amax = ((uint16_t*)data)[1];
            for(uint16_t i=amin; i<amax; i++)
            {
                DataAdd16(i);
            }
            DataSend();
        }
        break;
    case COMMAND_SET_FREQ:
    	{
    		uint32_t freq = ((uint32_t*)data)[0];

            RawParamSetFreq(freq);

            DataAdd8(command);
            DataAdd32(freq);
            DataSend();
    	}
    	break;
    case COMMAND_START_SAMPLING:
    	{
            uint8_t prevComplete = SamplingCompleted();
			SamplingStart();
            DataAdd8(command);
            DataAdd8(prevComplete);
            DataSend();
        }
    	break;
    case COMMAND_SAMPLING_COMPLETE:
    	{
            DataAdd8(command);
            DataAdd8(SamplingCompleted());
            DataSend();
    	}
    	break;
    case COMMAND_SAMPLING_BUFFER_SIZE:
    	{
            DataAdd8(command);
            DataAdd16(SAMPLE_BUFFER_SIZE);
            DataSend();
    	}
    	break;
    case COMMAND_GET_SAMPLES:
    	{
    		uint8_t isQ = data[0];
            int offset = *(uint16_t*)(data+1);
            int count = *(uint16_t*)(data+3);
            if(offset<0 || count<0 || offset+count>SAMPLE_BUFFER_SIZE)
            {
            	//error!
                DataSend();
            	break;
            }

            for(uint16_t i=0; i<count; i++)
            {
            	if(isQ)
                    DataAdd32i(g_samplesQ[i+offset]);
            	else
                    DataAdd32i(g_samplesI[i+offset]);
            }

            DataSend();
    	}
    	break;
    case COMMAND_SET_TX:
    	{
    		uint8_t tx = data[0];
    		setTX(tx?1:0);

            DataAdd8(command);
            DataAdd8(tx);
            DataSend();
    	}
    	break;
    case COMMAND_GET_CALCULATED:
        {
            JobSendCalculated();
        }
        break;
    case COMMAND_START_SAMPLING_AND_CALCULATE:
        {
            bool ok = JobState()==JOB_NONE || JobState()==JOB_CALCULATING_COMPLETE;
            if(ok)
            {
                JobStartSampling();
            }

            DataAdd8(command);
            DataAdd8(ok);
            DataSend();
        }
        break;
    case COMMAND_SET_FREQ_LIST:
        {
            uint32_t count = *(uint32_t*)data;
            if(count>MAX_FREQUENCIES)
            {
                //error!
                DataSend();
                break;
            }

            RawParamReset(count);
            uint32_t* freq_data = 1+(uint32_t*)data;
            for(uint32_t i=0; i<count; i++)
            {
                SParam* p = raw.s_param + i;
                p->freq = freq_data[i];
                p->S11 = 0;
                p->S21 = 0;
                p->S11err = 0;
                p->S21err = 0;
            }

            DataAdd8(command);
            DataAdd32(raw.s_param_count);
            DataSend();
        }
        break;

    case COMMAND_JUMP_TO_BOOTLOADER:
        DataAdd8(command);
        DataSend();
        g_jump_to_bootloader = true;
        break;
    case COMMAND_GET_CALIBRATION:
        OnGetCalibration((GET_CALIBRATION_ENUM)data[0]);
        break;
    case COMMAND_GET_CALIBRATION_NAME:
        DataAdd8(command);
        for(int i=0; i<MAX_NAME_LEN+1; i++)
        {
            char c = g_calibration.name[i];
            DataAdd8(c);
            if(c==0)
                break;
        }
        DataSend();
        break;

    case COMMAND_SET_RELATIVE_LEVEL_RF:
        g_relative_level_rf = data[0];
        if(g_relative_level_rf>MAX_RELATIVE_LEVEL_RF)
            g_relative_level_rf = MAX_RELATIVE_LEVEL_RF;

        DataAdd8(command);
        DataAdd8(g_relative_level_rf);
        DataSend();
        break;
    case COMMAND_GET_RELATIVE_LEVEL_RF:
        DataAdd8(command);
        DataAdd8(g_relative_level_rf);
        DataSend();
        break;
    }
}

void OnGetCalibration(GET_CALIBRATION_ENUM type)
{
    DataAdd8(COMMAND_GET_CALIBRATION);

    if((type==GET_CALIBRATION_S11_OPEN && !g_calibration.open_valid)
     ||(type==GET_CALIBRATION_S21_OPEN && !g_calibration.open_valid)
     ||(type==GET_CALIBRATION_S11_SHORT && !g_calibration.short_valid)
     ||(type==GET_CALIBRATION_S11_LOAD && !g_calibration.load_valid)
     ||(type==GET_CALIBRATION_S11_THRU && !g_calibration.thru_valid)
     ||(type==GET_CALIBRATION_S21_THRU && !g_calibration.thru_valid)
      )
    {
        DataAdd16(0);
        DataSend();
        return;
    }

    DataAdd16(g_calibration.count);

    for(int i=0; i<g_calibration.count; i++)
    {
        CalibrationElem* e = g_calibration.elems + i;
        switch(type)
        {
        case GET_CALIBRATION_FREQ: DataAdd64(e->freq); break;
        case GET_CALIBRATION_S11_OPEN: DataAddC(e->S11_open); break;
        case GET_CALIBRATION_S11_SHORT: DataAddC(e->S11_short); break;
        case GET_CALIBRATION_S11_LOAD: DataAddC(e->S11_load); break;
        case GET_CALIBRATION_S11_THRU: DataAddC(e->S11_thru); break;
        case GET_CALIBRATION_S21_THRU: DataAddC(e->S21_thru); break;
        case GET_CALIBRATION_S21_OPEN: DataAddC(e->S21_open); break;
        }
    }

    DataSend();
}
