#ifndef CALIBRATIONSOLT_H
#define CALIBRATIONSOLT_H

#include "device/vnasamplingdata.h"

//Используем 12-Term Error Model см. Rytting_NAModels.pdf

class CalibrationSOLT
{
public:
    CalibrationSOLT();

    //Устанавливает коэффициэнты, по которым была откалиброванна железка.
    void setCoeff(const VnaDataSOLT& calibrationData);

    //Вычисляем коэффициэнт отражения для rx порта
    //учитывая щупы и коэффициэнт усиления усилителя.
    //Т.е. деталь подключается только к rx порту.
    //G==S11
    complexf calculateG(complexf Gm, double freq);
    complexf GtoZ(complexf G);

    //Предполагаем, что двухвыводная деталь подключенна между RX и TX входами.
    complexf calculateZfromS11_S21(complexf S11, complexf S21);

    //Вариант, когда четырехполюсник подключенн к Rx, Tx портам
    //и он является reciprocal и symmetrical
    //S11out, S21out - вычисленные после калибровки значения.
    //Urx==S11m, Utx=S21m - измеренные прибором значения.
    void calculateS11_S21(complexf S11m, complexf S21m, double freq,
                          complexf& S11, complexf& S21);

    void calculateFull(complexf S11m, complexf S21m,
                       complexf S12m, complexf S22m,
                       double freq,
                       complexf& S11, complexf& S21,
                       complexf& S12, complexf& S22
                       );
protected:
    struct OneData
    {
        complexf S11_open;
        complexf S11_short;
        complexf S11_load;

        complexf S11_transmission;
        complexf S21_transmission;
        complexf S21_open;
    };

    //12-Term Error Model e parameters
    struct EParams
    {
        complexf e00;
        complexf e11;
        complexf De;
        complexf e10_01;
        complexf e30;
        complexf e22;
        complexf e10_32;
    };

    //Ищем индекс соответствующий выбранной частоте.
    //Частота на этом индексе меньше иди равна частоте
    //freq (только, если это не край диапазона)
    int bisectF(double freq);

    complexf interpolate(const QVector<complexf>& arr, double freq, int idx);


    OneData interpolate(double freq);
    EParams calcEParams(double freq);

    void test();

    /*
     Функция для проверки, что все сходится.
     S11, S21 - "идеальные" рассчитанные параметры.
     S11m, S21m - измеренные параметры.
    */
    void calcS11m_S21m(EParams& params,
                       complexf S11, complexf S21,
                       complexf S12, complexf S22,
                       complexf& S11m, complexf& S21m);

    //Общий случай, когда по 4 измеренным S-параметрам
    //получаются 4 откорректированных S-параметра
    void calc4params(EParams& p,
                     complexf S11m, complexf S21m,
                     complexf S12m, complexf S22m,
                     complexf& S11, complexf& S21,
                     complexf& S12, complexf& S22);
protected:
    QVector<double> _freqs;
    QVector<complexf> _S11_open;
    QVector<complexf> _S11_short;
    QVector<complexf> _S11_load;

    QVector<complexf> _S11_transmission;
    QVector<complexf> _S21_transmission;

    QVector<complexf> _S21_open;

    complexf Zstd;
};

QString number(complexf f);

#endif // CALIBRATIONSOLT_H
