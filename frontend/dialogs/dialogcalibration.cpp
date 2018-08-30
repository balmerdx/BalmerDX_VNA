#include "dialogcalibration.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QGroupBox>
#include <QSignalMapper>
#include <QDir>
#include <QMessageBox>

#include <QDebug>

#include "device/vnasampling.h"
#include "project.h"

DialogCalibration::DialogCalibration(QWidget *parent)
    : QDialog(parent)
    , sampling(nullptr)
    , currentTypeIndex(VnaDataSOLT::LOAD)
    , changed(false)
{
    setWindowTitle("Calibration");

    calibrationName = g_project->currentCalibrationName();
    dataSolt = g_project->calibration(calibrationName);

    calibrationLayout = new QGridLayout();
    QGroupBox* groupCalibration = new QGroupBox(calibrationName);
    groupCalibration->setLayout(calibrationLayout);

    startMapper = new QSignalMapper();
    connect(startMapper, SIGNAL(mapped(int)), this, SLOT(onStartClicked(int)));

    int line = 0;
    addCalibrationLine(line++, VnaDataSOLT::OPEN, "Open");
    addCalibrationLine(line++, VnaDataSOLT::SHORT, "Short");
    addCalibrationLine(line++, VnaDataSOLT::LOAD, "Load (50 Om)");
    addCalibrationLine(line++, VnaDataSOLT::TRANSMISSION, "Transmission");

    progressBar = new QProgressBar();
    progressBar->setVisible(false);

    QPushButton* okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));

    QHBoxLayout* okLayout = new QHBoxLayout();
    okLayout->addStretch();
    okLayout->addWidget(okButton);

    QVBoxLayout* topLayout;
    topLayout = new QVBoxLayout();

    topLayout->addWidget(groupCalibration);
    topLayout->addWidget(progressBar);
    topLayout->addStretch();
    topLayout->addLayout(okLayout);


    this->setLayout(topLayout);
}

void DialogCalibration::onOk()
{
    if(changed)
    {
        save();
    }

    done(1);
}

void DialogCalibration::addCalibrationLine(int line, VnaDataSOLT::TYPE type, QString name)
{
    LineData& d = lineData[type];

    QLabel* label = new QLabel(name);
    d.state = new QLabel();
    QPushButton* startButton = new QPushButton(tr("Start"));
    connect(startButton, SIGNAL(clicked()), startMapper, SLOT(map()));
    startMapper->setMapping(startButton, type);

    calibrationLayout->addWidget(label, line, 0);
    calibrationLayout->addWidget(d.state, line, 1);
    calibrationLayout->addWidget(startButton, line, 2);

    setDate(type);
}

void DialogCalibration::setDate(VnaDataSOLT::TYPE type)
{
    QDateTime date = dataSolt.dataTime[type];
    QLabel* label = lineData[type].state;

    if(date.isValid())
    {
        label->setText(
                "<font color=\"green\">"+
                date.toString(Qt::ISODate)+
                "</font>"
                );
    } else
    {
        label->setText(
                "<font color=\"red\">"+
                tr("Not passed")+
                "</font>");
    }

}

void DialogCalibration::onStartClicked(int typeIndex)
{
    qDebug() << "On start " << typeIndex;
    delete sampling;

    currentTypeIndex = (VnaDataSOLT::TYPE)typeIndex;
    samplingData.arrFreq = makeClaibrationFrequencies();

    sampling = new VnaSampling(&samplingData);
    connect(sampling, SIGNAL(signalSample(const HardSamplingData&, int, bool)), this, SLOT(onSample(const HardSamplingData&, int, bool)));
    connect(sampling, SIGNAL(signalEnd()), this, SLOT(onSamplingEnd()));

    sampling->setSamplingType(VnaSampling::Scan::RX_TX);

    progressBar->setMinimum(0);
    progressBar->setMaximum(samplingData.arrFreq.size()*2);
    progressBar->setValue(0);
    progressBar->setVisible(true);

    sampling->start();
}

void DialogCalibration::onSample(const HardSamplingData& data, int freqIndex, bool isTx)
{
    (void)data;
    int value = freqIndex;
    if(isTx)
        value += samplingData.arrFreq.size();

    progressBar->setValue(value);
}

void DialogCalibration::onSamplingEnd()
{
    delete sampling;
    sampling = nullptr;
    progressBar->setVisible(false);

    samplingData.convertToSparam(dataSolt.data[currentTypeIndex]);
    dataSolt.dataTime[currentTypeIndex] = QDateTime::currentDateTime();

    setDate(currentTypeIndex);

    changed = true;
}

void DialogCalibration::save()
{
    QDir dir(g_project->calibrationDirectory());

    if(!dir.isReadable())
    {
        QMessageBox::critical(this, QString(), QString("Cannot open directory: " + g_project->calibrationDirectory()));
        return;
    }

    QString filepath = dir.filePath(calibrationName+VNA_EXT);
    if(!dataSolt.save(filepath))
    {
        QMessageBox::critical(this, QString(), QString("Cannot save file: " + filepath));
        return;
    }

    g_project->setCalibration(calibrationName, dataSolt);
}
