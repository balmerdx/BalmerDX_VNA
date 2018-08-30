#ifndef VNAUTILS_H
#define VNAUTILS_H

#include <QObject>

#include <complex>
typedef std::complex<float> complexf;


class VnaUtils : public QObject
{
    Q_OBJECT
public:
    explicit VnaUtils(QObject *parent = 0);

signals:

public slots:
};

QVector<double> makeClaibrationFrequencies();

#endif // VNAUTILS_H
