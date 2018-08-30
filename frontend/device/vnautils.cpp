#include "vnautils.h"

#include <QVector>
#include <vector>


QVector<double> makeClaibrationFrequencies()
{
/*
    std::vector<uint32_t> data =
    {
        10,11,12,13,14,15,16,17,18,19,
        20,22,24,26,28,30,32,34,36,38,
        40,44,48,52,56,60,64,68,72,76,
        80,84,88,92,96
    };
*/
    std::vector<uint32_t> data =
    {
        10,11,12,13,14,15,16,17,18,19,
        20,22,24,26,28,30,32,34,36,38,
        40,42,44,46,48,50,52,54,56,58,
        60,62,64,66,68,70,72,74,76,78,
        80,82,84,86,88,90,92,94,96,98,
    };

    QVector<double> out;

    //10 КГц-100 КГц
    for(uint32_t f : data)
        out.append(f*1000);

    //100 КГц-1 МГц
    for(uint32_t f : data)
        out.append(f*10000);

    //1 МГц-10 МГц
    for(uint32_t f : data)
        out.append(f*100000);

    //10 МГц-100 Мгц
    for(uint32_t f : data)
        out.append(f*1000000);

    //100 МГц - 200 МГц
    for(int i=0; i<=20; i++)
    {
        out.append(100e6+i*5e6);
    }

    return out;
}


VnaUtils::VnaUtils(QObject *parent) : QObject(parent)
{

}
