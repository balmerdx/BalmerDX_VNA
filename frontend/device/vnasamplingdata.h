#ifndef VNASAMPLINGDATA_H
#define VNASAMPLINGDATA_H

#include <QDateTime>
#include "pugixml/pugixml.hpp"
#include "vnacommands.h"

struct VnaSParam;

struct VnaSamplingData
{
    QVector<double> arrFreq;
    QVector<HardSamplingData> arrDataRx;
    QVector<HardSamplingData> arrDataTx;

    //F - частота, на которой происходило измерение.
    pugi::xml_node saveRaw(pugi::xml_node pugi_parent, double F, const HardSamplingData& data);
    void saveRawArray(pugi::xml_node root_node);

    //Конвертирует RAW данные в S11, S21 параметры.
    //Очищает VnaSParam
    void convertToSparam(VnaSParam& param);

    void addToSparamReverse(VnaSParam& param);
};

struct SParam
{
    double F;
    // S11, S21 - forward measure parameters
    // S12, S22 - reverse measure parameters

    complexf S11;
    complexf S21;

    complexf S12;
    complexf S22;

    float S11err;
    float S21err;

    float S12err;
    float S22err;

    SParam();
};

struct VnaSParam
{
    QVector<SParam> sparam;
    bool S11valid;
    bool S21valid;
    bool S12valid;
    bool S22valid;

    VnaSParam();
    pugi::xml_node save(pugi::xml_node parent_node, bool saveErrors=true);
    bool load(pugi::xml_node data_node);

    //В начале или в конце sparam может быть частота,
    //сильно отстоящая от других.
    //Как правило ее не нужно показывать на графиках.
    void rangeWithoutAdditionalFrequency(int& indexBegin, int& indexEnd);
    bool isAdditionalFrequency(bool* minFreqPtr=nullptr);

    bool saveTouchstone(const QString& filename);
};

struct VnaDataSOLT
{
    enum TYPE
    {
        SHORT = 0,
        OPEN,
        LOAD,//50 Om
        TRANSMISSION,
        COUNT
    };

    VnaSParam data[TYPE::COUNT];
    QDateTime dataTime[TYPE::COUNT];

    bool save(const QString& filename);
    bool load(const QString& filename);

    QString name(TYPE type);

    bool empty() const;
};

#endif // VNASAMPLINGDATA_H
