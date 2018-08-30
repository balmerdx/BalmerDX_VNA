#include "mainwindow.h"
#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QComboBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QSpinBox>

#include "project.h"
#include "device/vnadevice.h"
#include "device/vnautils.h"

#include "dialogs/dialogselectfrequency.h"
#include "dialogs/dialogcalibration.h"
#include "dialogs/scandock.h"
#include "dialogs/selectgraph.h"
#include "dialogs/updateflashdialog.h"

MainWindow* mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _sampling(nullptr)
    , _pix_red_orb(":/icons/red_orb.png")
    , _pix_green_orb(":/icons/green_orb.png")
{
    setWindowTitle("Balmer Vector Network Analyzer");
    mainWindow = this;

    setGeometry(300, 150, 1300, 700);

    _device = new VnaDevice();
    connect(_device, SIGNAL(signalClose()), this, SLOT(onCloseSerial()));

    _commands = new VnaCommands(_device);
    connect(g_commands, SIGNAL(signalEndSampling()), this, SLOT(onEndSamplingSin()), Qt::QueuedConnection);
    connect(g_commands, SIGNAL(signalDeviceIdChaged()), this, SLOT(onDeviceIdChaged()), Qt::QueuedConnection);
    connect(g_commands, SIGNAL(signalRelativeLevelRF(uint8_t)), this, SLOT(onDeviceRelativeLevelRF(uint8_t)), Qt::QueuedConnection);

    createCustomPlot();
    createActions();
    createToolbar();
    createMenu();

    _dock = new ScanDock(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, _dock);

    _progressBar = new QProgressBar();
    statusBar()->addPermanentWidget(_progressBar);

    _statusBisy = new QLabel();
    statusBar()->addPermanentWidget(_statusBisy);

    _statusConnect = new QLabel();
    statusBar()->addPermanentWidget(_statusConnect);


    setCentralWidget(_customPlot);

    setStatusConnected(false);
    setBisy(false);

    openSerialPort();

    //setMouseTracking(true);

    //test code
    _calib.setCoeff(g_project->currentCalibration());

    _receiveCalibration = new VnaReceiveCalibration(this);
    connect(_receiveCalibration, SIGNAL(signalComplete()), this, SLOT(onCalibrationReceived()), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete _commands;
    delete _device;
    mainWindow = nullptr;
}

void MainWindow::createActions()
{
    settingsAct = new QAction(QIcon(":/icons/settings.png"), tr("Settings"), this );
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(onSettings()));

    sendTestSinAct = new QAction(QIcon(":/icons/file_send.png"), tr("Test sin data"), this );
    connect(sendTestSinAct, SIGNAL(triggered()), this, SLOT(sendTestSinData()));

    sendBadPacketAct = new QAction(tr("Send bad packet"), this );
    connect(sendBadPacketAct, SIGNAL(triggered()), this, SLOT(sendBadPacketData()));

    startSamplingAct = new QAction(QIcon(":/icons/blue_play.png"), tr("Start sampling"), this );
    connect(startSamplingAct, SIGNAL(triggered()), this, SLOT(onStartSampling()));

    stopSamplingAct = new QAction(QIcon(":/icons/blue_stop.png"), tr("Stop sampling"), this );
    connect(stopSamplingAct, SIGNAL(triggered()), this, SLOT(onStopSampling()));

    openAct = new QAction(QIcon(":/icons/fileopen.png"), tr("Read hard data"), this );
    connect(openAct, SIGNAL(triggered()), this, SLOT(onOpenFile()));

    saveAct = new QAction(QIcon(":/icons/save.png"), tr("Save xml"), this );
    connect(saveAct, SIGNAL(triggered()), this, SLOT(onSave()));

    saveTouchstoneAct = new QAction(QIcon(":/icons/save.png"), tr("Save touchstone s1p, s2p"), this );
    connect(saveTouchstoneAct, SIGNAL(triggered()), this, SLOT(onSaveTouchstone()));

    lnXAct = new QAction(QIcon(":/icons/ln_x.png"), tr("Log scale X graph"), this );
    lnXAct->setCheckable(true);
    connect(lnXAct, SIGNAL(triggered()), this, SLOT(onLnX()));
}

void MainWindow::createToolbar()
{
    _comboRxTx = new QComboBox();
    _comboRxTx->addItem("RX", QVariant((int)Scan::RX));
    _comboRxTx->addItem("RX & TX", QVariant((int)Scan::RX_TX));
    _comboRxTx->addItem("RX & TX x2", QVariant((int)Scan::RX_TX_X2));
    _comboRxTx->setCurrentIndex(1);
    connect(_comboRxTx, SIGNAL(currentIndexChanged(int)), this, SLOT(onRxTxIndexChanged(int)));


    _levelRF = new QSpinBox();
    _levelRF->setRange(1, MAX_RELATIVE_LEVEL_RF);
    _levelRF->setValue(MAX_RELATIVE_LEVEL_RF);
    connect(_levelRF, SIGNAL(valueChanged(int)), this, SLOT(onLevelRFValueChanged(int)));


    mainToolBar = addToolBar("main");
    mainToolBar->addAction(openAct);
    mainToolBar->addAction(saveAct);
    //mainToolBar->addAction(settingsAct);
    mainToolBar->addWidget(_comboRxTx);
    mainToolBar->addAction(startSamplingAct);
    mainToolBar->addAction(stopSamplingAct);
    mainToolBar->addAction(lnXAct);
    mainToolBar->addWidget(new QLabel(tr("LevelRF:")));
    mainToolBar->addWidget(_levelRF);
}

void MainWindow::createMenu()
{
    QMenu* menuCommands = menuBar()->addMenu(tr("Commands"));

    QAction* calibrationAct = menuCommands->addAction(tr("Calibration"));
    connect(calibrationAct, SIGNAL(triggered()), this, SLOT(onCalibrationDialog()));

    QAction* newCalibrationAct = menuCommands->addAction(tr("New calibration"));
    connect(newCalibrationAct, SIGNAL(triggered()), this, SLOT(onNewCalibration()));

    menuCommands->addAction(saveTouchstoneAct);

#ifdef QT_DEBUG
    QMenu* menuDebug = menuBar()->addMenu(tr("Debug"));
    menuDebug->addAction(sendTestSinAct);
    menuDebug->addAction(sendBadPacketAct);
#endif
}

void MainWindow::enableHardwareCommands(bool enable)
{
    startSamplingAct->setEnabled(enable);
    stopSamplingAct->setEnabled(enable);
    _comboRxTx->setEnabled(enable);
}

void MainWindow::createCustomPlot()
{
    _customPlot = new QCustomPlot(this);
    _customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    _customPlot->axisRect()->setRangeZoom(Qt::Vertical);
    _customPlot->axisRect()->setRangeDrag(Qt::Vertical);

    connect(_customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onPlotMouseMove(QMouseEvent*)));

    QVector<double> x(101), y(101); // initialize with entries 0..100
    QVector<double> y2(101);
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i];  // let's plot a quadratic function
      y2[i] = sin(x[i]);  // let's plot a quadratic function
    }
    // create graph and assign data to it:
    _customPlot->addGraph();
    _customPlot->graph(0)->setPen(QPen(Qt::red));
    _customPlot->graph(0)->setData(x, y);

    _customPlot->addGraph();
    _customPlot->graph(1)->setPen(QPen(Qt::blue));
    _customPlot->graph(1)->setData(x, y2);
    // give the axes some labels:
    _customPlot->xAxis->setLabel("x");
    _customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    _customPlot->xAxis->setRange(-1, 1);
    _customPlot->yAxis->setRange(0, 1);

    //customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    //customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    _customPlot->replot();


    statusBar()->showMessage(tr("Unknown state"));
}

void MainWindow::openSerialPort()
{
    if(!_device->open())
    {
        enableHardwareCommands(false);
        setStatusConnected(false);
        statusBar()->showMessage(tr("Cannot connect to device"));
        return;
    }

    setStatusConnected(true);

    g_commands->appendCommand(new VnaCommandPing());
    enableHardwareCommands(true);
}

void MainWindow::sendTestSinData()
{
    //commands->appendCommand(new VnaCommandPing());
    setBisy(true);
    _commands->appendCommand(new VnaCommandSetTx(scanType()!=Scan::RX), false);
    _commands->commandSampling(1000000);
}

void MainWindow::sendBadPacketData()
{
    gVnaDevice->startCommand(123);
    gVnaDevice->add32(12345);
    gVnaDevice->add32(67890);
    gVnaDevice->add32(54321);
    gVnaDevice->add32(98760);
    gVnaDevice->endCommand(false);
}

void MainWindow::setStatusConnected(bool connected)
{
    _statusConnect->setPixmap(QPixmap(connected?":/icons/connect_status.png":":/icons/disconnect_status.png"));
}

void MainWindow::setBisy(bool bisy)
{
    if(bisy)
        _statusBisy->setPixmap(_pix_red_orb);
    else
        _statusBisy->setPixmap(_pix_green_orb);
}

void MainWindow::onCloseSerial()
{
    setStatusConnected(false);
    enableHardwareCommands(false);
}

void MainWindow::onEndSamplingSin()
{
    //commands->appendCommand(new VnaCommandSetFreq(10000, 511));
    _customPlot->clearGraphs();
    _customPlot->addGraph();
    _customPlot->addGraph();

    int count = _commands->samplingBufferSize();
    QVector<double> x(count), yI(count), yQ(count);
    double mul = 1.0/0x7FFFFFFF;
    for(int i=0; i<count; i++)
    {
        x[i] = i;
        yI[i] = _commands->arrayI()[i]*mul;
        yQ[i] = _commands->arrayQ()[i]*mul;
        //yI[i] = yQ[i] = commands->arrayI()[i];
    }

    _customPlot->graph(0)->setPen(QPen(Qt::red));
    _customPlot->graph(0)->setData(x, yI);

    _customPlot->graph(1)->setPen(QPen(Qt::blue));
    _customPlot->graph(1)->setData(x, yQ);

    _customPlot->rescaleAxes();
    _customPlot->replot();

    setBisy(false);

    //testWriteRxTxSin("/home/balmer/radio/stm32/projects/BalmerVNA/script/fft/data_iq.py");
}

bool MainWindow::testWriteRxTxSin(QString filename)
{
    QFile file( filename );
    if ( !file.open(QIODevice::Text|QIODevice::ReadWrite|QIODevice::Truncate) )
        return false;

    QTextStream stream( &file );

    stream << "data = {" << endl;
    stream << "  'freq': " << _commands->currentFreq() << "," << endl;

    for(int IQ=0; IQ<2; IQ++)
    {
        QVector<int32_t>& arr = IQ==0?_commands->arrayI():_commands->arrayQ();
        if(IQ==0)
        {
            stream << "  'I': [";
        } else
        {
            stream << "  'Q': [";
        }

        int count = 0;
        for(auto y : arr)
        {
            if(count%10==0)
                stream << endl << "    ";
            count++;

            stream << y <<", ";
        }

        stream << "  ]," << endl;
    }

    stream << "}" << endl;

    return true;
}

MainWindow::Scan MainWindow::scanType()
{
    return (Scan)_comboRxTx->currentData().toInt();
}

void MainWindow::onRxTxIndexChanged(int index)
{
    (void)index;
}

void MainWindow::onStartSampling()
{
    _dock->updateProject();

    setBisy(true);

    startSamplingInternal(false);
}

void MainWindow::startSamplingInternal(bool reverse)
{
    if(_sampling)
        delete _sampling;
    _sampling = new VnaSampling(&_data);

    connect(_sampling, SIGNAL(signalSample(const HardSamplingData&, int, bool)), this, SLOT(onSample(const HardSamplingData&, int, bool)));
    connect(_sampling, SIGNAL(signalEnd()), this, SLOT(onSamplingEnd()));

    _data.arrFreq = g_project->getScanFrequencyList();
    Scan sc = scanType();

    VnaSampling::Scan samplingScan;
    if(sc == Scan::RX)
        samplingScan = VnaSampling::Scan::RX;
    else
        samplingScan = VnaSampling::Scan::RX_TX;

    _sampling->setSamplingType(samplingScan);

    if(sc == Scan::RX)
        _progressBar->setRange(0, _data.arrFreq.size());
    else
        _progressBar->setRange(0, _data.arrFreq.size()*2);

    _progressBar->setValue(0);

    statusBar()->showMessage(tr("sampling rx"));

    _samplingReverse = reverse;
    _sampling->start();

}

void MainWindow::onSamplingEnd()
{
    if(_samplingReverse)
        _data.addToSparamReverse(_sparam);
    else
        _data.convertToSparam(_sparam);

    if(scanType() == Scan::RX_TX_X2 && !_samplingReverse)
    {
        QMessageBox::question(this, QString(), "Переверните деталь для продолжения сэмплирования.",
                                        QMessageBox::Yes);

        startSamplingInternal(true);
        return;
    }


    setBisy(false);
    statusBar()->showMessage(tr("sampling complete"));

    _progressBar->setValue(_data.arrFreq.size()*(scanType()==Scan::RX?1:2));

    delete _sampling;
    _sampling = nullptr;

    emit signalSParamChanged();
    updateGraph();
}


void MainWindow::onStopSampling()
{
    setBisy(false);
    statusBar()->showMessage(tr("sampling cancelled"));

    delete _sampling;
    _sampling = nullptr;

}

void MainWindow::onSample(const HardSamplingData &data, int freqIndex, bool isTx)
{
    (void)data;
    if(isTx && freqIndex==0)
    {
        statusBar()->showMessage(tr("sampling tx"));
    }

    if(isTx)
    {
        _progressBar->setValue(_data.arrFreq.size()+freqIndex);
    } else
    {
        _progressBar->setValue(freqIndex);
    }
}

void MainWindow::onOpenFile()
{
    QSettings settings;
    QString prevFile = settings.value("prevSavedFile").toString();
    QFileInfo prevFileInfo(prevFile);

    QString openFile = QFileDialog::getOpenFileName(this, QString(), prevFileInfo.absolutePath());

    if(openFile.isEmpty())
        return;

    if(!loadData(openFile))
        return;

    settings.setValue("prevSavedFile", openFile);

    emit signalSParamChanged();

    updateGraph();
}

void MainWindow::onSave()
{
    onSaveWithType(SaveDataType::BVNA);
}

void MainWindow::onSaveTouchstone()
{
    onSaveWithType(SaveDataType::TOUCHSTONE);
}

void MainWindow::onSaveWithType(SaveDataType type)
{
    QSettings settings;
    QString prevFile = settings.value("prevSavedFile").toString();
    QFileInfo prevFileInfo(prevFile);
    QString saveFile = QFileDialog::getSaveFileName(this, QString(), prevFileInfo.absolutePath());
    if(saveFile.isEmpty())
        return;

    QFileInfo saveFileInfo(saveFile);
    if(saveFileInfo.completeSuffix().isEmpty())
    {
        switch(type)
        {
        case SaveDataType::RAW: saveFile.append(".raw"); break;
        case SaveDataType::BVNA: saveFile.append(".bvna"); break;
        case SaveDataType::TOUCHSTONE:
            //if(_sparam.S12valid)
            //    saveFile.append(".s2p");
            //else
                saveFile.append(".s1p");
            break;
        }
    }

    if(saveData(saveFile, type))
        settings.setValue("prevSavedFile", saveFile);
}

bool MainWindow::saveData(const QString& fileName, SaveDataType type)
{

    if(type==SaveDataType::RAW ||
       type==SaveDataType::BVNA)
    {
        pugi::xml_document doc;
        pugi::xml_node root_node = doc.append_child("root");

        root_node.append_attribute("calibration").set_value(g_project->currentCalibrationName().toUtf8().constData());

        if(type==SaveDataType::BVNA)
        {
            //Complex format
            pugi::xml_node data = _sparam.save(root_node);
            data.append_attribute("type").set_value("raw");

            VnaSParam correctedSParam;
            if(makeCorrection(correctedSParam))
            {
                pugi::xml_node data = correctedSParam.save(root_node, false);
                data.append_attribute("type").set_value("corrected");
            }
        } else
        {
            _data.saveRawArray(root_node);
        }

        return doc.save_file(fileName.toUtf8().constData());
    }

    if(type==SaveDataType::TOUCHSTONE)
    {
        VnaSParam correctedSParam;
        if(!makeCorrection(correctedSParam))
            return false;
        return correctedSParam.saveTouchstone(fileName);
    }

    return false;
}

bool MainWindow::loadData(const QString& fileName)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(fileName.toUtf8().constData());
    if(!result)
        return false;

    pugi::xml_node root_node = doc.child("root");
    QString calibration = root_node.attribute("calibration").as_string();

    VnaSParam param;

    pugi::xml_node data_node = root_node.child("data");
    if(!param.load(data_node))
        return false;

    if(!calibration.isEmpty())
    {
        g_project->setCurrentCalibration(calibration);
    }

    _sparam = param;

    return true;
}

void MainWindow::onSettings()
{
    //DialogSelectFrequency dialog;
    //dialog.exec();
}

void MainWindow::onCalibrationDialog()
{
    DialogCalibration dialog(this);
    dialog.exec();
}

void MainWindow::onNewCalibration()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, QString(), tr("Enter calibration name"), QLineEdit::Normal, QString(), &ok);
    if(!ok || name.isEmpty())
        return;
    g_project->setCurrentCalibration(name);
    onCalibrationDialog();
    emit g_project->signalCalibrationNew(name);
}

void MainWindow::onLnX()
{
    _customPlot->xAxis->setScaleType(lnXAct->isChecked() ? QCPAxis::stLogarithmic : QCPAxis::stLinear);
    updateGraph();
}

void MainWindow::onPlotMouseMove(QMouseEvent * event)
{
    QPoint pos = event->pos();

    QCPAbstractPlottable * plottable = _customPlot->plottable();
    if(plottable==nullptr)
        return;
    double x =  plottable->keyAxis()->pixelToCoord(pos.x());
    double freq = x*1e6;
    g_graphSettings->setFreq(freq);

    QList<double> values;

    for(int i=0; i<_customPlot->graphCount(); i++)
    {
        values.append(graphValue(x, i));
    }

    g_graphSettings->setValues(values, freq);

    //QString str;
    //double y =  plottable->valueAxis()->pixelToCoord(pos.y());
    //str = QString("%1\n%2").arg(x).arg(y);
    //QToolTip::showText(_customPlot->mapToGlobal(pos), str, this);
}

double MainWindow::graphValue(double f, int index)
{
    if(index<0)
        return 0;
    if(index>=_customPlot->graphCount())
        return 0;
    //Ищем два ключа, чтобы между ними попадал key.
    //Интерполируем.
    QCPDataMap& data = *_customPlot->graph(index)->data();

    bool valid0 = false;
    bool valid1 = false;
    double f0 = -1;
    double d0 = -1;
    double f1 = -1;
    double d1 = -1;

    for(const QCPData& d : data)
    {
        if(d.key <= f && (f0 < d.key || !valid0))
        {
            //Самое большое, меньше или равное f
            f0 = d.key;
            d0 = d.value;
            valid0 = true;
        }

        if(d.key > f && (f1 > d.key || !valid1))
        {
            //Самое маленькое, больше f
            f1 = d.key;
            d1 = d.value;
            valid1 = true;
        }
    }

    if(f0 < 0 && f1 < 0)
        return 0;
    if(f0 > 0 && f1 < 1)
        return d0;
    if(f0 < 0 && f1 > 1)
        return d1;

    float t = (f-f0)/(f1-f0);

    return d0 + (d1-d0)*t;
}

void MainWindow::onDeviceIdChaged()
{
    if(g_commands->isDeviceVNA())
    {
        statusBar()->showMessage(tr("Connected to VNA"));
        g_commands->appendCommand(new VnaCommandSamplingBufferSize());
        g_commands->appendCommand(new VnaCommandPing());
        g_commands->appendCommand(new VnaCommandSetTx(false));
        _receiveCalibration->start();
    } else
    if(g_commands->isDeviceBootloader())
    {
        statusBar()->showMessage(tr("Connected to Bootloader"));

        UpdateFlashDialog dialog;
        dialog.exec();
    } else
    {
        statusBar()->showMessage(tr("Connected to unknown device"));
    }
}

void MainWindow::onCalibrationReceived()
{
    VnaDataSOLT data;
    if(!_receiveCalibration->getCalibration(data))
        return;

    g_project->setCalibration(_receiveCalibration->calibrationName(), data);
    g_project->setCurrentCalibration(_receiveCalibration->calibrationName());
    _calib.setCoeff(data);
    emit g_project->signalCalibrationNew(_receiveCalibration->calibrationName());
}

void MainWindow::onLevelRFValueChanged(int)
{
    if(_levelRFSetData)
        return;
    g_commands->appendCommand(new VnaSetRelativeLevelRF(_levelRF->value()));
}

void MainWindow::onDeviceRelativeLevelRF(uint8_t level)
{
    _levelRFSetData = true;
    _levelRF->setValue(level);
    _levelRFSetData = false;
}
