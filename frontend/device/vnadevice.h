#ifndef VNADEVICE_H
#define VNADEVICE_H

#include <QtSerialPort/QSerialPort>

#include <QTime>
#include "7bit.h"

const int VIRTUAL_COM_PORT_DATA_SIZE = 64;

class VnaDevice : public QObject
{
    Q_OBJECT
public:
    VnaDevice();
    ~VnaDevice();

    bool open();

    void startCommand(uint8_t command);

    //addEndPacket==false нужно только для тестовых целей проверки таймаутов на девайса.
    void endCommand(bool addEndPacket=true);

    //Вызывать тольбко внутри startCommand/endCommand
    void add(const uint8_t* data, uint32_t size);
    void add8(uint8_t data);
    void add16(uint16_t data);
    void add32(uint32_t data);

    void printDebug(const QByteArray& data, QString receive_str="Receive:");
signals:
    //Пришел пакет с девайса
    void signalPacket(const QByteArray& data);
    void signalError(QSerialPort::SerialPortError error);
    void signalClose();
public slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData();

    void update();
protected:
    void closeSerialPort();
    void errorMessage(QString info);
protected:
    QSerialPort *serial;
    QByteArray sendBuffer;
    QByteArray readBuffer;

    bool commandStarted;

    Bit7EncodeContext encodeContext;
    Bit7DecodeContext decodeContext;

    //Если за время _timeoutMsec пакет не успел закончится,
    //значит надо очистить decodeContext
    int _timeoutMsec;
    QTime _timeoutTimer;
};

extern VnaDevice* gVnaDevice;

#endif // VNADEVICE_H
