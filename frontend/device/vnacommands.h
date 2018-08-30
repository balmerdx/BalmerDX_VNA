#ifndef VNACOMMANDS_H
#define VNACOMMANDS_H

#include <QObject>
#include <QVector>
#include "../4code/src/commands.h"
#include "device/vnautils.h"


class VnaDevice;

class VnaCommand
{
public:
    VnaCommand();
    virtual ~VnaCommand();

    void startCommand(USB_COMMANDS command);
    void endCommand();
    void add(const uint8_t* data, uint32_t size);
    void add8(uint8_t data);
    void add16(uint16_t data);
    void add32(uint32_t data);

    virtual USB_COMMANDS command();

    //Вызывается один раз при запуске команды
    virtual void start()=0;

    virtual void onPacket(uint8_t* cdata, int csize);

    //return msec time
    virtual uint32_t waitBeforeStart();

    virtual bool firstByteIsCommand() { return true; }
protected:
    USB_COMMANDS _command;
    uint32_t _waitTime;//msec
};

class VnaCommandPing : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    uint32_t pingIdx;
};

class VnaCommandBigData : public VnaCommand
{
protected:
    uint16_t imin;
    uint16_t imax;
public:
    VnaCommandBigData(uint16_t imin, uint16_t imax);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandSetFreq : public VnaCommand
{
    uint32_t freq;
public:
    VnaCommandSetFreq(uint32_t freq);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandStartSampling : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandSamplingComplete : public VnaCommand
{
public:
    VnaCommandSamplingComplete(int retryCount);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    int retryCount;
};

class VnaCommandSamplingBufferSize : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandGetSamples : public VnaCommand
{
public:
    VnaCommandGetSamples(bool sampleQ, uint16_t offset, uint16_t count);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

    bool firstByteIsCommand() override { return false; }
protected:
    bool sampleQ;
    uint16_t offset;
    uint16_t count;
};

class VnaCommandEndSampling : public VnaCommand
{
public:
    void start();
};

class VnaCommandSetTx : public VnaCommand
{
public:
    VnaCommandSetTx(bool tx);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    bool tx;
};

struct HardSamplingData
{
    float freq;
    float q_cconst;
    float q_csin;
    float q_ccos;
    float q_sqr;
    float i_cconst;
    float i_csin;
    float i_ccos;
    float i_sqr;
    uint16_t result_time;
    uint16_t nop;

    HardSamplingData();
    complexf toComplex() const;
    float squareMean() const;
};

class VnaCommandStartSamplingAndCalculate : public VnaCommand
{
public:
    VnaCommandStartSamplingAndCalculate();
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandGetCalculated : public VnaCommand
{
public:
    VnaCommandGetCalculated(int retryCount);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

protected:
    int retryCount;
};

class VnaCommandStartWriteFlash : public VnaCommand
{
public:
    VnaCommandStartWriteFlash(uint32_t program_size, const QByteArray& hash);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

protected:
    uint32_t _program_size;
    QByteArray _hash;
};

class VnaCommandWriteFlash : public VnaCommand
{
public:
    VnaCommandWriteFlash(uint8_t* data);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

protected:
    uint8_t _data[WRITE_TO_FLASH_SIZE];
};

class VnaCommandSendBack : public VnaCommand
{
public:
    VnaCommandSendBack(const std::vector<uint8_t>& data);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

protected:
    std::vector<uint8_t> _data;
};

class VnaCommandJumpToBootloader : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
};

class VnaCommandGetCalibration : public VnaCommand
{
public:
    VnaCommandGetCalibration(GET_CALIBRATION_ENUM type);
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

    GET_CALIBRATION_ENUM type() const { return _type; }
    const std::vector<uint64_t>& freq() const { return _freq; }
    const std::vector<complexf>& cal() const { return _cal;}
protected:
    GET_CALIBRATION_ENUM _type;
    std::vector<uint64_t> _freq;
    std::vector<complexf> _cal;
};

class VnaCommandGetCalibrationName : public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;

    QString name() const { return _name; }
protected:
    QString _name;
};


class VnaSetRelativeLevelRF: public VnaCommand
{
public:
    VnaSetRelativeLevelRF(uint8_t level) : level(level) {}
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
    uint8_t level;
};

class VnaGetRelativeLevelRF: public VnaCommand
{
public:
    void start() override;
    void onPacket(uint8_t* cdata, int csize) override;
protected:
};


class VnaCommands : public QObject
{
    Q_OBJECT
public:

public:
    explicit VnaCommands(VnaDevice* device, QObject *parent = 0);
    ~VnaCommands();

    void appendCommand(VnaCommand* command, bool autostart=true);
    void startCommand(bool wait=true);

    uint32_t currentFreq() const { return _currentFreq; }
    void setCurrentFreq(uint32_t freq) { _currentFreq = freq; }  //вызывается только из команды

    uint16_t samplingBufferSize() const { return _samplingBufferSize; }
    void setSamplingBufferSize(uint16_t size);  //вызывается только из команды

    bool samplingStarted() const { return _samplingStarted; }
    void setSamplingStarted(bool b) { _samplingStarted = b;} //вызывается только из команды

    void commandSampling(uint32_t freq);

    QVector<int32_t>& arrayI() { return _arrayI; }
    QVector<int32_t>& arrayQ() { return _arrayQ; }

    const HardSamplingData& hardData() { return _hardSamplingData; }
    void setHardData(const HardSamplingData& data);  //вызывается только из команды

    void setDeviceSelfID(uint32_t id);

    bool isDeviceVNA() const { return _deviceId==SELF_ID_VNA; }
    bool isDeviceBootloader() const { return _deviceId==SELF_ID_BOOTLOADER; }


    const std::vector<uint64_t>& calibrationFreq() const { return _calibrationFreq; }
    const std::vector<std::vector<complexf> >& calibrationS() const { return _calibrationS; }
signals:
    //Использовать connect(, Qt::QueuedConnection) при подключении к этим сигналам.
    //Иначе внутри сигнала нельзя будет отсылать пакеты с данными
    void signalBadPacket();
    void signalSetFreq(uint32_t freq);
    void signalEndSampling();
    void signalReceiveHard();
    void signalDeviceIdChaged();
    void signalBootEraseFlash(uint32_t error, uint32_t sectors_to_erase, uint32_t size, uint32_t fit_size);
    void signalBootWriteFlash(uint32_t offset, uint8_t ok);
    void signalSendBack(int size);

    void signalCalibrationFreq();
    //type == GET_CALIBRATION_ENUM
    void signalCalibrationData(uint32_t type);
    void signalCalibrationName(QString name);

    void signalRelativeLevelRF(uint8_t level);
public slots:
    void onPacket(const QByteArray& data);
    void onWaitStart();
public:
    //Для VnaCommand и ее потомков
    VnaDevice* device;
    //Вывести следующюю команду в дебаговый output
    void debugRaw();
    //Пришла плохая команда, очищаем очередь команд.
    void badPacket();
    //Перезапускаем текущую команду
    void restartCurrentCommand() { _restartCurrentCommand = true; }

protected:
    void onReceiveBadPacket(const QByteArray& data);
    void printDebug(const QByteArray& data);

protected:
    //Какие команды мы уже послали (что-бы проверять на верность ответы)
    QList<VnaCommand*> commandQueue;

    int numTryBad;
    bool _debugRaw;
    bool _badPacket;
    bool _restartCurrentCommand;

    uint32_t _currentFreq;

    bool _samplingStarted;
    uint16_t _samplingBufferSize;

    QVector<int32_t> _arrayI;
    QVector<int32_t> _arrayQ;

    HardSamplingData _hardSamplingData;

    uint32_t _deviceId;

    std::vector<uint64_t> _calibrationFreq;
    std::vector<std::vector<complexf> > _calibrationS;

    friend class VnaCommandGetSamples;
    friend class VnaCommandGetCalibration;
};

extern VnaCommands* g_commands;

#endif // VNACOMMANDS_H
