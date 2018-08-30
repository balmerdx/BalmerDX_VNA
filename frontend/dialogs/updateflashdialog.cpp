#include "updateflashdialog.h"

#include <QAction>
#include <QToolBar>
#include <QFormLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>

#include "device/vnacommands.h"
#include "md5.h"

UpdateFlashDialog::UpdateFlashDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Update Flash Utility");

    connect(&_flash, SIGNAL(signalError(QString)), this, SLOT(onError(QString)));
    connect(&_flash, SIGNAL(signalFlashEraseComplete()), this, SLOT(onFlashEraseComplete()));
    connect(&_flash, SIGNAL(signalWriteProgress(uint32_t)), this, SLOT(onWriteProgress(uint32_t)));
    connect(&_flash, SIGNAL(signalWriteComplete()), this, SLOT(onWriteCompleteSuccess()));
    connect(g_commands, SIGNAL(signalSendBack(int)), this, SLOT(onSendBack(int)));

    QSettings settings;
    _filename = settings.value("firmwareFilename").toString();

    _fileLabel = new QLabel();
    _fileLabel->setText(_filename);

    _eraseLabel = new QLabel();
    _eraseLabel->setFixedWidth(200);
    _writeProgress = new QProgressBar();
    _resultLabel = new QLabel();

    _writeProgress->setVisible(false);

    _helpLabel = new QLabel(tr("TIP: Open file BalmerDX_VNA.bin_bootable or 4code/output/program_rboot.bin"));
    _helpLabel->setStyleSheet("QLabel { color : blue; }");

    QFormLayout* fLayout = new QFormLayout();
    fLayout->setMargin(0);
    fLayout->addRow(tr("File:"), _fileLabel);
    fLayout->addRow(tr("Erase:"), _eraseLabel);
    fLayout->addRow(tr("Write:"), _writeProgress);
    fLayout->addRow(tr("Result:"), _resultLabel);

    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addLayout(fLayout);
    topLayout->addWidget(_helpLabel);

    setLayout(topLayout);

    openAct = new QAction(QIcon(":/icons/fileopen.png"), tr("Open firmware"), this );
    connect(openAct, SIGNAL(triggered()), this, SLOT(onOpenFile()));

    flashAct = new QAction(QIcon(":/icons/file_send.png"), tr("Flash firmware"), this );
    connect(flashAct, SIGNAL(triggered()), this, SLOT(onFlashStart()));

    _toolbar = new QToolBar();
    _toolbar->addAction(openAct);
    _toolbar->addAction(flashAct);

    layout()->setMenuBar(_toolbar);
}

void UpdateFlashDialog::onError(QString text)
{
    _resultLabel->setText(text);
    openAct->setEnabled(true);
    flashAct->setEnabled(true);
}


void UpdateFlashDialog::onFlashStart()
{

    if(0)
    {
        std::vector<uint8_t> data { 3};
        //std::vector<uint8_t> data;
        g_commands->appendCommand(new VnaCommandSendBack(data));
        return;
    }

    openAct->setEnabled(false);
    flashAct->setEnabled(false);
    _writeProgress->setVisible(false);
    _resultLabel->setText(QString());

    _flash.start(_filename);

    if(_resultLabel->text().isEmpty())
        _eraseLabel->setText(tr("Start erasing flash..."));
}

void UpdateFlashDialog::onFlashEraseComplete()
{
    _eraseLabel->setText(tr("Flash erase complete."));

    _writeProgress->setVisible(true);
    _writeProgress->setFormat("%v / %m");
    _writeProgress->setMinimum(0);
    _writeProgress->setMaximum(_flash.dataSize());
    _writeProgress->setValue(0);
}

void UpdateFlashDialog::onWriteProgress(uint32_t offset)
{
    _writeProgress->setValue(offset);
}

void UpdateFlashDialog::onOpenFile()
{
    QSettings settings;
    QString prevFile = settings.value("firmwareFilename").toString();
    QFileInfo prevFileInfo(prevFile);

    QString openFile = QFileDialog::getOpenFileName(this, QString(), prevFileInfo.absolutePath());

    if(openFile.isEmpty())
        return;

    _filename = openFile;
    _fileLabel->setText(_filename);
    _resultLabel->setText(QString());
}

void UpdateFlashDialog::onWriteCompleteSuccess()
{
    _resultLabel->setText(tr("Flash write complete"));
    openAct->setEnabled(true);
    flashAct->setEnabled(true);

    QSettings settings;
    settings.setValue("firmwareFilename", _filename);
}

void UpdateFlashDialog::onSendBack(int size)
{
    qDebug() << "UpdateFlashDialog::onSendBack=" << size;
}
