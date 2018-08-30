#ifndef SELECTCALIBRATION_H
#define SELECTCALIBRATION_H

#include <QWidget>

class QComboBox;

class SelectCalibration : public QWidget
{
    Q_OBJECT
public:
    explicit SelectCalibration(QWidget *parent = 0);

signals:
public slots:
    void onCurrentChanged(int);
    void onCalibrationNew(QString);
protected:
    void updateCalibration();
protected:
    QComboBox* _calibrationBox;

    bool _inSetData;
};

#endif // SELECTCALIBRATION_H
