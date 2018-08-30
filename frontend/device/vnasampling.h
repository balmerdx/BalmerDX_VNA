#ifndef VNASAMPLING_H
#define VNASAMPLING_H

#include <QObject>

#include "vnasamplingdata.h"



class VnaSampling : public QObject
{
    Q_OBJECT
public:
    enum class Scan
    {
        RX,
        TX,
        RX_TX,
    };
public:
    explicit VnaSampling(VnaSamplingData* samplingData, QObject *parent = 0);

    VnaSamplingData* samplingData() const { return _samplingData; }

    bool samplingTx() const { return _samplingTx; }
    Scan samplingType() const { return _samplingType; }

    void setSamplingType(Scan type);

    //Частота, которую в данный момент сэмплируем
    int freqIndex() const { return _freqIndex; }

    void start();

signals:
    //Железка промерила очередной сэмпл, и данные для него готовы.
    void signalSample(const HardSamplingData& data, int freqIndex, bool isTx);

    //Железка промерила все сэмплы.
    void signalEnd();
public slots:
    void onEndSampling();
    void onReceiveHard();
protected:
    void startNextSample();
protected:
    //Основные данные
    VnaSamplingData* _samplingData;

    //Поддержка процесса сканирования.
    Scan _samplingType;
    int _freqIndex;
    bool _samplingTx;
    bool _bStopSample;
    int _countBeforeStart;
};

#endif // VNASAMPLING_H
