#include "calibrationsolt.h"

#include <QDebug>

QString number(complexf f)
{
    return QString("(") + QString::number(f.real(), 'f', 4) + ", " + QString::number(f.imag(), 'f', 4) + ")";
}

CalibrationSOLT::CalibrationSOLT()
{
    Zstd = 49.9f;
}

void CalibrationSOLT::setCoeff(const VnaDataSOLT& calibrationData)
{
    int fsize = calibrationData.data[VnaDataSOLT::SHORT].sparam.size();

    for(int i=0; i<VnaDataSOLT::COUNT; i++)
    {
        if(calibrationData.data[i].sparam.size() != fsize)
            return;
    }


    const QVector<SParam>& arrShort = calibrationData.data[VnaDataSOLT::SHORT].sparam;
    const QVector<SParam>& arrOpen = calibrationData.data[VnaDataSOLT::OPEN].sparam;
    const QVector<SParam>& arrLoad = calibrationData.data[VnaDataSOLT::LOAD].sparam;
    const QVector<SParam>& arrTransmission = calibrationData.data[VnaDataSOLT::TRANSMISSION].sparam;

    _freqs.resize(arrShort.size());
    _S11_open.resize(arrShort.size());
    _S11_short.resize(arrShort.size());
    _S11_load.resize(arrShort.size());

    _S11_transmission.resize(arrShort.size());
    _S21_transmission.resize(arrShort.size());
    _S21_open.resize(arrShort.size());
    for(int i=0; i<_freqs.size(); i++)
    {
        _freqs[i] = arrShort[i].F;
        _S11_open[i] = arrOpen[i].S11;
        _S11_short[i] = arrShort[i].S11;
        _S11_load[i] = arrLoad[i].S11;
        _S11_transmission[i] = arrTransmission[i].S11;
        _S21_transmission[i] = arrTransmission[i].S21;
        _S21_open[i] = arrShort[i].S21;
    }

    //test();
}

int CalibrationSOLT::bisectF(double freq)
{
    auto it = std::lower_bound(_freqs.begin(), _freqs.end(), freq);

    if(it==_freqs.end())
        return _freqs.size()-1;
    int idx = it - _freqs.begin();
    if(idx>0 && _freqs[idx]>freq)
        idx--;
    return idx;
}

complexf CalibrationSOLT::interpolate(const QVector<complexf>& arr, double freq, int idx)
{
    if(idx+1 == _freqs.size())
        return arr.back();
    if(idx==0 && freq<_freqs.first())
        return arr.first();

    float f0 = _freqs[idx];
    float f1 = _freqs[idx+1];
    Q_ASSERT( f0 < f1);
    Q_ASSERT( f0<=freq && freq<=f1);
    float a = (freq-f0)/(f1-f0);
    return arr[idx]*(1-a)+arr[idx+1]*a;
}

CalibrationSOLT::OneData CalibrationSOLT::interpolate(double freq)
{
    OneData data;

    int idx = bisectF(freq);

    data.S11_open = interpolate(_S11_open, freq, idx);
    data.S11_short = interpolate(_S11_short, freq, idx);

    data.S11_load = interpolate(_S11_load, freq, idx);
    data.S11_transmission = interpolate(_S11_transmission, freq, idx);
    data.S21_transmission = interpolate(_S21_transmission, freq, idx);
    data.S21_open = interpolate(_S21_open, freq, idx);

    return data;
}

CalibrationSOLT::EParams CalibrationSOLT::calcEParams(double freq)
{
    /*
     * Gm_load = S11_load
     * Gm_short = S11_short
     * Gm_open = S11_open
     *
     * Gm_tans = S11_transmission
    */
    OneData data = interpolate(freq);


    EParams p;
    p.e00 = data.S11_load;
    p.e11 = (data.S11_open+data.S11_short-2.0f*p.e00)/(data.S11_open-data.S11_short);
    p.De = data.S11_short-p.e00+data.S11_short*p.e11;
    p.e10_01 = p.e00*p.e11 - p.De;

    p.e22 = (data.S11_transmission - p.e00)/(data.S11_transmission*p.e11-p.De);
    p.e30 = data.S21_open;
    p.e10_32 = (data.S21_transmission-p.e30)*(1.0f-p.e11*p.e22);

    return p;
}

complexf CalibrationSOLT::calculateG(complexf Gm, double freq)
{
    EParams p = calcEParams(freq);
    complexf G = (Gm - p.e00)/(Gm*p.e11-p.De);
    return G;
}

void CalibrationSOLT::test()
{
    //bisect test
    std::vector<float> freqs = {9e3, 10e3, 11e3, 11.5e3, 1e6, 22.33e6, 99e6, 100e6, 102e6};
    for(float freq : freqs)
    {
        int idx = bisectF(freq);
        Q_ASSERT(idx>=0 && idx<_freqs.size());
        if(idx>0)
        {
            Q_ASSERT(_freqs[idx] <= freq);
        }

        if(idx+1<_freqs.size())
        {
            Q_ASSERT(_freqs[idx+1] > freq);
        }

    }
}

complexf CalibrationSOLT::GtoZ(complexf G)
{
    /*
    G = (Z-Z0)/(Z+Z0)
    G*Z+G*Z0 = Z-Z0
    G*Z-Z = -G*Z0-Z0
    (G-1)*Z = -(G+1)*Z0
    Z = Z0 * (1+G)/(1-G)
    */
    if( abs(1.0f-G)<1e-5f)
    {
        G = 0;
    }

    return Zstd * (1.0f+G)/(1.0f-G);
}

void CalibrationSOLT::calculateS11_S21(complexf S11m, complexf S21m, double freq, complexf& S11, complexf& S21)
{
    EParams p = calcEParams(freq);

    complexf S12, S22;
    calc4params(p,
                S11m, S21m,
                S21m, S11m,
                S11, S21,
                S12, S22);

    //qDebug() << "S11" << number(S11) << " S21" << number(S21);
    //qDebug() << "S22" << number(S22) << " S12" << number(S12);

    complexf S11m_check;
    complexf S21m_check;
    calcS11m_S21m(p, S11, S21, S21, S11, S11m_check, S21m_check);
    qDebug() << "err S11" << std::abs(S11m_check-S11m) << "err S21" << std::abs(S21m_check-S21m);
}

void CalibrationSOLT::calcS11m_S21m(EParams& params, complexf S11, complexf S21, complexf S12, complexf S22, complexf& S11m, complexf& S21m)
{
    EParams& p = params;
    complexf Ds = S11*S22 - S21*S12;
    complexf divn = (1.0f-p.e11*S11-p.e22*S22+p.e11*p.e22*Ds);
    S11m = p.e00 + p.e10_01*(S11-p.e22*Ds)/divn;
    S21m = p.e30 + p.e10_32*S21/divn;
}

void CalibrationSOLT::calculateFull(complexf S11m, complexf S21m,
                   complexf S12m, complexf S22m,
                   double freq,
                   complexf& S11, complexf& S21,
                   complexf& S12, complexf& S22
                   )
{
    EParams p = calcEParams(freq);

    calc4params(p,
                S11m, S21m,
                S12m, S22m,
                S11, S21,
                S12, S22);

}

void CalibrationSOLT::calc4params(EParams& p,
                 complexf S11m, complexf S21m,
                 complexf S12m, complexf S22m,
                 complexf& S11, complexf& S21,
                 complexf& S12, complexf& S22)
{
    //r - reversed
    complexf e33r = p.e00;
    complexf e11r = p.e22;
    complexf e23r_e32r = p.e10_01;
    complexf e23r_e01r = p.e10_32;
    complexf e22r = p.e11;
    complexf e03r = p.e30;

    complexf D = (1.0f + (S11m-p.e00)/p.e10_01*p.e11)*(1.0f + (S22m-e33r)/e23r_e32r*e22r)-
                 (S21m-p.e30)/p.e10_32 * (S12m-e03r)/e23r_e01r*p.e22*e11r;


    S11 = ((S11m-p.e00)/p.e10_01*(1.0f+(S22m-e33r)/e23r_e32r*e22r) - p.e22*(S21m-p.e30)/p.e10_32*(S12m-e03r)/e23r_e01r)/D;

    S21 = (S21m-p.e30)/p.e10_32*(1.0f+(S22m-e33r)/e23r_e32r*(e22r-p.e22))/D;

    S22 = ((S22m-e33r)/e23r_e32r*(1.0f+(S11m-p.e00)/p.e10_01*p.e11) - e11r*(S21m-p.e30)/p.e10_32*(S12m-e03r)/e23r_e01r)/D;

    S12 = (S12m-e03r)/e23r_e01r*(1.0f+(S11m-p.e00)/p.e10_01*(p.e11-e11r))/D;

}

complexf CalibrationSOLT::calculateZfromS11_S21(complexf S11, complexf S21)
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
    if(abs(Z1)<100.0f)
        return Z1;

    if(abs(S21)<1e-6)
        return Z1;

    complexf Z2 = 2.0f*Zstd*(1.0f/S21-1.0f);
    return Z2;
}
