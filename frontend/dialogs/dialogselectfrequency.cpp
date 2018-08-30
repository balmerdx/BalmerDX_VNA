#include "dialogselectfrequency.h"

#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "project.h"

DialogSelectFrequency::DialogSelectFrequency(QWidget *parent)
    : QWidget(parent)
{
    comboFreqType = new QComboBox();
    comboFreqType->addItem("Standart series", (int)FrequencyIntervalType::Standart);
    comboFreqType->addItem("Linear", (int)FrequencyIntervalType::Linear);
    comboFreqType->addItem("Exponential", (int)FrequencyIntervalType::Exponential);
    comboFreqType->addItem("Linear Add", (int)FrequencyIntervalType::LinearAdd);

    for(int i=0; i<comboFreqType->count(); i++)
    {
        if(comboFreqType->itemData(i).toInt()==(int)g_project->scanFrequencyIntervalType())
        {
            comboFreqType->setCurrentIndex(i);
            break;
        }
    }

    double freqMin = 10;
    double freqMax = 200e3;

    spinFrequencyMin = new QDoubleSpinBox();
    spinFrequencyMin->setRange(freqMin, freqMax);
    spinFrequencyMin->setValue(g_project->scanFrequencyMin()*1e-3);

    spinFrequencyMax = new QDoubleSpinBox();
    spinFrequencyMax->setRange(freqMin, freqMax);
    spinFrequencyMax->setValue(g_project->scanFrequencyMax()*1e-3);

    spinSamplesCount = new QSpinBox();
    spinSamplesCount->setRange(2, 1000);
    spinSamplesCount->setValue(g_project->scanFrequencyCount());

    spinFrequencyAdditional = new QDoubleSpinBox();//only for linear
    spinFrequencyAdditional->setRange(0, freqMax);
    spinFrequencyAdditional->setValue(g_project->scanFrequencyAdditional()*1e-3);

    topLayout = new QFormLayout();
    topLayout->addRow("Frequency type", comboFreqType);
    topLayout->addRow("Frequency min (KHz)", spinFrequencyMin);
    topLayout->addRow("Frequency max (KHz)", spinFrequencyMax);
    topLayout->addRow("Samples count", spinSamplesCount);
    topLayout->addRow("Additional frequency", spinFrequencyAdditional);

    setLayout(topLayout);

    connect(comboFreqType, SIGNAL(currentIndexChanged(int)), this, SLOT(onFreqTypeChanged(int)));
    onFreqTypeChanged(0);
}

void DialogSelectFrequency::updateProject()
{
    g_project->setScanFrequencyMin(spinFrequencyMin->value()*1e3);
    g_project->setScanFrequencyMax(spinFrequencyMax->value()*1e3);
    g_project->setScanFrequencyCount(spinSamplesCount->value());
    g_project->setScanFrequencyIntervalType((FrequencyIntervalType)comboFreqType->currentData().toInt());

    double fa = spinFrequencyAdditional->value();
    if(fa>=spinFrequencyMin->value() && fa<=spinFrequencyMax->value())
        fa=0;

    g_project->setScanFrequencyAdditional(fa*1e3);

}


DialogSelectFrequency::~DialogSelectFrequency()
{
    updateProject();
}

void DialogSelectFrequency::onFreqTypeChanged(int)
{
    int freqType = comboFreqType->currentData().toInt();

    bool st = (freqType == (int)FrequencyIntervalType::Standart);

    spinFrequencyMin->setVisible(!st);
    topLayout->labelForField(spinFrequencyMin)->setVisible(!st);
    spinFrequencyMax->setVisible(!st);
    topLayout->labelForField(spinFrequencyMax)->setVisible(!st);
    spinSamplesCount->setVisible(!st);
    topLayout->labelForField(spinSamplesCount)->setVisible(!st);

    bool additional = freqType == (int)FrequencyIntervalType::LinearAdd;
    spinFrequencyAdditional->setVisible(additional);
    topLayout->labelForField(spinFrequencyAdditional)->setVisible(additional);
}
