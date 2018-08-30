#ifndef CONSOLEWRITEFLASH_H
#define CONSOLEWRITEFLASH_H

#include <QObject>
#include "device/vnaupdateflash.h"


class ConsoleWriteFlash : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleWriteFlash(QObject *parent = 0);

    void setFilename(QString filename);
signals:

public slots:
    void onStart();
    void onError(QString text);
    void onFlashEraseComplete();
    void onWriteProgress(uint32_t offset);
    void onWriteCompleteSuccess();
    void onDeviceIdChaged();
protected:
    void error(QString text) { onError(text);}
protected:
    VnaUpdateFlash _flash;
    QString _filename;
    bool _waitPing;
};

#endif // CONSOLEWRITEFLASH_H
