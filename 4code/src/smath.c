#include "main.h"
#include "smath.h"
#include "dac.h" //for pi definition

#include <math.h>


/*
	Решение уравнение M*x=C
	M - матрица 3x3
	Учитываем, что m00, m12, m21 доминирующие элементы матрицы
	Портит матрицу M и вектор C
*/
static void gaussSolve(float M[3][3], float C[3], float x[3])
{
	//вычитаем первую строку из второй и третьей
	float k10 = M[1][0]/M[0][0];
	//M[1][0] -= M[0][0]*k10
	M[1][0] = 0; //при вычитании гарантированно 0 получается
	M[1][1] -= M[0][1]*k10;
	M[1][2] -= M[0][2]*k10;
	C[1] -= C[0]*k10;

	float k20 = M[2][0]/M[0][0];
	//M[2][0] -= M[0][0]*k20;
	M[2][0] = 0; //при вычитании гарантированно 0 получается (аналогично в других присваиваниях нулю)
	M[2][1] -= M[0][1]*k20;
	M[2][2] -= M[0][2]*k20;
	C[2] -= C[0]*k20;

	//вычитаем вторую строку из первой и третьей
	//учитываем, что М[1][0] == 0
	float k01 = M[0][2]/M[1][2];
	M[0][1] -= M[1][1]*k01;
	//M[0][2] -= M[1][2]*k01
	M[0][2] = 0;
	C[0] -= C[1]*k01;

	float k21 = M[2][2]/M[1][2];
	M[2][1] -= M[1][1]*k21;
	//M[2][2] -= M[1][2]*k21
	M[2][2] = 0;
	C[2] -= C[1]*k21;

	//вычитаем третью строку из первой и второй
	//учитываем, что М[2][0] == 0 и M[2][2] == 0
	float k02 = M[0][1]/M[2][1];
	//M[0][1] -= M[2][1]*k02
	M[0][1] = 0;
	C[0] -= C[2]*k02;

	float k12 = M[1][1]/M[2][1];
	//M[1][1] -= M[2][1]*k12
	M[1][1] = 0;
	C[1] -= C[2]*k12;

	x[0] = C[0]/M[0][0];
	x[1] = C[2]/M[2][1];
	x[2] = C[1]/M[1][2];
}

/*
	Используя метод наименьших квадратов, ищим наилучшее приближение для 
	функции cconst+csin*sin(f)+ccos*cos(f)

	arr - массив сэмплов по частоте
	arrSize - размер массива
	freq - частота, на которой мы хотим считать
	step - шаг частоты в массиве (1/48 KHz например)
*/
void calcSinCosMatrix(int32_t* arr, uint32_t arrSize, float freq, float step,
	float* cconst, float* csin, float* ccos
	)
{
	uint32_t N = arrSize;
	float fcycle = 2*pi*freq;

	float sumy = 0;
	float sumy_fcos = 0;
	float sumy_fsin = 0;
	float sum_fsin = 0;
	float sum_fcos = 0;
	float sum_fsin_fcos = 0;
	float sum_fcos_fcos = 0;
	float sum_fsin_fsin = 0;


	for(uint32_t i=0; i<N; i++)
	{
		float t = step*i;
		float y = arr[i];
		float fsin = sinf(fcycle*t);
		float fcos = cosf(fcycle*t);

		sumy += y;
		sumy_fcos += y*fcos;
		sumy_fsin += y*fsin;
		sum_fsin += fsin;
		sum_fcos += fcos;
		sum_fsin_fcos += fsin*fcos;
		sum_fcos_fcos += fcos*fcos;
		sum_fsin_fsin += fsin*fsin;

	}

	float M[3][3];
	float C[3];
	float x[3];

	C[0] = sumy;
	C[1] = sumy_fcos;
	C[2] = sumy_fsin;

	M[0][0] = N;
	M[0][1] = sum_fsin;
	M[0][2] = sum_fcos;
	M[1][0] = sum_fcos;
	M[1][1] = sum_fsin_fcos;
	M[1][2] = sum_fcos_fcos;
	M[2][0] = sum_fsin;
	M[2][1] = sum_fsin_fsin;
	M[2][2] = sum_fsin_fcos;

	gaussSolve(M, C, x);

	*cconst = x[0];
	*csin = x[1];
	*ccos = x[2];
}

/*
Ищем такую частоту, на которой ошибка будет минимальной.
Сканируем частотный диапазон от freqMin до freqMax
count - количество шагов при сканировании.
*/
float findFreqMax(int32_t* arr, uint32_t arrSize, float step, float freqMin, float freqMax, float count)
{
	float fstep = (freqMax-freqMin)/(count-1);

	float valMax = 0;
	float fmax = freqMin;

	for(int i=0; i<count; i++)
	{
		float fcur = freqMin+fstep*i;
		float cconst, csin, ccos;
		calcSinCosMatrix(arr, arrSize, fcur, step, &cconst, &csin, &ccos);
		float val = sqrt(csin*csin+ccos*ccos);
		if(val>valMax)
		{
			fmax = fcur;
			valMax = val;
		}
	}

	return fmax;
}

/*
	Среднеквадратическое отклонение от рассчитанной функции
*/
float squareMean(int32_t* arr, uint32_t arrSize, float freq, float step,
	float cconst, float csin, float ccos)
{
	float ssum = 0;	
	float fcycle = 2*pi*freq;
	for(uint32_t i=0; i<arrSize; i++)
	{
		float tf = fcycle*step*i;
        float y = cconst+csin*sinf(tf)+ccos*cosf(tf);
		float dy = arr[i]-y;
		ssum += dy*dy;
	}

	return sqrtf(ssum/arrSize);
}

