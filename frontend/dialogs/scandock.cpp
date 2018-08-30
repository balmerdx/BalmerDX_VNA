#include "scandock.h"

#include <QVBoxLayout>

#include "dialogselectfrequency.h"
#include "selectcalibration.h"
#include "selectgraph.h"
#include "lcinfowidget.h"

ScanDock::ScanDock(QWidget *parent)
    : QDockWidget("Scan settings", parent)
{
    _selectFreq = new DialogSelectFrequency();
    _selectCalibration = new SelectCalibration();
    _lcInfo = new LCInfoWidget(this);
    _selectGraph = new SelectGraph(this, _lcInfo);

    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(_selectCalibration);
    topLayout->addWidget(_selectFreq);
    topLayout->addWidget(_selectGraph);
    topLayout->addWidget(_lcInfo);
    topLayout->addStretch();

    QWidget* topWidget = new QWidget(this);
    topWidget->setLayout(topLayout);

    this->setWidget(topWidget);
}

void ScanDock::updateProject()
{
    _selectFreq->updateProject();
}
