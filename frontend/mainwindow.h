#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "pugixml/pugixml.hpp"
#include "qcustomplot/qcustomplot.h"
#include "device/vnacommands.h"
#include "device/vnasampling.h"
#include "calibrationsolt.h"
#include "device/vnareceivecalibration.h"

class QAction;
class QToolBar;
class QLabel;
class QComboBox;

class VnaDevice;
class VnaCommands;
class ScanDock;
class QProgressBar;

enum class SaveDataType
{
    RAW,
    BVNA,
    TOUCHSTONE //s1p, s2p,
};



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum class Scan
    {
        //Сканирует только RX порт
        //Нужно, когда деталь является двухполюсником, подкдюченным к RX зажимам.
        RX,

        //Сканирует RX/TX порты.
        //Деталь является четырехполюсником.
        //Этого достаточно, если детяль является reciprocal и symmetrical.
        RX_TX,

        //Сканирует RX/TX порты. Потом деталь надо переставить (поменять Rx и TX местами).
        //Это наиболее общий случай, когда все 4 S параметра могут быть различными.
        RX_TX_X2,
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateGraph();

    VnaSParam& sparam() { return _sparam; }
signals:
    void signalSParamChanged();
public slots:
    void openSerialPort();
    void sendTestSinData();
    void sendBadPacketData();

    void onCloseSerial();

    void onRxTxIndexChanged(int index);

    void onStartSampling();
    void onStopSampling();

    void onOpenFile();
    void onSave();
    void onSaveTouchstone();
    void onSaveWithType(SaveDataType type);

    void onSettings();

    void onLnX();
    void onPlotMouseMove(QMouseEvent * event);

    void onSample(const HardSamplingData& data, int freqIndex, bool isTx);
    void onSamplingEnd();

    void onCalibrationDialog();
    void onNewCalibration();

    void onEndSamplingSin();

    void onDeviceIdChaged();
    void onCalibrationReceived();
    void onLevelRFValueChanged(int);
    void onDeviceRelativeLevelRF(uint8_t level);
private:
    void createMenu();
    void createActions();
    void createToolbar();
    void createCustomPlot();

    void setStatusConnected(bool connected);
    void setBisy(bool bisy);

    bool saveData(const QString& fileName, SaveDataType type);
    bool loadData(const QString& fileName);

    //Сейчас девайс сэмплирует tx данные (иначе rx)
    Scan scanType();

    void startSamplingInternal(bool reverse);

    void updateGraphRaw();
    void setErrorsInGraph(int i);
    void calcS4(QVector<double>& arrFreqM, QVector<complexf>& S11, QVector<complexf>& S21, QVector<complexf>& S12, QVector<complexf>& S22);
    bool makeCorrection(VnaSParam& correctedSParam);

    double graphValue(double freq, int index);

    void enableHardwareCommands(bool enable);

    bool testWriteRxTxSin(QString filename);
private:
    VnaDevice* _device;
    VnaCommands* _commands;
    VnaSampling* _sampling;
    //Сэмплируемые в текущий момент данные.
    VnaSamplingData _data;
    //Данные после сэмплирования откорвентированные.
    VnaSParam _sparam;

    VnaReceiveCalibration* _receiveCalibration;

    QToolBar* mainToolBar;
    ScanDock* _dock;
    QComboBox* _comboRxTx;
    QProgressBar* _progressBar;


    QAction* settingsAct;
    QAction* sendTestSinAct;
    QAction* sendBadPacketAct;

    QAction* startSamplingAct;
    QAction* stopSamplingAct;
    QAction* openAct;
    QAction* saveAct;
    QAction* saveTouchstoneAct;
    QAction* lnXAct;

    QLabel* _statusConnect;
    QLabel* _statusBisy;

    QCustomPlot* _customPlot;

    QPixmap _pix_red_orb;
    QPixmap _pix_green_orb;

    QSpinBox* _levelRF;
    bool _levelRFSetData = false;

    //true для случая RX_TX_X2, когда мы переворачиваем деталь и сэмплируем ее "задом наперед"
    bool _samplingReverse = false;

    CalibrationSOLT _calib;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
