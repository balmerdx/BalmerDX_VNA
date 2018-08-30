#ifndef LCINFOWIDGET_H
#define LCINFOWIDGET_H

#include <QWidget>
#include <QGroupBox>

#include "device/vnautils.h"

class QComboBox;
class QLabel;

class LCInfoWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit LCInfoWidget(QWidget *parent = 0);

    void setZ(complexf Z, float F);
signals:

public slots:
    void onCurrentIndexChanged(int);
protected:
    bool isSerial();

    void calculateLC(complexf Zx, float F,
                     float& L, float& C, bool& isC,
                     float& Rout);
protected:
    QComboBox* _comboSerPar;
    QLabel* _info;

    complexf Z;
    float F;
};

#endif // LCINFOWIDGET_H
