#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QVector>
#include <QHash>

#include "device/vnasamplingdata.h"

enum class FrequencyIntervalType
{
    //Стандартный интервал частот, на которых происходит калибровка.
    Standart,

    //Точки по интервалу распределены линейно.
    // f = fmin+(fmax-fmin)*t
    Linear,

    //На низкой частоте точек больше, на высокой меньше.
    // f = exp(log(fmin)+(log(fmax)-log(fmin))*t)
    Exponential,

    //Точки по интервалу распределены линейно.
    // f = fmin+(fmax-fmin)*t
    // плюс одна дополнительная частота в конце, на которой у кварца определяется емкость.
    LinearAdd,
};

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(QObject *parent = 0);
    ~Project();

    void loadSettings();
    void saveSettings();

    double scanFrequencyMin() { return scanFrequencyMin_;}
    double scanFrequencyMax() { return scanFrequencyMax_;}
    double scanFrequencyAdditional() { return scanFrequencyAdditional_;}
    int scanFrequencyCount() { return scanFrequencyCount_; }
    FrequencyIntervalType scanFrequencyIntervalType() { return scanFrequencyIntervalType_; }

    void setScanFrequencyMin(double f);
    void setScanFrequencyMax(double f);
    void setScanFrequencyAdditional(double f);
    void setScanFrequencyCount(int count);
    void setScanFrequencyIntervalType(FrequencyIntervalType type);

    QVector<double> getScanFrequencyList();

    QString calibrationDirectory() { return calibrationDirectory_; }

    const QHash<QString, VnaDataSOLT>& calibrations() const { return calibrations_; }
    void setCalibration(const QString& name, const VnaDataSOLT& value);
    VnaDataSOLT calibration(const QString& name) const;

    void setCurrentCalibration(const QString& name) { currentCalibration_ = name; }

    QString currentCalibrationName() const { return currentCalibration_; }
    VnaDataSOLT currentCalibration() const { return calibration(currentCalibration_); }
signals:
    void signalCalibrationChange(QString name);
    void signalCalibrationNew(QString name);
public slots:
protected:
    void loadCalibrations();

    void exportClaibrationFrequencies(const char* filename);
protected:
    double scanFrequencyMin_;
    double scanFrequencyMax_;
    int scanFrequencyCount_;
    double scanFrequencyAdditional_;
    FrequencyIntervalType scanFrequencyIntervalType_;

    QString calibrationDirectory_;

    QHash<QString, VnaDataSOLT> calibrations_;
    QString currentCalibration_;
};

extern Project* g_project;

extern const QString VNA_EXT;


#endif // PROJECT_H
