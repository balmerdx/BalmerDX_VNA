#include "vnadevice.h"

#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "mainwindow.h"
#include "../4code/src/usart/usart.h"
#include "console/console_utils.h"

VnaDevice* gVnaDevice = nullptr;

VnaDevice::VnaDevice()
    : commandStarted(false)
{
    gVnaDevice = this;
    serial = new QSerialPort(this);

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    _timeoutMsec = 2000;
    _timeoutTimer.start();
}

VnaDevice::~VnaDevice()
{
    gVnaDevice = nullptr;
}

bool VnaDevice::open()
{
    QString serialName;

    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
    {
        if(serialName.isEmpty())
            serialName = info.portName();
        qDebug() << "serial name=" << info.portName() << " desc=" << info.description() << " busy=" << info.isBusy();
    }

    serial->setPortName(serialName);
    //serial->setBaudRate(QSerialPort::Baud115200);
    serial->setBaudRate(USART_BAUD_RATE);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (!serial->open(QIODevice::ReadWrite))
    {
        errorMessage(serial->errorString());
        return false;
    }

    qDebug() << "Serial port connected:" << serialName;

    bit7encodeInit(&encodeContext);
    bit7decodeInit(&decodeContext);

    return true;
}

void VnaDevice::closeSerialPort()
{
    if (serial->isOpen())
    {
        serial->close();
        emit signalClose();
    }
}


void VnaDevice::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        errorMessage(serial->errorString());
        closeSerialPort();
    }

    emit signalError(error);
}

void VnaDevice::printDebug(const QByteArray& data, QString receive_str)
{
    //debug commands
    QString strDebug;
    for(uint8_t c : data)
    {
        strDebug += QString::number(c, 16);
        strDebug += " ";
    }
    qDebug() << receive_str << strDebug;
}

void VnaDevice::update()
{
    if(!serial->isOpen())
        return;
/*
    qint64 bytes = serial->bytesAvailable();
    if(bytes!=0)
    {
        qDebug() << "bytesAvailable=" << bytes;
    }
*/
    if(_timeoutTimer.elapsed() > _timeoutMsec &&
       (decodeContext.bit_count>0 || readBuffer.size()>0))
    {
        qDebug() << "VnaDevice timeout, command fail";
        readBuffer.clear();
        bit7decodeInit(&decodeContext);
        _timeoutTimer.start();
        signalError(QSerialPort::SerialPortError::TimeoutError);
    }
}


void VnaDevice::readData()
{
    QByteArray data = serial->readAll();
    //printDebug(data, "Raw:");
    _timeoutTimer.start();

    for(uint8_t c : data)
    {
        bit7decodeAddByte(&decodeContext, c);

        uint8_t outByte;
        bool complete;
        if(bit7decodeGetByte(&decodeContext, &outByte, &complete))
            readBuffer.append(outByte);


        if(complete)
        {
            if(readBuffer.size()>0)
            {
                emit signalPacket(readBuffer);
            } else
            {
                qDebug() << "empty";
            }
            readBuffer.clear();
            continue;
        }
    }

}

void VnaDevice::add(const uint8_t* data, uint32_t size)
{
    Q_ASSERT(commandStarted);
    for(uint32_t i=0; i<size; i++)
    {
        uint8_t byte;
        while(bit7encodeGetByte(&encodeContext, &byte))
            sendBuffer.append(byte);

        bit7encodeAddByte(&encodeContext, data[i]);
    }
}

void VnaDevice::add8(uint8_t data)
{
    add(&data, sizeof(data));
}

void VnaDevice::add16(uint16_t data)
{
    add((uint8_t*)&data, sizeof(data));
}

void VnaDevice::add32(uint32_t data)
{
    add((uint8_t*)&data, sizeof(data));
}


void VnaDevice::startCommand(uint8_t command)
{
    commandStarted = true;
    add8(command);
}

void VnaDevice::endCommand(bool addEndPacket)
{
    if(addEndPacket)
        bit7encodeEndPacket(&encodeContext);
    uint8_t byte;
    while(bit7encodeGetByte(&encodeContext, &byte))
        sendBuffer.append(byte);
    serial->flush();
    serial->write(sendBuffer);

    sendBuffer.clear();
    commandStarted = false;
}


void VnaDevice::errorMessage(QString info)
{
    if(mainWindow)
        QMessageBox::critical(mainWindow, tr("Error"), serial->errorString());
    else
        qStdOut() << "VnaDevice::error=" << info << "\n";
}
