#include "vnareceivecalibration.h"

#include <QDebug>

VnaReceiveCalibration::VnaReceiveCalibration(QObject *parent)
    : QObject(parent)
{

    connect(g_commands, SIGNAL(signalCalibrationFreq()), this, SLOT(onCalibrationFreq()), Qt::QueuedConnection);
    connect(g_commands, SIGNAL(signalCalibrationData(uint32_t)), this, SLOT(onCalibrationData(uint32_t)), Qt::QueuedConnection);
    connect(g_commands, SIGNAL(signalCalibrationName(QString)), this, SLOT(onCalibrationName(QString)), Qt::QueuedConnection);
}

void VnaReceiveCalibration::start()
{
    g_commands->appendCommand(new VnaCommandGetCalibrationName());
}

void VnaReceiveCalibration::onCalibrationFreq()
{
    if(0)
    {
        const std::vector<uint64_t>& freq = g_commands->calibrationFreq();
        QString str = "[";
        for(size_t i=0; i<freq.size(); i++)
        {
            str += QString::number(freq[i]);
            if(i+1!=freq.size())
            {
                str += ", ";
            }
        }

        str += "]";

        qDebug() << "freq = " << str;
    }

    g_commands->appendCommand(new VnaCommandGetCalibration(GET_CALIBRATION_S11_OPEN));
}

void VnaReceiveCalibration::onCalibrationData(uint32_t type)
{
    if(type==GET_CALIBRATION_S21_OPEN)
    {
        emit signalComplete();
        return;
    }

    g_commands->appendCommand(new VnaCommandGetCalibration((GET_CALIBRATION_ENUM)(type+1)));
}

void VnaReceiveCalibration::onCalibrationName(QString name)
{
    _name = name;
    qDebug() << "Calibration name received = " << name;
    g_commands->appendCommand(new VnaCommandGetCalibration(GET_CALIBRATION_FREQ));
}

bool VnaReceiveCalibration::getCalibration(VnaDataSOLT& data)
{
    const std::vector<uint64_t>& freq = g_commands->calibrationFreq();
    const std::vector<std::vector<complexf> >& calibrationS = g_commands->calibrationS();

    size_t count = freq.size();
    if(count==0)
    {
        qDebug() << "VnaReceiveCalibration freq empty";
        return false;
    }

    for(int i=GET_CALIBRATION_S11_OPEN; i<=GET_CALIBRATION_S21_OPEN; i++)
    {
        if(calibrationS[i].size()!=count)
        {
            qDebug() << "VnaReceiveCalibration calibration="<< i <<" size=" <<calibrationS[i].size() << "  not equal to freq count="<<count;
            return false;
        }
    }

    {
        VnaSParam& sp = data.data[VnaDataSOLT::SHORT];
        sp.S11valid = true;
        for(size_t i=0; i<count; i++)
        {
            SParam s;
            s.F = freq[i];
            s.S11 = calibrationS[GET_CALIBRATION_S11_SHORT][i];
            sp.sparam.append(s);
        }
    }

    {
        VnaSParam& sp = data.data[VnaDataSOLT::OPEN];
        sp.S11valid = true;
        sp.S21valid = true;
        for(size_t i=0; i<count; i++)
        {
            SParam s;
            s.F = freq[i];
            s.S11 = calibrationS[GET_CALIBRATION_S11_OPEN][i];
            s.S21 = calibrationS[GET_CALIBRATION_S21_OPEN][i];
            sp.sparam.append(s);
        }
    }

    {
        VnaSParam& sp = data.data[VnaDataSOLT::LOAD];
        sp.S11valid = true;
        for(size_t i=0; i<count; i++)
        {
            SParam s;
            s.F = freq[i];
            s.S11 = calibrationS[GET_CALIBRATION_S11_LOAD][i];
            sp.sparam.append(s);
        }
    }

    {
        VnaSParam& sp = data.data[VnaDataSOLT::TRANSMISSION];
        sp.S11valid = true;
        sp.S21valid = true;
        for(size_t i=0; i<count; i++)
        {
            SParam s;
            s.F = freq[i];
            s.S11 = calibrationS[GET_CALIBRATION_S11_THRU][i];
            s.S21 = calibrationS[GET_CALIBRATION_S21_THRU][i];
            sp.sparam.append(s);
        }
    }

    return true;
}
