#ifndef DIALOGCALIBRATION_H
#define DIALOGCALIBRATION_H

#include <QDialog>
#include "device/vnasamplingdata.h"

class QProgressBar;
class QGridLayout;
class QLabel;
class QSignalMapper;
class VnaSampling;

class DialogCalibration : public QDialog
{
    Q_OBJECT
public:
    explicit DialogCalibration(QWidget *parent = 0);

signals:

public slots:
    void onOk();
    void onStartClicked(int typeIndex);

    void onSample(const HardSamplingData& data, int freqIndex, bool isTx);
    void onSamplingEnd();
protected:
    struct LineData
    {
        QLabel* state;
    };

    void addCalibrationLine(int line, VnaDataSOLT::TYPE type, QString name);
    void setDate(VnaDataSOLT::TYPE type);

    void save();
protected:
    QProgressBar* progressBar;
    QGridLayout* calibrationLayout;
    QSignalMapper* startMapper;

    VnaSampling* sampling;
    VnaSamplingData samplingData;
    VnaDataSOLT::TYPE currentTypeIndex;

    LineData lineData[VnaDataSOLT::COUNT];

    VnaDataSOLT dataSolt;

    bool changed;
    QString calibrationName;
};

#endif // DIALOGCALIBRATION_H
