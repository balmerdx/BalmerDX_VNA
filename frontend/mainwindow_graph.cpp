#include "mainwindow.h"
#include "project.h"

#include "dialogs/selectgraph.h"

static QColor colorOrange("#FF8000");

static QString sRed(QString s)
{
    return QString("<font color=\"#FF0000\">")+ s + "</font>";
}

static QString sGreen(QString s)
{
    return QString("<font color=\"#00FF00\">")+ s + "</font>";
}

static QString sBlue(QString s)
{
    return QString("<font color=\"#0000FF\">")+ s + "</font>";
}

static QString sOrange(QString s)
{
    return QString("<font color=\"#ff8000\">")+ s + "</font>";
}
/*
static QString sBlack(QString s)
{
    return QString("<font color=\"#000000\">")+ s + "</font>";
}
*/

void MainWindow::setErrorsInGraph(int i)
{
    _customPlot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDot, 4));
    _customPlot->graph(i)->setErrorType(QCPGraph::etValue);
    _customPlot->graph(i)->setErrorPen(QPen(QColor(160,160,160)));
}

void MainWindow::calcS4(QVector<double>& arrFreqM, QVector<complexf>& S11, QVector<complexf>& S21, QVector<complexf>& S12, QVector<complexf>& S22)
{
    int indexBegin, indexEnd;
    _sparam.rangeWithoutAdditionalFrequency(indexBegin, indexEnd);

    size_t size = indexEnd-indexBegin;
    MathModel model = g_graphSettings->mathModel();
    Q_ASSERT(model!=MathModel::RAW);
    arrFreqM.resize(size);
    S11.resize(size);
    S21.resize(size);
    S12.resize(size);
    S22.resize(size);

    for(size_t freqIndex=0; freqIndex<size; freqIndex++)
    {
        SParam& s = _sparam.sparam[freqIndex+indexBegin];

        arrFreqM[freqIndex] = s.F*1e-6;
        if(model==MathModel::RAW)
        {
            if(_sparam.S11valid)
                S11[freqIndex] = s.S11;
            if(_sparam.S21valid)
                S21[freqIndex] = s.S21;
            if(_sparam.S12valid)
                S12[freqIndex] = s.S12;
            if(_sparam.S22valid)
                S22[freqIndex] = s.S22;
        } else
        if(model==MathModel::S11)
        {
            if(_sparam.S11valid)
                S11[freqIndex] = _calib.calculateG(s.S11, s.F);
            if(_sparam.S22valid)
                S22[freqIndex] = _calib.calculateG(s.S22, s.F);

        } else
        if(model==MathModel::RECIPROCAL_SYMMETRICAL)
        {
            _calib.calculateS11_S21(s.S11, s.S21, s.F,
                                  S11[freqIndex], S21[freqIndex]);
        } else
        if(model==MathModel::FULL)
        {
            _calib.calculateFull(s.S11, s.S21, s.S12, s.S22, s.F,
                   S11[freqIndex], S21[freqIndex], S12[freqIndex], S22[freqIndex]);
        }
    }
}

bool MainWindow::makeCorrection(VnaSParam& correctedSParam)
{
    correctedSParam = VnaSParam();
    VnaSParam& sout = correctedSParam;
    if(_sparam.S11valid && _sparam.S12valid && _sparam.S21valid && _sparam.S22valid)
    {
        sout.sparam.resize(_sparam.sparam.size());
        sout.S11valid =
        sout.S21valid =
        sout.S12valid =
        sout.S22valid = true;

        for(int freqIndex=0; freqIndex < _sparam.sparam.size(); freqIndex++)
        {
            SParam& in = _sparam.sparam[freqIndex];
            SParam& out = sout.sparam[freqIndex];
            out.F = in.F;
            _calib.calculateFull(in.S11, in.S21, in.S12, in.S22, in.F,
                   out.S11, out.S21, out.S12, out.S22);
        }

        return true;
    }

    if(_sparam.S11valid && _sparam.S21valid)
    {
        sout.sparam.resize(_sparam.sparam.size());
        sout.S11valid =
        sout.S21valid = true;

        for(int freqIndex=0; freqIndex < _sparam.sparam.size(); freqIndex++)
        {
            SParam& in = _sparam.sparam[freqIndex];
            SParam& out = sout.sparam[freqIndex];
            out.F = in.F;
            _calib.calculateS11_S21(in.S11, in.S21, in.F,
                                  out.S11, out.S21);
        }

        return true;
    }

    if(_sparam.S11valid)
    {
        sout.sparam.resize(_sparam.sparam.size());
        sout.S11valid = true;

        for(int freqIndex=0; freqIndex < _sparam.sparam.size(); freqIndex++)
        {
            SParam& in = _sparam.sparam[freqIndex];
            SParam& out = sout.sparam[freqIndex];
            out.F = in.F;

            out.S11 = _calib.calculateG(in.S11, in.F);
        }

        return true;
    }

    return false;
}

void MainWindow::updateGraph()
{
    MathModel model = g_graphSettings->mathModel();
    GraphType type = g_graphSettings->graphType();

    if(model==MathModel::RAW)
    {
        updateGraphRaw();
        return;
    }

    //Данные для графиков
    QVector<double> arrFreqM;
    QVector<complexf> S11, S21, S12, S22;
    calcS4(arrFreqM, S11, S21, S12, S22);
    size_t size = arrFreqM.size();


    _customPlot->xAxis->setLabel("Freq (MHz)");
    _customPlot->yAxis->setLabel("");
    _customPlot->clearGraphs();

    if(type==GraphType::ABS_S11)
    {
        QVector<double> arrS11abs;
        arrS11abs.resize(size);

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            arrS11abs[freqIndex] = std::abs(S11[freqIndex]);
        }

        g_graphSettings->setValuesPattern(QStringList()
                  << sOrange("abs(S11)=%1"));

        _customPlot->addGraph();

        _customPlot->graph(0)->setPen(QPen(colorOrange));
        _customPlot->graph(0)->setData(arrFreqM, arrS11abs);
    }

    if(type==GraphType::ABS_S11_S21 || type==GraphType::ABS_S12_S22)
    {
        QVector<double> arrS11abs;
        QVector<double> arrS21abs;
        arrS11abs.resize(size);
        arrS21abs.resize(size);

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            if(type==GraphType::ABS_S11_S21)
            {
                arrS11abs[freqIndex] = std::abs(S11[freqIndex]);
                arrS21abs[freqIndex] = std::abs(S21[freqIndex]);
            }

            if(type==GraphType::ABS_S12_S22)
            {
                arrS11abs[freqIndex] = std::abs(S22[freqIndex]);
                arrS21abs[freqIndex] = std::abs(S12[freqIndex]);
            }
        }

        if(type==GraphType::ABS_S11_S21)
            g_graphSettings->setValuesPattern(QStringList()
                      << sOrange("abs(S11)=%1")
                      << sGreen("abs(S21)=%1"));
        if(type==GraphType::ABS_S12_S22)
            g_graphSettings->setValuesPattern(QStringList()
                      << sOrange("abs(S22)=%1")
                      << sGreen("abs(S12)=%1"));

        _customPlot->addGraph();
        _customPlot->addGraph();

        _customPlot->graph(0)->setPen(QPen(colorOrange));
        _customPlot->graph(1)->setPen(QPen(Qt::green));
        _customPlot->graph(0)->setData(arrFreqM, arrS11abs);
        _customPlot->graph(1)->setData(arrFreqM, arrS21abs);
    }

    if(type==GraphType::S11 ||
       type==GraphType::S21 ||
       type==GraphType::S12 ||
       type==GraphType::S22 ||
       type==GraphType::Z_S11 ||
       type==GraphType::Z_S22 ||
       type==GraphType::Z_RX_TX
      )
    {
        QVector<double> arrSreal;
        QVector<double> arrSimag;
        arrSreal.resize(size);
        arrSimag.resize(size);

        bool isZ = false;

        QString ss;
        if(type==GraphType::S11)
        {
            ss = "S11";
        }

        if(type==GraphType::S21)
        {
            ss = "S21";
        }

        if(type==GraphType::S12)
        {
            ss = "S12";
        }

        if(type==GraphType::S22)
        {
            ss = "S22";
        }

        if(type==GraphType::Z_S11)
        {
            ss = "Z(S11)";
            isZ = true;
        }

        if(type==GraphType::Z_S22)
        {
            ss = "Z(S22)";
            isZ = true;
        }

        if(type==GraphType::Z_RX_TX)
        {
            ss = "Z(RX-[]-TX)";
            isZ = true;
        }

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            complexf x;
            if(type==GraphType::S11)
                x = S11[freqIndex];
            if(type==GraphType::S21)
                x = S21[freqIndex];
            if(type==GraphType::S12)
                x = S12[freqIndex];
            if(type==GraphType::S22)
                x = S22[freqIndex];
            if(type==GraphType::Z_S11)
                x = _calib.GtoZ(S11[freqIndex]);
            if(type==GraphType::Z_S22)
                x = _calib.GtoZ(S22[freqIndex]);
            if(type==GraphType::Z_RX_TX)
                x = _calib.calculateZfromS11_S21(S11[freqIndex], S21[freqIndex]);

            arrSreal[freqIndex] = x.real();
            arrSimag[freqIndex] = x.imag();
        }

        g_graphSettings->setValuesPattern(QStringList()
                    << sRed(ss+" real=%1")
                    << sBlue(ss+" imag=%1"), isZ);
        _customPlot->addGraph();
        _customPlot->addGraph();

        _customPlot->graph(0)->setPen(QPen(Qt::red));
        _customPlot->graph(1)->setPen(QPen(Qt::blue));
        _customPlot->graph(0)->setData(arrFreqM, arrSreal);
        _customPlot->graph(1)->setData(arrFreqM, arrSimag);
    }

    if(type==GraphType::DB_S21 ||
       type==GraphType::DB_S12
      )
    {
        QVector<double> arrDB_S;
        arrDB_S.resize(size);

        QString ss;
        if(type==GraphType::DB_S21)
        {
            ss = "S21";
        }

        if(type==GraphType::DB_S12)
        {
            ss = "S12";
        }

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            complexf x;
            if(type==GraphType::DB_S21)
                x = S21[freqIndex];
            if(type==GraphType::DB_S12)
                x = S12[freqIndex];

            float db = 20*log10(abs(x));
            arrDB_S[freqIndex] = db;
        }

        g_graphSettings->setValuesPattern(QStringList()
                    << sBlue("dB("+ss+")=%1"));
        _customPlot->addGraph();

        _customPlot->graph(0)->setPen(QPen(Qt::blue));
        _customPlot->graph(0)->setData(arrFreqM, arrDB_S);
    }
    _customPlot->rescaleAxes();
    _customPlot->replot();
}

void MainWindow::updateGraphRaw()
{
    GraphType type = g_graphSettings->graphType();

    QVector<double> arrFreqM;
    QVector<double> arrS11error;
    QVector<double> arrS21error;
    QVector<double> arrS12error;
    QVector<double> arrS22error;

    int indexBegin, indexEnd;
    _sparam.rangeWithoutAdditionalFrequency(indexBegin, indexEnd);
    size_t size = indexEnd-indexBegin;
    QVector<complexf> S11, S21, S12, S22;
    S11.resize(size);
    S21.resize(size);
    S12.resize(size);
    S22.resize(size);

    arrFreqM.resize(size);
    arrS11error.resize(size);
    arrS21error.resize(size);
    arrS12error.resize(size);
    arrS22error.resize(size);

    for(size_t freqIndex=0; freqIndex<size; freqIndex++)
    {
        SParam& s = _sparam.sparam[freqIndex+indexBegin];
        arrFreqM[freqIndex] = s.F*1e-6;
        if(_sparam.S11valid)
        {
            S11[freqIndex] = s.S11;
            arrS11error[freqIndex] = s.S11err;
        }

        if(_sparam.S21valid)
        {
            S21[freqIndex] = s.S21;
            arrS21error[freqIndex] = s.S21err;
        }

        if(_sparam.S12valid)
        {
            S12[freqIndex] = s.S12;
            arrS12error[freqIndex] = s.S12err;
        }

        if(_sparam.S22valid)
        {
            S22[freqIndex] = s.S22;
            arrS22error[freqIndex] = s.S22err;
        }
    }

    _customPlot->xAxis->setLabel("Freq (MHz)");
    _customPlot->yAxis->setLabel("");
    _customPlot->clearGraphs();

    if(type==GraphType::ABS_S11)
    {
        QVector<double> arrS11abs;
        arrS11abs.resize(size);

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            arrS11abs[freqIndex] = std::abs(S11[freqIndex]);
        }

        g_graphSettings->setValuesPattern(QStringList()
                          << sOrange("abs(S11)=%1"));

        _customPlot->addGraph();

        if(g_graphSettings->isErrorsVisible())
        {
            setErrorsInGraph(0);
        }

        _customPlot->graph(0)->setPen(QPen(colorOrange));
        _customPlot->graph(0)->setDataValueError(arrFreqM, arrS11abs, arrS11error);
    }

    if(type==GraphType::ABS_S11_S21 ||
       type==GraphType::ABS_S12_S22
       )
    {
        QVector<double> arrS11abs;
        QVector<double> arrS21abs;
        arrS11abs.resize(size);
        arrS21abs.resize(size);

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            if(type==GraphType::ABS_S11_S21)
            {
                arrS11abs[freqIndex] = std::abs(S11[freqIndex]);
                arrS21abs[freqIndex] = std::abs(S21[freqIndex]);
            }

            if(type==GraphType::ABS_S12_S22)
            {
                arrS11abs[freqIndex] = std::abs(S22[freqIndex]);
                arrS21abs[freqIndex] = std::abs(S12[freqIndex]);
            }
        }

        if(type==GraphType::ABS_S11_S21)
            g_graphSettings->setValuesPattern(QStringList()
                              << sOrange("abs(S11)=%1")
                              << sGreen("abs(S21)=%1"));
        if(type==GraphType::ABS_S12_S22)
            g_graphSettings->setValuesPattern(QStringList()
                              << sOrange("abs(S22)=%1")
                              << sGreen("abs(S12)=%1"));

        _customPlot->addGraph();
        _customPlot->addGraph();

        if(g_graphSettings->isErrorsVisible())
        {
            setErrorsInGraph(0);
            setErrorsInGraph(1);
        }

        _customPlot->graph(0)->setPen(QPen(colorOrange));
        _customPlot->graph(1)->setPen(QPen(Qt::green));
        _customPlot->graph(0)->setDataValueError(arrFreqM, arrS11abs, arrS11error);
        _customPlot->graph(1)->setDataValueError(arrFreqM, arrS21abs, arrS21error);
    }

    if(type==GraphType::S11 ||
       type==GraphType::S21 ||
       type==GraphType::S12 ||
       type==GraphType::S22)
    {
        QVector<double> arrSreal;
        QVector<double> arrSimag;
        QVector<double> arrError;
        arrSreal.resize(size);
        arrSimag.resize(size);

        QString ss;
        if(type==GraphType::S11)
        {
            ss = "S11";
            arrError = arrS11error;
        }

        if(type==GraphType::S21)
        {
            ss = "S21";
            arrError = arrS21error;
        }

        if(type==GraphType::S12)
        {
            ss = "S12";
            arrError = arrS12error;
        }

        if(type==GraphType::S22)
        {
            ss = "S22";
            arrError = arrS22error;
        }

        for(size_t freqIndex=0; freqIndex<size; freqIndex++)
        {
            complexf x;
            if(type==GraphType::S11)
                x = S11[freqIndex];
            if(type==GraphType::S21)
                x = S21[freqIndex];
            if(type==GraphType::S12)
                x = S12[freqIndex];
            if(type==GraphType::S22)
                x = S22[freqIndex];

            arrSreal[freqIndex] = x.real();
            arrSimag[freqIndex] = x.imag();
        }

        g_graphSettings->setValuesPattern(QStringList()
                          << sRed(ss+" real=%1")
                          << sBlue(ss+" imag=%1"));
        _customPlot->addGraph();
        _customPlot->addGraph();

        if(g_graphSettings->isErrorsVisible())
        {
            setErrorsInGraph(0);
            setErrorsInGraph(1);
        }

        _customPlot->graph(0)->setPen(QPen(Qt::red));
        _customPlot->graph(1)->setPen(QPen(Qt::blue));
        _customPlot->graph(0)->setDataValueError(arrFreqM, arrSreal, arrError);
        _customPlot->graph(1)->setDataValueError(arrFreqM, arrSimag, arrError);
    }

    _customPlot->rescaleAxes();
    _customPlot->replot();
}
