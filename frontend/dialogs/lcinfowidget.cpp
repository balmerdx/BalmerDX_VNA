#include "lcinfowidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>


LCInfoWidget::LCInfoWidget(QWidget *parent)
    : QGroupBox(tr("Equivalent Network"), parent)
    , Z(0,0)
    , F(0)
{
    _comboSerPar = new QComboBox();
    _comboSerPar->addItem("Parallel");
    _comboSerPar->addItem("Serial");
    _comboSerPar->setCurrentIndex(1);
    connect(_comboSerPar, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

    _info = new QLabel();


    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(_comboSerPar);
    topLayout->addWidget(_info);

    this->setLayout(topLayout);
}

bool LCInfoWidget::isSerial()
{
    return _comboSerPar->currentIndex()==1;
}

QString formatR(float R)
{
    float RA = fabsf(R);
    if(RA<1e-2)
        return QString::number(R*1e3, 'f', 3) + " mOhm";
    if(RA<1)
        return QString::number(R*1e3, 'f', 2) + " mOhm";
    if(RA<1e1)
        return QString::number(R, 'f', 3) + " Ohm";
    if(RA<1e3)
        return QString::number(R, 'f', 2) + " Ohm";
    if(RA<8e3)
        return QString::number(R*1e-3, 'f', 3) + " KOhm";
    if(RA<2e4)
        return QString::number(R*1e-3, 'f', 2) + " KOhm";
    if(RA<1e6)
        return QString::number(R*1e-3, 'f', 1) + " KOhm";
    if(RA<1e7)
        return QString::number(R*1e-6, 'f', 3) + " MOhm";
    return QString::number(R*1e-6, 'f', 2) + " MOhm";
}


QString formatC(float C)
{
    float CA = fabsf(C);
    if(CA<1e-12)
        return QString::number(C*1e15, 'f', 1) + " fF";
    if(CA<1e-11)
        return QString::number(C*1e12, 'f', 3) + " pF";
    if(CA<1e-9)
        return QString::number(C*1e12, 'f', 1) + " pF";
    if(CA<1e-8)
        return QString::number(C*1e9, 'f', 2) + " nF";
    if(CA<1e-6)
        return QString::number(C*1e9, 'f', 1) + " nF";
    if(CA<1e-5)
        return QString::number(C*1e6, 'f', 2) + " μF";
    if(CA<1e-3)
        return QString::number(C*1e6, 'f', 1) + " μF";
    if(CA<1e-2)
        return QString::number(C*1e3, 'f', 2) + " mF";
    if(CA<1)
        return QString::number(C*1e3, 'f', 1) + " mF";
    if(CA<1e1)
        return QString::number(C, 'f', 2) + " F";

    return QString::number(C, 'f', 1) + " F";
}

QString formatL(float L)
{
    float LA = fabsf(L);
    if(LA<1e-8)
        return QString::number(L*1e9, 'f', 2) + " nH";
    if(LA<1e-6)
        return QString::number(L*1e9, 'f', 1) + " nH";
    if(LA<1e-5)
        return QString::number(L*1e6, 'f', 2) + " μH";
    if(LA<1e-3)
        return QString::number(L*1e6, 'f', 1) + " μH";
    if(LA<1e-2)
        return QString::number(L*1e3, 'f', 2) + " mH";
    if(LA<1)
        return QString::number(L*1e3, 'f', 1) + " mH";
    if(LA<1e1)
        return QString::number(L, 'f', 2) + " H";
    return QString::number(L, 'f', 1) + " H";
}


void LCInfoWidget::setZ(complexf Z, float F)
{
    this->Z = Z;
    this->F = F;
    if(F==0)
    {
        _info->setText("");
        return;
    }

    float L, C;
    bool isC;
    float Rout;
    calculateLC(Z, F, L, C, isC, Rout);

    QString text;
    if(isSerial())
    {
        text = "ESR = " + formatR(Rout);
    } else
    {
        text = "EPR = " + formatR(Rout);
    }

    text += "<br>";

    if(isC)
        text += "C = " + formatC(C);
    else
        text += "L = " + formatL(L);

    _info->setText(text);
}

void LCInfoWidget::calculateLC(complexf Zx, float F,
                 float& L, float& C, bool& isC, float& Rout)
{
    C = 0;
    L = 0;
    isC = false;

    Rout = 0;

    float Cmax = 1e-2f;
    if(isSerial())
    {
        isC = false;
        L = Zx.imag()/(2*M_PI*F);
        Rout = Zx.real();

        if(Zx.imag()<-1e-10)
        {
            isC = true;
            C = -1/(2*M_PI*F*Zx.imag());
        } else
        {
            C = 0;
        }

        //если сопротивление маленькое и индуктивность немного отрицательная, то таки считаем что это ошибка калибрации
        if(abs(Zx)<2 && L<0 && L>-1e-9)
        {
            isC = false;
        }
    } else
    { //parallel
        isC = true;
        complexf Yx = 1.0f/Zx;
        C = Yx.imag()/(2*M_PI*F);
        C = std::min(C, Cmax);
        C = std::max(C, -Cmax);

        if(fabsf(Yx.real())>-1e-9)
        {
            Rout = 1/Yx.real();
        } else
        {
            Rout = 1e9f;
        }

        if(Yx.imag()<-1e-10)
        {
            isC = false;
            L = -1/(2*M_PI*F*Yx.imag());
        } else
        {
            L = 0;
        }

        //если сопротивление большое и емкость немного отрицательная, то таки считаем что это ошибка калибрации
        if(abs(Zx)>1e5 && C<0 && C>-5e-15)
            isC = true;
    }
}

void LCInfoWidget::onCurrentIndexChanged(int)
{
    setZ(Z, F);
}
