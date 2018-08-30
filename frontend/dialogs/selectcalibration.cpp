#include "selectcalibration.h"

#include <QFormLayout>
#include <QComboBox>
#include "project.h"


SelectCalibration::SelectCalibration(QWidget *parent)
    : QWidget(parent)
    , _inSetData(false)
{

    connect(g_project, SIGNAL(signalCalibrationNew(QString)), this, SLOT(onCalibrationNew(QString)));

    _calibrationBox = new QComboBox();
    connect(_calibrationBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentChanged(int)));

    QFormLayout* topLayout;
    topLayout = new QFormLayout();
    topLayout->addRow(tr("Calibration"), _calibrationBox);

    this->setLayout(topLayout);

    updateCalibration();
}

void SelectCalibration::updateCalibration()
{
    _inSetData = true;

    _calibrationBox->clear();
    const auto& hash = g_project->calibrations();

    int idx = 0;
    for(auto it = hash.begin(); it!=hash.end(); ++it, ++idx)
    {
        _calibrationBox->addItem(it.key());

        if(it.key()==g_project->currentCalibrationName())
            _calibrationBox->setCurrentIndex(idx);
    }

    _inSetData = false;
}

void SelectCalibration::onCurrentChanged(int)
{
    if(_inSetData)
        return;
    g_project->setCurrentCalibration(_calibrationBox->currentText());
}

void SelectCalibration::onCalibrationNew(QString)
{
    updateCalibration();
}
