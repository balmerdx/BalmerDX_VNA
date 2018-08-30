#include "vnaupdateflash.h"

#include <QFile>
#include <QFileInfo>
#include <QThread>

#include "vnacommands.h"
#include "md5.h"

VnaUpdateFlash::VnaUpdateFlash(QObject *parent) : QObject(parent)
{
    connect(g_commands, SIGNAL(signalBootEraseFlash(uint32_t, uint32_t, uint32_t, uint32_t)), this, SLOT(onBootEraseFlash(uint32_t, uint32_t, uint32_t, uint32_t)), Qt::QueuedConnection);
    connect(g_commands, SIGNAL(signalBootWriteFlash(uint32_t, uint8_t)), this, SLOT(onBootWriteFlash(uint32_t, uint8_t)), Qt::QueuedConnection);
    connect(gVnaDevice, SIGNAL(signalError(QSerialPort::SerialPortError)), this, SLOT(onVnaError(QSerialPort::SerialPortError)));
}

void VnaUpdateFlash::start(QString filename)
{
    _filename = filename;

    _completeWrite = false;
    if(!readFile())
        return;

    g_commands->appendCommand(new VnaCommandStartWriteFlash(_dataToFlash.size(), _hash));
}

void VnaUpdateFlash::error(QString text)
{
    emit signalError(text);
}

void VnaUpdateFlash::onBootEraseFlash(uint32_t error_erase, uint32_t sectors_to_erase, uint32_t size, uint32_t fit_size)
{
    (void)size;
    (void)sectors_to_erase;
    (void)fit_size;

    if(error_erase!=0)
    {
        error(tr("Flash erasing error = ") + QString::number(error_erase));
        return;
    }

    //Какаято магия. Если здесь не ждать, то приходит в ответ мусор!
    //QThread::currentThread()->sleep(1);
    QThread::currentThread()->msleep(300);

    emit signalFlashEraseComplete();

    _writeOffset = 0;
    g_commands->appendCommand(new VnaCommandWriteFlash(_writeOffset + (uint8_t*)_dataToFlash.data()));
}

void VnaUpdateFlash::onBootWriteFlash(uint32_t offset, uint8_t ok)
{
    if(ok!=0)
    {
        error(tr("Flash write error = ") + QString::number(ok));
        return;
    }

    Q_ASSERT(_writeOffset==offset);

    _writeOffset += WRITE_TO_FLASH_SIZE;
    emit signalWriteProgress(offset);

    if(_writeOffset<(uint32_t)_dataToFlash.size())
    {
        Q_ASSERT(_writeOffset+WRITE_TO_FLASH_SIZE<=(uint32_t)_dataToFlash.size());
        g_commands->appendCommand(new VnaCommandWriteFlash(_writeOffset + (uint8_t*)_dataToFlash.data()));
    } else
    {
        _completeWrite = true;
        emit signalWriteComplete();
    }
}

void VnaUpdateFlash::onVnaError(QSerialPort::SerialPortError serial_error)
{
    if(serial_error == QSerialPort::SerialPortError::TimeoutError)
    {
        if(_completeWrite)
        {
            _completeWrite = false;
            return;
        }

        error(tr("QSerialPort::SerialPortError::TimeoutError"));
    }
}

QByteArray md5(const QByteArray& data)
{
    uint8_t hash[MD5_HASH_SIZE];
    MD5_CTX ctx;
    md5_init(&ctx);
    md5_update(&ctx, (uint8_t*)data.constData(), data.size());
    md5_final(&ctx, hash);
    return QByteArray((const char*)hash, MD5_HASH_SIZE);
}

bool VnaUpdateFlash::readFile()
{
    QFile file(_filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        error(tr("Cannot open file: ") + _filename);
        return false;
    }

    _dataToFlash = file.readAll();
    if(_dataToFlash.size()==0)
    {
        error(tr("File is empty or reading error: ") + _filename);
        return false;
    }

    QString suffix = QFileInfo(_filename).completeSuffix();

    if(suffix == "bin")
    {
        int additionalBytes = (_dataToFlash.size()+WRITE_TO_FLASH_SIZE-1)/WRITE_TO_FLASH_SIZE*WRITE_TO_FLASH_SIZE-_dataToFlash.size();
        QByteArray addData(additionalBytes, 0xFF);
        _dataToFlash += addData;
        Q_ASSERT(_dataToFlash.size()%WRITE_TO_FLASH_SIZE==0);
        _hash = md5(_dataToFlash);
    } else
    if(suffix == "bin_bootable")
    {
        const uint32_t SIZE_BOOTLOADER = 32*1024;
        const uint32_t SIZE_MISC_DATA = 32*1024;
        const uint32_t SIZE_PROGRAM_INFO = 1024;
        const uint32_t SIZE_PREFIX = SIZE_BOOTLOADER + SIZE_MISC_DATA + SIZE_PROGRAM_INFO;
        const uint32_t OFFSET_PROGRAM_INFO = SIZE_BOOTLOADER + SIZE_MISC_DATA;


        if(_dataToFlash.size() <= SIZE_PREFIX)
        {
            error(tr("Bad file format: ") + _filename);
            return false;
        }

        uint32_t program_size = *(uint32_t*)(_dataToFlash.constData()+OFFSET_PROGRAM_INFO);
        QByteArray program_hash(_dataToFlash.constData()+OFFSET_PROGRAM_INFO+4, MD5_HASH_SIZE);

        _dataToFlash.remove(0, SIZE_PREFIX);
        _hash = md5(_dataToFlash);

        if(program_size != _dataToFlash.size() ||
           program_hash!=_hash ||
           _dataToFlash.size()%WRITE_TO_FLASH_SIZE!=0)
        {
            error(tr("Bad file format: ") + _filename);
            return false;
        }

        return true;
    } else
    {
        error(tr("Unknown file extension: ") + _filename);
        return false;
    }

    return true;
}
