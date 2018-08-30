#ifndef CONSOLESERIAL_H
#define CONSOLESERIAL_H

#include <QObject>
#include "device/vnadevice.h"
#include "device/vnacommands.h"

class ConsoleSerial : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleSerial(QObject *parent = 0);

    bool openSerialPort();
signals:

public slots:
    void onCloseSerial();
    void onBadPacket();
protected:
    VnaDevice* _device;
    VnaCommands* _commands;
};

#endif // CONSOLESERIAL_H
