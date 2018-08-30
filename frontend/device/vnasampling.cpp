#include "vnasampling.h"

const int SAMPLE_BEFORE_START = 5;


VnaSampling::VnaSampling(VnaSamplingData *samplingData, QObject *parent)
    : QObject(parent)
    , _samplingData(samplingData)
    , _samplingType(Scan::RX)
    , _freqIndex(0)
    , _bStopSample(true)
    , _countBeforeStart(0)
{
    connect(g_commands, SIGNAL(signalEndSampling()), this, SLOT(onEndSampling()));
    connect(g_commands, SIGNAL(signalReceiveHard()), this, SLOT(onReceiveHard()));
}

void VnaSampling::setSamplingType(VnaSampling::Scan type)
{
    _samplingType = type;
    _samplingTx = _samplingType==Scan::TX;
}

void VnaSampling::start()
{
    //В случае SCAN_RX_TX сначала сканируем RX, потом TX
    if(_samplingType==Scan::RX || _samplingType==Scan::RX_TX)
        _samplingData->arrDataRx.resize(_samplingData->arrFreq.size());
    else
        _samplingData->arrDataRx.resize(0);

    if(_samplingType==Scan::TX || _samplingType==Scan::RX_TX)
        _samplingData->arrDataTx.resize(_samplingData->arrFreq.size());
    else
        _samplingData->arrDataTx.resize(0);

    _bStopSample = false;

    _freqIndex = 0;
    _countBeforeStart = SAMPLE_BEFORE_START;

    startNextSample();
}

void VnaSampling::startNextSample()
{
    if(_bStopSample)
        return;
    //qDebug() << "start " << freqIndex << "f=" << arrFreq[freqIndex];
    g_commands->appendCommand(new VnaCommandSetTx(_samplingTx));
    g_commands->appendCommand(new VnaCommandSetFreq(_samplingData->arrFreq[_freqIndex]));
    g_commands->appendCommand(new VnaCommandStartSamplingAndCalculate());
    g_commands->appendCommand(new VnaCommandGetCalculated(10));
}


void VnaSampling::onEndSampling()
{
    _bStopSample = true;
    emit signalEnd();
}

void VnaSampling::onReceiveHard()
{
    if(_countBeforeStart>0)
    {
        startNextSample();
        _countBeforeStart--;
        return;
    }

    const HardSamplingData& data = g_commands->hardData();
    if(_samplingTx)
        _samplingData->arrDataTx[_freqIndex] = data;
    else
        _samplingData->arrDataRx[_freqIndex] = data;

    emit signalSample(data, _freqIndex, _samplingTx);

    if(_freqIndex+1==_samplingData->arrFreq.size())
    {
        if(_samplingType==Scan::RX_TX && !_samplingTx)
        {
            _countBeforeStart = SAMPLE_BEFORE_START;
            _freqIndex = 0;
            _samplingTx = true;
        } else
        {
            _bStopSample = true;
            onEndSampling();
            return;
        }
    } else
    {
        _freqIndex = (_freqIndex+1)%_samplingData->arrFreq.size();
    }

    startNextSample();
}
