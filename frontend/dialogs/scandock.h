#ifndef SCANDOCK_H
#define SCANDOCK_H

#include <QDockWidget>

class DialogSelectFrequency;
class SelectCalibration;
class SelectGraph;
class LCInfoWidget;

class ScanDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ScanDock(QWidget *parent = 0);

    void updateProject();
signals:

public slots:
protected:
    DialogSelectFrequency* _selectFreq;
    SelectCalibration* _selectCalibration;
    SelectGraph* _selectGraph;
    LCInfoWidget* _lcInfo;
};

#endif // SCANDOCK_H
