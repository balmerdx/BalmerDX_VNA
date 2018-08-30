#include "consolewriteflash.h"
#include "console_utils.h"
#include "device/vnacommands.h"

#include <QCoreApplication>
#include <QThread>
#include <QTimer>

ConsoleWriteFlash::ConsoleWriteFlash(QObject *parent) : QObject(parent)
{
    connect(&_flash, SIGNAL(signalError(QString)), this, SLOT(onError(QString)));
    connect(&_flash, SIGNAL(signalFlashEraseComplete()), this, SLOT(onFlashEraseComplete()));
    connect(&_flash, SIGNAL(signalWriteProgress(uint32_t)), this, SLOT(onWriteProgress(uint32_t)));
    connect(&_flash, SIGNAL(signalWriteComplete()), this, SLOT(onWriteCompleteSuccess()));
    connect(g_commands, SIGNAL(signalDeviceIdChaged()), this, SLOT(onDeviceIdChaged()), Qt::QueuedConnection);

    _waitPing = false;
}

void ConsoleWriteFlash::setFilename(QString filename)
{
    _filename = filename;
}

void ConsoleWriteFlash::onStart()
{
    qStdOut() << "Check device id" << endl;
    _waitPing = true;
    g_commands->appendCommand(new VnaCommandPing());
}

void ConsoleWriteFlash::onDeviceIdChaged()
{
    if(!_waitPing)
        return;

    if(!g_commands->isDeviceBootloader())
    {
        //error(tr("Device id is not a bootloader."));
        qStdOut() << tr("Device id is not a bootloader.") << endl;
        g_commands->appendCommand(new VnaCommandJumpToBootloader());

        QTimer::singleShot(1000, this, SLOT(onStart()));
        return;
    }

    _waitPing = false;
    qStdOut() << "Start erase flash program:" << _filename << endl;
    _flash.start(_filename);
}

void ConsoleWriteFlash::onError(QString text)
{
    qStdOut() << text << endl;
    QCoreApplication::exit(1);
}

void ConsoleWriteFlash::onFlashEraseComplete()
{
    qStdOut() << "Flash erase complete" <<endl;
}

void ConsoleWriteFlash::onWriteProgress(uint32_t offset)
{
    (void)offset;
    if((offset%2048)==0)
    {
        qStdOut() << "*";
        qStdOut().flush();
    }
}

void ConsoleWriteFlash::onWriteCompleteSuccess()
{
    qStdOut() << endl;
    qStdOut() << "Flash write complete" << endl;
    QCoreApplication::instance()->exit(0);
}
