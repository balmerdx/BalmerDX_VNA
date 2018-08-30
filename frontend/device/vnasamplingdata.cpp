#include "vnasamplingdata.h"
#include <QFile>
#include <QTextStream>

SParam::SParam()
    : F(0)
    , S11(0,0)
    , S21(0,0)
    , S12(0,0)
    , S22(0,0)
    , S11err(0)
    , S21err(0)
    , S12err(0)
    , S22err(0)
{

}

VnaSParam::VnaSParam()
    : S11valid(false)
    , S21valid(false)
    , S12valid(false)
    , S22valid(false)
{

}

pugi::xml_node VnaSParam::save(pugi::xml_node parent_node, bool saveErrors)
{
    pugi::xml_node data_node = parent_node.append_child("data");

    for(const SParam& s : sparam)
    {
        pugi::xml_node h_node = data_node.append_child("c");
        h_node.append_attribute("F").set_value(s.F);

        if(S11valid)
        {
            h_node.append_attribute("S11re").set_value(s.S11.real());
            h_node.append_attribute("S11im").set_value(s.S11.imag());
            if(saveErrors)
                h_node.append_attribute("S11err").set_value(s.S11err);
        }

        if(S21valid)
        {
            h_node.append_attribute("S21re").set_value(s.S21.real());
            h_node.append_attribute("S21im").set_value(s.S21.imag());
            if(saveErrors)
                h_node.append_attribute("S21err").set_value(s.S21err);
        }

        if(S12valid)
        {
            h_node.append_attribute("S12re").set_value(s.S12.real());
            h_node.append_attribute("S12im").set_value(s.S12.imag());
            if(saveErrors)
                h_node.append_attribute("S12err").set_value(s.S12err);
        }

        if(S22valid)
        {
            h_node.append_attribute("S22re").set_value(s.S22.real());
            h_node.append_attribute("S22im").set_value(s.S22.imag());
            if(saveErrors)
                h_node.append_attribute("S22err").set_value(s.S22err);
        }
    }

    return data_node;
}

bool VnaSParam::load(pugi::xml_node data_node)
{
    pugi::xml_node c_first = data_node.child("c");
    S11valid = !c_first.attribute("S11re").empty();
    S21valid = !c_first.attribute("S21re").empty();
    S12valid = !c_first.attribute("S12re").empty();
    S22valid = !c_first.attribute("S22re").empty();

    for(pugi::xml_node c_node : data_node.children("c"))
    {
        SParam s;
        s.F = c_node.attribute("F").as_float();

        if(S11valid)
        {
            s.S11 = complexf(c_node.attribute("S11re").as_float(), c_node.attribute("S11im").as_float());
            s.S11err = c_node.attribute("S11err").as_float();
        }

        if(S21valid)
        {
            s.S21 = complexf(c_node.attribute("S21re").as_float(), c_node.attribute("S21im").as_float());
            s.S21err = c_node.attribute("S21err").as_float();
        }

        if(S12valid)
        {
            s.S12 = complexf(c_node.attribute("S12re").as_float(), c_node.attribute("S12im").as_float());
            s.S12err = c_node.attribute("S12err").as_float();
        }

        if(S22valid)
        {
            s.S22 = complexf(c_node.attribute("S22re").as_float(), c_node.attribute("S22im").as_float());
            s.S22err = c_node.attribute("S22err").as_float();
        }

        sparam.append(s);
    }

    return true;
}

pugi::xml_node VnaSamplingData::saveRaw(pugi::xml_node pugi_parent, double F, const HardSamplingData& data)
{
    pugi::xml_node h_node = pugi_parent.append_child("h");

    const HardSamplingData& h = data;
    h_node.append_attribute("F").set_value(F);
    h_node.append_attribute("freq").set_value(h.freq);
    h_node.append_attribute("q_cconst").set_value(h.q_cconst);
    h_node.append_attribute("q_csin").set_value(h.q_csin);
    h_node.append_attribute("q_ccos").set_value(h.q_ccos);
    h_node.append_attribute("q_sqr").set_value(h.q_sqr);
    h_node.append_attribute("i_cconst").set_value(h.i_cconst);
    h_node.append_attribute("i_csin").set_value(h.i_csin);
    h_node.append_attribute("i_ccos").set_value(h.i_ccos);
    h_node.append_attribute("i_sqr").set_value(h.i_sqr);

    return h_node;
}


void VnaSamplingData::saveRawArray(pugi::xml_node root_node)
{
    bool saveRx = !arrDataRx.empty();
    bool saveTx = !arrDataTx.empty();

    if(saveTx)
    {
        pugi::xml_node data_node = root_node.append_child("data");
        data_node.append_attribute("tx").set_value(true);
        for(int i=0; i<arrFreq.size(); i++)
        {
            saveRaw(data_node, arrFreq[i], arrDataTx[i]);
        }
    }

    if(saveRx)
    {
        pugi::xml_node data_node = root_node.append_child("data");
        data_node.append_attribute("tx").set_value(false);
        for(int i=0; i<arrFreq.size(); i++)
        {
            saveRaw(data_node, arrFreq[i], arrDataRx[i]);
        }
    }
}

void VnaSamplingData::convertToSparam(VnaSParam& param)
{
    param.S11valid = !arrDataRx.empty();
    param.S21valid = !arrDataTx.empty();
    param.S12valid = false;
    param.S22valid = false;

    param.sparam.clear();
    param.sparam.resize(arrFreq.size());

    for(int i=0; i<arrFreq.size(); i++)
    {
        SParam& s = param.sparam[i];
        s.F = arrFreq[i];

        if(param.S11valid)
        {
            const HardSamplingData& hrx = arrDataRx[i];
            s.S11 = hrx.toComplex();
            s.S11err = hrx.squareMean();
        }

        if(param.S21valid)
        {
            const HardSamplingData& hrx = arrDataTx[i];
            s.S21 = hrx.toComplex();
            s.S21err = hrx.squareMean();
        }
    }
}

void VnaSamplingData::addToSparamReverse(VnaSParam& param)
{
    Q_ASSERT(param.sparam.size() == arrFreq.size());
    param.S22valid = !arrDataRx.empty();
    param.S12valid = !arrDataTx.empty();


    for(int i=0; i<arrFreq.size(); i++)
    {
        SParam& s = param.sparam[i];
        Q_ASSERT(s.F == arrFreq[i]);

        if(param.S22valid)
        {
            const HardSamplingData& hrx = arrDataRx[i];
            s.S22 = hrx.toComplex();
            s.S22err = hrx.squareMean();
        }

        if(param.S12valid)
        {
            const HardSamplingData& hrx = arrDataTx[i];
            s.S12 = hrx.toComplex();
            s.S12err = hrx.squareMean();
        }
    }

}

QString VnaDataSOLT::name(TYPE type)
{
    switch(type)
    {
    case SHORT: return "Short";
    case OPEN: return "Open";
    case LOAD: return "Load";
    case TRANSMISSION: return "Transmission";
    default:
        Q_ASSERT(0);
    }
    return "";
}

bool VnaDataSOLT::save(const QString& filename)
{
    pugi::xml_document doc;
    pugi::xml_node root_node = doc.append_child("root");

    for(int i=0; i<COUNT; i++)
    {
        pugi::xml_node data_node = data[i].save(root_node);
        data_node.append_attribute("type").set_value(name((TYPE)i).toUtf8().constData());
        data_node.append_attribute("date").set_value(dataTime[i].toString(Qt::ISODate).toUtf8().constData());
    }

    return doc.save_file(filename.toUtf8().constData());
}

bool VnaDataSOLT::empty() const
{
    for(int i=0; i<COUNT; i++)
        if(dataTime[i].isValid())
            return false;

    return true;
}

bool VnaDataSOLT::load(const QString& filename)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.toUtf8().constData());
    if(!result)
        return false;

    pugi::xml_node root_node = doc.child("root");

    for(pugi::xml_node data_node : root_node.children("data"))
    {
        QString typeName = data_node.attribute("type").as_string();

        TYPE type = COUNT;
        for(int i=0; i<COUNT; i++)
        {
            if(typeName == name((TYPE)i))
            {
                type = (TYPE)i;
                break;
            }
        }

        if(type==COUNT)
            continue;

        QString dateString = data_node.attribute("date").as_string();

        dataTime[type] = QDateTime::fromString(dateString, Qt::ISODate);
        if(!data[type].load(data_node))
            return false;

    }

    return true;
}

void VnaSParam::rangeWithoutAdditionalFrequency(int& indexBegin, int& indexEnd)
{
    bool minFreq;
    if(!isAdditionalFrequency(&minFreq))
    {
        indexBegin = 0;
        indexEnd = sparam.size();
        return;
    }

    if(minFreq)
    {
        indexBegin = 1;
        indexEnd = sparam.size();
    } else
    {
        indexBegin = 0;
        indexEnd = sparam.size()-1;
    }
}

bool VnaSParam::isAdditionalFrequency(bool* minFreqPtr)
{
    if(sparam.size()<4)
        return false;

    if(sparam[1].F - sparam[0].F > sparam[sparam.size()-1].F - sparam[1].F)
    {
        if(minFreqPtr)
            *minFreqPtr = true;
        return true;
    }

    if(sparam[sparam.size()-1].F - sparam[sparam.size()-2].F > sparam[sparam.size()-2].F - sparam[0].F)
    {
        if(minFreqPtr)
            *minFreqPtr = false;
        return true;
    }

    return false;
}

float phase180(complexf S)
{
    return std::arg(S)*180.0/M_PI;
}

bool VnaSParam::saveTouchstone(const QString& filename)
{
    bool isS11only = filename.endsWith(".s1p");

    if(!isS11only)
    {
        if(!S21valid)
            return false;
    }

    QFile data(filename);
    if (!data.open(QFile::WriteOnly | QFile::Truncate))
        return false;

    QTextStream out(&data);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out << "# KHz S MA R 50\n";

    if(isS11only)
    {
        for(SParam& s : sparam)
        {
            out << int(s.F*1e-3) << " " << std::abs(s.S11) << " " << phase180(s.S11) << "\n";
        }
    } else
    {
        for(SParam& s : sparam)
        {
            complexf S11, S21, S12, S22;
            S11 = s.S11;
            S21 = s.S21;
            S12 = S12valid? s.S12 : s.S21;
            S22 = S22valid? s.S22 : s.S11;
            out << s.F*1e-3
                << " " << std::abs(S11) << " " << phase180(S11)
                << " " << std::abs(S21) << " " << phase180(S21)
                << " " << std::abs(S12) << " " << phase180(S12)
                << " " << std::abs(S22) << " " << phase180(S22)
                << "\n";
        }
    }


    return true;
}
