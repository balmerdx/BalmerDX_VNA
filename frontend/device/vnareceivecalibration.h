#pragma once

#include <QObject>
#include "vnacommands.h"
#include "project.h"


class VnaReceiveCalibration : public QObject
{
    Q_OBJECT
public:
    explicit VnaReceiveCalibration(QObject *parent = 0);

    void start();

    bool getCalibration(VnaDataSOLT& data);

    QString calibrationName() const { return _name; }
signals:
    void signalComplete();
public slots:
    void onCalibrationFreq();
    void onCalibrationData(uint32_t type);
    void onCalibrationName(QString name);
protected:
    QString _name;
};
