#include "project.h"
#include "device/vnautils.h"

#include <QSettings>
#include <QDir>

Project* g_project = nullptr;
const QString VNA_EXT = ".bvna";

Project::Project(QObject *parent)
    : QObject(parent)
{
    g_project = this;

    scanFrequencyMin_ = 10e3;
    scanFrequencyMax_ = 100e6;
    scanFrequencyCount_ = 100;
    scanFrequencyIntervalType_ = FrequencyIntervalType::Standart;
    scanFrequencyAdditional_ = 0;
    calibrationDirectory_ = "/home/balmer/radio/stm32/projects/BalmerVNA/script/cal";

    //exportClaibrationFrequencies("/home/balmer/radio/stm32/projects/BalmerVNA/script/freq.txt");

    //Не загружаем калибровки, т.к. пользуемся встроенной
    //loadCalibrations();
}

Project::~Project()
{
}

void Project::loadSettings()
{
    QSettings settings;

    settings.beginGroup("ScanFrequency");
    scanFrequencyMin_ = settings.value("min", scanFrequencyMin_).toDouble();
    scanFrequencyMax_ = settings.value("max", scanFrequencyMax_).toDouble();
    scanFrequencyAdditional_ = settings.value("additional", scanFrequencyAdditional_).toDouble();
    scanFrequencyCount_ = settings.value("count", scanFrequencyCount_).toInt();
    scanFrequencyIntervalType_ = (FrequencyIntervalType)settings.value("type", (int)scanFrequencyIntervalType_).toInt();
    settings.endGroup();

    currentCalibration_ = settings.value("currentCalibration").toString();
    if(calibrations_.find(currentCalibration_)==calibrations_.end() &&
       !calibrations_.empty())
    {
        currentCalibration_ = calibrations_.begin().key();
    }

}

void Project::saveSettings()
{
    QSettings settings;

    settings.beginGroup("ScanFrequency");
    settings.setValue("min", scanFrequencyMin_);
    settings.setValue("max", scanFrequencyMax_);
    settings.setValue("count", scanFrequencyCount_);
    settings.setValue("type", (int)scanFrequencyIntervalType_);
    settings.setValue("additional", scanFrequencyAdditional_);
    settings.endGroup();

    settings.setValue("currentCalibration", currentCalibrationName());
}


void Project::setScanFrequencyMin(double f)
{
    scanFrequencyMin_ = f;
}

void Project::setScanFrequencyMax(double f)
{
    scanFrequencyMax_ = f;
}

void Project::setScanFrequencyAdditional(double f)
{
    scanFrequencyAdditional_ = f;
}

void Project::setScanFrequencyCount(int count)
{
    count = qMax(count, 1);
    count = qMin(count, 10000);
    scanFrequencyCount_ = count;
}

void Project::setScanFrequencyIntervalType(FrequencyIntervalType type)
{
    scanFrequencyIntervalType_ = type;
}

QVector<double> Project::getScanFrequencyList()
{
    if(scanFrequencyIntervalType_==FrequencyIntervalType::Standart)
        return makeClaibrationFrequencies();

    QVector<double> out;
    out.resize(scanFrequencyCount_);
    if(scanFrequencyCount_==1)
    {
        out[0] = scanFrequencyMin_;
        return out;
    }

    if(scanFrequencyIntervalType_==FrequencyIntervalType::Linear ||
       scanFrequencyIntervalType_==FrequencyIntervalType::LinearAdd)
    {

        for(int i=0; i<scanFrequencyCount_; i++)
        {
            out[i] = scanFrequencyMin_ +(scanFrequencyMax_-scanFrequencyMin_)*i/(scanFrequencyCount_-1);
        }

        if(scanFrequencyIntervalType_==FrequencyIntervalType::LinearAdd)
        {
            if(scanFrequencyAdditional_<scanFrequencyMin_)
                out.push_front(scanFrequencyAdditional_);
            if(scanFrequencyAdditional_>scanFrequencyMax_)
                out.push_back(scanFrequencyAdditional_);
        }

        return out;
    }

    //log
    double log_max = log(scanFrequencyMax_);
    double log_min = log(scanFrequencyMin_);

    for(int i=0; i<scanFrequencyCount_; i++)
    {
        out[i] = exp(log_min +(log_max-log_min)*i/(scanFrequencyCount_-1));
    }

    return out;
}

VnaDataSOLT Project::calibration(const QString& name) const
{
    auto it = calibrations_.find(name);
    if(it==calibrations_.end())
        return VnaDataSOLT();

    return it.value();
}

void Project::setCalibration(const QString &name, const VnaDataSOLT& value)
{
    bool isNew = calibrations_.find(name)!=calibrations_.end();

    calibrations_[name] = value;

    if(!isNew)
        emit signalCalibrationChange(name);
}

void Project::loadCalibrations()
{
    QDir dir(calibrationDirectory_);

    QStringList filenames = dir.entryList(QStringList() << "*"+VNA_EXT, QDir::Files, QDir::Name);
    for(QString filename : filenames)
    {
        VnaDataSOLT data;
        if(!data.load(dir.absoluteFilePath(filename)))
            continue;
        QString name = filename.left(filename.size() - VNA_EXT.size());
        currentCalibration_ = name;
        calibrations_[name] = data;
    }

}

void Project::exportClaibrationFrequencies(const char* filename)
{
    QVector<double> freq = makeClaibrationFrequencies();
    FILE* f = fopen(filename, "wt");
    if(f==NULL)
        return;

    fprintf(f, "uint32_t freq[%li]=[\n", lround(freq.size()));

    for(int i=0; i<freq.size(); i++)
    {
        fprintf(f, "%li, ", lround(freq[i]));
        if(i%10==9)
            fprintf(f, "\n");
    }

    fprintf(f, "];\n");

    fclose(f);

}
