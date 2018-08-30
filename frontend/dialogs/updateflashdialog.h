#ifndef UPDATEFLASHDIALOG_H
#define UPDATEFLASHDIALOG_H

#include <QDialog>
#include <QByteArray>
#include "device/vnaupdateflash.h"

class QLabel;
class QToolBar;
class QTextEdit;
class QPushButton;
class QProgressBar;

class UpdateFlashDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateFlashDialog(QWidget *parent = 0);

signals:

public slots:
    void onFlashStart();
    void onOpenFile();

    void onError(QString text);
    void onFlashEraseComplete();
    void onWriteProgress(uint32_t offset);
    void onWriteCompleteSuccess();

    void onSendBack(int size);
protected:
protected:
    QToolBar* _toolbar;
    QAction* openAct;
    QAction* flashAct;

    QLabel* _fileLabel;
    QLabel* _eraseLabel;
    QProgressBar* _writeProgress;
    QLabel* _resultLabel;

    QLabel* _helpLabel;

    QString _filename;
    VnaUpdateFlash _flash;
};

#endif // UPDATEFLASHDIALOG_H
