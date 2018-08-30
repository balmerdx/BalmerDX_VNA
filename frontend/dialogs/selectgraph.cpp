#include "selectgraph.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>

#include "mainwindow.h"
#include "lcinfowidget.h"

SelectGraph* g_graphSettings = nullptr;

SelectGraph::SelectGraph(QWidget *parent, LCInfoWidget* lcinfo)
    : QGroupBox(tr("Graph settings"), parent)
    , _inSetData(false)
    , _isZPattern(false)
    , _lcinfo(lcinfo)
{
    _prevModel = MathModel::BAD;
    _prevGraph = GraphType::BAD;

    g_graphSettings = this;
    _comboModelType = new QComboBox();
    connect(_comboModelType, SIGNAL(currentIndexChanged(int)), this, SLOT(onMathModelChanged(int)));

    connect(mainWindow, SIGNAL(signalSParamChanged()), this, SLOT(onSParamChanged()));

    _comboGraphType = new QComboBox();
    connect(_comboGraphType, SIGNAL(currentIndexChanged(int)), this, SLOT(onGraphTypeChanged(int)));

    _legend = new QLabel();
    _legend->setTextFormat(Qt::RichText);

    _isErrorsVisible = new QCheckBox("Errors");
    _isErrorsVisible->setChecked(true);
    connect(_isErrorsVisible, SIGNAL(stateChanged(int)), this, SLOT(onErrorsVisible(int)));

    _freq = new QLabel();

    updateGraphType();

    QVBoxLayout* topLayout = new QVBoxLayout();

    topLayout->addWidget(new QLabel("Math model:"));
    topLayout->addWidget(_comboModelType);
    topLayout->addSpacing(10);
    topLayout->addWidget(new QLabel("Graph type:"));
    topLayout->addWidget(_comboGraphType);
    topLayout->addSpacing(10);
    topLayout->addWidget(_freq);
    topLayout->addWidget(_legend);
    topLayout->addWidget(_isErrorsVisible);

    setLayout(topLayout);

    _lcinfo->setVisible(false);
}

void SelectGraph::updateGraphType()
{
    _comboGraphType->clear();
    MathModel model = mathModel();
    VnaSParam& p = mainWindow->sparam();

    if(p.S11valid)
    {
        if(p.S21valid && model != MathModel::S11)
        {
            _comboGraphType->addItem("Abs(S11), Abs(S21)", QVariant((int)GraphType::ABS_S11_S21));
        } else
        {
            _comboGraphType->addItem("Abs(S11)", QVariant((int)GraphType::ABS_S11));
        }
    }

    if(p.S12valid && p.S22valid
            && model != MathModel::S11
            && model != MathModel::RECIPROCAL_SYMMETRICAL
            )
    {
        _comboGraphType->addItem("Abs(S12), Abs(S22)", QVariant((int)GraphType::ABS_S12_S22));
    }

    if(p.S11valid)
    {
        _comboGraphType->addItem("S11", QVariant((int)GraphType::S11));
    }

    if(p.S21valid && model != MathModel::S11)
    {
        _comboGraphType->addItem("S21", QVariant((int)GraphType::S21));
        if(model != MathModel::RAW)
            _comboGraphType->addItem("dB(S21)", QVariant((int)GraphType::DB_S21));
    }

    if(p.S12valid
            && model != MathModel::S11
            && model != MathModel::RECIPROCAL_SYMMETRICAL
       )
    {
        _comboGraphType->addItem("S12", QVariant((int)GraphType::S12));
        if(model != MathModel::RAW)
            _comboGraphType->addItem("dB(S12)", QVariant((int)GraphType::DB_S12));
    }

    if(p.S22valid
            && model != MathModel::RECIPROCAL_SYMMETRICAL)
    {
        _comboGraphType->addItem("S22", QVariant((int)GraphType::S22));
    }

    if(p.S11valid && model != MathModel::RAW)
    {
        _comboGraphType->addItem("Z(S11)", QVariant((int)GraphType::Z_S11));
    }

    if(p.S22valid
            && model != MathModel::RAW
            && model != MathModel::RECIPROCAL_SYMMETRICAL
       )
    {
        _comboGraphType->addItem("Z(S22)", QVariant((int)GraphType::Z_S22));
    }

    if(p.S11valid && p.S21valid && model == MathModel::RECIPROCAL_SYMMETRICAL)
    {
        _comboGraphType->addItem("Z(RX-[]-TX)", QVariant((int)GraphType::Z_RX_TX));

    }


    int idx = _comboGraphType->findData(QVariant((int)_prevGraph));
    if(idx>=0)
    {
        _comboGraphType->setCurrentIndex(idx);
    } else
    {
        if(_comboGraphType->count())
        {
            _comboGraphType->setCurrentIndex(0);
        } else
        {
            _prevGraph = GraphType::BAD;
        }
    }

    _isErrorsVisible->setVisible(model == MathModel::RAW);
}

GraphType SelectGraph::graphType()
{
    QVariant data = _comboGraphType->currentData();
    GraphType type = GraphType::BAD;
    if(data.isValid())
        type = (GraphType)data.toInt();
    return type;
}

MathModel SelectGraph::mathModel()
{
    QVariant data = _comboModelType->currentData();
    if(data.isValid())
        return (MathModel)data.toInt();

    return MathModel::BAD;

}

void SelectGraph::onGraphTypeChanged(int)
{
    if(_inSetData)
        return;

    QVariant data = _comboGraphType->currentData();
    if(data.isValid())
        _prevGraph = (GraphType)data.toInt();

    mainWindow->updateGraph();
}

void SelectGraph::onMathModelChanged(int)
{
    if(_inSetData)
        return;
    _prevModel = mathModel();
    updateGraphType();
}

void SelectGraph::onSParamChanged()
{
    if(_inSetData)
        return;

    _inSetData = true;
    VnaSParam& p = mainWindow->sparam();

    _comboModelType->clear();
    if(p.S11valid)
    {
        _comboModelType->addItem("RAW", QVariant((int)MathModel::RAW));
        _comboModelType->addItem("S11", QVariant((int)MathModel::S11));

        if(p.S21valid)
        {
            _comboModelType->addItem("reciprocal & symmetrical", QVariant((int)MathModel::RECIPROCAL_SYMMETRICAL));

            if(p.S12valid && p.S22valid)
            {
                _comboModelType->addItem("4 S-params", QVariant((int)MathModel::FULL));
            }
        }
    }

    int idx = _comboModelType->findData(QVariant((int)_prevModel));
    if(idx>=0)
        _comboModelType->setCurrentIndex(idx);
    else
    {
        if(_comboModelType->count())
        {
            _comboModelType->setCurrentIndex(0);
            _prevModel = mathModel();
        } else
        {
            _prevModel = MathModel::BAD;
        }
    }


    updateGraphType();

    _inSetData = false;
}

bool SelectGraph::isErrorsVisible()
{
    return _isErrorsVisible->checkState()==Qt::Checked;
}

void SelectGraph::onErrorsVisible(int)
{
    mainWindow->updateGraph();
}

void SelectGraph::setFreq(double freq)
{
    QString str = "Frequency: ";

    if(freq<1e6)
    {
        str += QString::number(freq/1e3);
        str += " KHz";
    } else
    {
        str += QString::number(freq/1e6);
        str += " MHz";
    }


    _freq->setText(str);
}

void SelectGraph::setValuesPattern(QStringList pattern, bool isZ)
{
    _pattern = pattern;
    _isZPattern = isZ;

    QList<double> values;
    for(QString s : _pattern)
        values.append(0);


    setValues(values, 0);

    _lcinfo->setVisible(_isZPattern);
}

void SelectGraph::setValues(QList<double> values, float F)
{
    if(_pattern.size() != values.size())
        return;
    QString text;

    for(int i=0; i<values.size(); i++)
    {
        if(i!=0)
            text += "<br>";

        text += _pattern[i].arg(values[i]);

    }

    _legend->setText(text);

    if(_isZPattern)
        _lcinfo->setZ(complexf(values[0], values[1]), F);
}
