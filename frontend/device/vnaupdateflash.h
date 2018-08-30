#ifndef VNAUPDATEFLASH_H
#define VNAUPDATEFLASH_H

#include <QObject>
#include "vnadevice.h"

class VnaUpdateFlash : public QObject
{
    Q_OBJECT
public:
    explicit VnaUpdateFlash(QObject *parent = 0);

    void start(QString filename);

    QString filename() const { return _filename; }

    int dataSize() const { return _dataToFlash.size(); }
signals:
    void signalError(QString text);
    void signalFlashEraseComplete();
    void signalWriteProgress(uint32_t offset);
    void signalWriteComplete();
public slots:
    void onBootEraseFlash(uint32_t error_erase, uint32_t sectors_to_erase, uint32_t size, uint32_t fit_size);
    void onBootWriteFlash(uint32_t offset, uint8_t ok);
    void onVnaError(QSerialPort::SerialPortError serial_error);
protected:
    void error(QString text);
    bool readFile();
protected:
    QString _filename;

    bool _completeWrite;
    QByteArray _dataToFlash;
    uint32_t _writeOffset;
    QByteArray _hash;
};

#endif // VNAUPDATEFLASH_H
