#include "consoleserial.h"

#include <QCoreApplication>

#include "console_utils.h"

ConsoleSerial::ConsoleSerial(QObject *parent) : QObject(parent)
{
    _device = new VnaDevice();
    connect(_device, SIGNAL(signalClose()), this, SLOT(onCloseSerial()));

    _commands = new VnaCommands(_device);

    connect(_commands, SIGNAL(signalBadPacket()), this, SLOT(onBadPacket()));

}

bool ConsoleSerial::openSerialPort()
{
    if(!_device->open())
    {
        qStdOut() << "Cannot open serial port" << endl;
        return false;
    }

    return true;
}


void ConsoleSerial::onCloseSerial()
{
    qStdOut() << "Serial port closed" << endl;
    QCoreApplication::exit(1);
}


void ConsoleSerial::onBadPacket()
{
    QCoreApplication::exit(1);
}
