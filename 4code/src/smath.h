#ifndef _SMATH_H_
#define _SMATH_H_

void calcSinCosMatrix(int32_t* arr, uint32_t arrSize, float freq, float step,
	float* cconst, float* csin, float* ccos
	);

float findFreqMax(int32_t* arr, uint32_t arrSize, float step, float freqMin, float freqMax, float count);

float squareMean(int32_t* arr, uint32_t arrSize, float freq, float step,
	float cconst, float csin, float ccos);


#endif//_SMATH_H_