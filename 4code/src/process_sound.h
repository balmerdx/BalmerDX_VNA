#define FFT_LENGTH 1024
//#define FFT_LENGTH 256


void OnSoundData(int32_t sampleQ, int32_t sampleI);
void SoundQuant();

void SamplingStart();
bool SamplingCompleted();

/*
void InitFft();
//Куда в текущий момент пишем в буфере
uint16_t DacGetWritePos();

//Расстояние между позийией записи и чтения.
uint16_t DacGetDeltaPos();

extern uint16_t fft_to_display[FFT_LENGTH];
*/

#define SAMPLE_BUFFER_SIZE 1024
#define SAMPLE_FREQUENCY 48000
extern int32_t g_samplesQ[SAMPLE_BUFFER_SIZE];
extern int32_t g_samplesI[SAMPLE_BUFFER_SIZE];

typedef void (*SoundCallback)(int32_t sampleQ, int32_t sampleI);

void SetOnSouncCallback(SoundCallback callback);
