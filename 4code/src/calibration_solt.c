#include "calibration_solt.h"
#include <string.h>
#include <math.h>

float Zstd = 49.9f;

void Calibration_calcS11m_S21m(EParams* params, complexf S11, complexf S21, complexf S12, complexf S22, complexf* S11m, complexf* S21m);
void Calibration_calc4params(EParams* p,
                 complexf S11m, complexf S21m,
                 complexf S12m, complexf S22m,
                 complexf* S11, complexf* S21,
                 complexf* S12, complexf* S22);

static int RawBisectF(freq_type freq)
{
    static int last_index = 0;
    if(last_index >= raw.s_param_count)
        last_index = 0;
    if(raw.s_param[last_index].freq >= freq)
        last_index = 0;


    int ifound = 0;
    for(int i=last_index; i<raw.s_param_count; i++)
    {
        if(raw.s_param[i].freq < freq)
        {
            ifound = i;
        } else
        {
            break;
        }
    }

    last_index = ifound;
    return ifound;

}

SParam RawInterpolate(freq_type freq)
{
    //Предполагаем, что все калибровочные данные валидные!
    SParam data;
    memset(&data, 0, sizeof(data));

    int idx = RawBisectF(freq);
    if(idx+1 == raw.s_param_count)
    {
        data = raw.s_param[raw.s_param_count-1];
        data.freq = freq;
        return data;
    }

    if(idx==0 && freq<raw.s_param[0].freq)
    {
        data = raw.s_param[0];
        data.freq = freq;
        return data;
    }

    data.freq = freq;
    SParam* p0 = raw.s_param + idx;
    SParam* p1 = raw.s_param + idx + 1;

    float a = (freq - p0->freq)/(float)(p1->freq - p0->freq);
    data.S11 = p0->S11*(1-a) + p1->S11*a;
    data.S21 = p0->S21*(1-a) + p1->S21*a;
    data.S11err = p0->S11err*(1-a) + p1->S11err*a;
    data.S21err = p0->S21err*(1-a) + p1->S21err*a;
    data.ref_amplitude = p0->ref_amplitude*(1-a) + p1->ref_amplitude*a;
    return data;
}


static int CalibrationBisectF(freq_type freq)
{
    static int last_index = 0;
    if(last_index >= g_calibration.count)
        last_index = 0;
    if(g_calibration.elems[last_index].freq >= freq)
        last_index = 0;


    int ifound = 0;
    for(int i=last_index; i<g_calibration.count; i++)
    {
        if(g_calibration.elems[i].freq < freq)
        {
            ifound = i;
        } else
        {
            break;
        }
    }

    last_index = ifound;
    return ifound;
}

CalibrationElem CalibrationInterpolate(freq_type freq)
{
    //Предполагаем, что все калибровочные данные валидные!
    CalibrationElem data;
    memset(&data, 0, sizeof(data));

    int idx = CalibrationBisectF(freq);
    if(idx+1 == g_calibration.count)
    {
        data = g_calibration.elems[g_calibration.count-1];
        data.freq = freq;
        return data;
    }

    if(idx==0 && freq<g_calibration.elems[0].freq)
    {
        data = g_calibration.elems[0];
        data.freq = freq;
        return data;
    }

    CalibrationElem* p0 = g_calibration.elems + idx;
    CalibrationElem* p1 = g_calibration.elems + idx + 1;

    float a = (freq - p0->freq)/(float)(p1->freq - p0->freq);

    data.S11_open = p0->S11_open*(1-a) + p1->S11_open*a;
    data.S11_short = p0->S11_short*(1-a) + p1->S11_short*a;
    data.S11_load = p0->S11_load*(1-a) + p1->S11_load*a;
    data.S11_thru = p0->S11_thru*(1-a) + p1->S11_thru*a;
    data.S21_thru = p0->S21_thru*(1-a) + p1->S21_thru*a;
    data.S21_open = p0->S21_open*(1-a) + p1->S21_open*a;

    return data;
}

EParams calcEParams(freq_type freq)
{
    /*
     * Gm_load = S11_load
     * Gm_short = S11_short
     * Gm_open = S11_open
     *
     * Gm_tans = S11_transmission
    */
    CalibrationElem data = CalibrationInterpolate(freq);


    EParams p;
    p.e00 = data.S11_load;
    p.e11 = (data.S11_open+data.S11_short-2.0f*p.e00)/(data.S11_open-data.S11_short);
    p.De = data.S11_short-p.e00+data.S11_short*p.e11;
    p.e10_01 = p.e00*p.e11 - p.De;

    p.e22 = (data.S11_thru - p.e00)/(data.S11_thru*p.e11-p.De);
    p.e30 = data.S21_open;
    p.e10_32 = (data.S21_thru-p.e30)*(1.0f-p.e11*p.e22);

    return p;
}

complexf CalibrationCalculateG(complexf Gm, freq_type freq)
{
    EParams p = calcEParams(freq);
    complexf G = (Gm - p.e00)/(Gm*p.e11-p.De);
    return G;
}

complexf CalibrationGtoZ(complexf G)
{
    /*
    G = (Z-Z0)/(Z+Z0)
    G*Z+G*Z0 = Z-Z0
    G*Z-Z = -G*Z0-Z0
    (G-1)*Z = -(G+1)*Z0
    Z = Z0 * (1+G)/(1-G)
    */
    if( fabsf(1.0f-G)<1e-5f)
    {
        G = 0;
    }

    return Zstd * (1.0f+G)/(1.0f-G);
}

void Calibration_calculateS11_S21(complexf S11m, complexf S21m, freq_type freq, complexf* S11, complexf* S21)
{
    EParams p = calcEParams(freq);

    complexf S12, S22;
    Calibration_calc4params(&p,
                S11m, S21m,
                S21m, S11m,
                S11, S21,
                &S12, &S22);


    complexf S11m_check;
    complexf S21m_check;
    Calibration_calcS11m_S21m(&p, *S11, *S21, *S21, *S11, &S11m_check, &S21m_check);
}

void Calibration_calcS11m_S21m(EParams* params, complexf S11, complexf S21, complexf S12, complexf S22, complexf* S11m, complexf* S21m)
{
    EParams* p = params;
    complexf Ds = S11*S22 - S21*S12;
    complexf divn = (1.0f-p->e11*S11-p->e22*S22+p->e11*p->e22*Ds);
    *S11m = p->e00 + p->e10_01*(S11-p->e22*Ds)/divn;
    *S21m = p->e30 + p->e10_32*S21/divn;
}

void Calibration_calculateFull(complexf S11m, complexf S21m,
                   complexf S12m, complexf S22m,
                   float freq,
                   complexf* S11, complexf* S21,
                   complexf* S12, complexf* S22
                   )
{
    EParams p = calcEParams(freq);

    Calibration_calc4params(&p,
                S11m, S21m,
                S12m, S22m,
                S11, S21,
                S12, S22);

}

void Calibration_calc4params(EParams* p,
                 complexf S11m, complexf S21m,
                 complexf S12m, complexf S22m,
                 complexf* S11, complexf* S21,
                 complexf* S12, complexf* S22)
{
    //r - reversed
    complexf e33r = p->e00;
    complexf e11r = p->e22;
    complexf e23r_e32r = p->e10_01;
    complexf e23r_e01r = p->e10_32;
    complexf e22r = p->e11;
    complexf e03r = p->e30;

    complexf D = (1.0f + (S11m-p->e00)/p->e10_01*p->e11)*(1.0f + (S22m-e33r)/e23r_e32r*e22r)-
                 (S21m-p->e30)/p->e10_32 * (S12m-e03r)/e23r_e01r*p->e22*e11r;


    *S11 = ((S11m-p->e00)/p->e10_01*(1.0f+(S22m-e33r)/e23r_e32r*e22r) - p->e22*(S21m-p->e30)/p->e10_32*(S12m-e03r)/e23r_e01r)/D;

    *S21 = (S21m-p->e30)/p->e10_32*(1.0f+(S22m-e33r)/e23r_e32r*(e22r-p->e22))/D;

    *S22 = ((S22m-e33r)/e23r_e32r*(1.0f+(S11m-p->e00)/p->e10_01*p->e11) - e11r*(S21m-p->e30)/p->e10_32*(S12m-e03r)/e23r_e01r)/D;

    *S12 = (S12m-e03r)/e23r_e01r*(1.0f+(S11m-p->e00)/p->e10_01*(p->e11-e11r))/D;

}

complexf CalibrationSOLT_calculateZfromS11_S21(complexf S11, complexf S21)
{
    /*
    Предпологаем, что деталь - это комплексное сопротивление, подключенное между RX и TX портами.
    S11 = S22 = Z / (Z+2*Z0)
    S12 = S21 = 2*Z0 / (Z+2*Z0)

    S11*(Z+2*Z0) = Z
    S12*(Z+2*Z0) = 2*Z0

    Z = S11*2*Z0/(1-S11) = 2*Z0/(1/S11-1)
    Z = 2*Z0*(1-S12)/S12 = 2*Z0*(1/S12-1)

    Когда Z мало, лучше пользоваться S11.
    Когда Z велико, лучше пользоваться S12
    */
    complexf Z1 = 2.0f*Zstd/(1.0f/S11-1.0f);
    if(cabsf(Z1)<100.0f)
        return Z1;

    if(cabsf(S21)<1e-6)
        return Z1;

    complexf Z2 = 2.0f*Zstd*(1.0f/S21-1.0f);
    return Z2;
}
