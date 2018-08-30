#include "vnacommands.h"

#include "vnadevice.h"

#include "../4code/src/job_state.h"
#include "md5.h"

#include <QDebug>
#include <QTimer>

Q_DECLARE_METATYPE(uint32_t);

VnaCommands* g_commands = nullptr;

static bool com_debug = false;

uint32_t g_pingIdx = 12;

VnaCommands::VnaCommands(VnaDevice *device, QObject *parent)
    : QObject(parent)
    , device(device)
    , numTryBad(0)
    , _debugRaw(false)
    , _badPacket(false)
    , _restartCurrentCommand(false)
    , _currentFreq(0)
    , _samplingStarted(false)
    , _samplingBufferSize(0)
    , _deviceId(0)
{
    //for Qt::QueuedConnection
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint8_t>("uint8_t");

    g_commands = this;
    connect(device, SIGNAL(signalPacket(const QByteArray&)), this, SLOT(onPacket(const QByteArray&)));

    //Элемент для GET_CALIBRATION_FREQ не используется
    _calibrationS.resize(GET_CALIBRATION_S21_OPEN+1);
}

VnaCommands::~VnaCommands()
{
    g_commands = nullptr;
}

void VnaCommands::onPacket(const QByteArray& data)
{
    uint8_t* cdata = (uint8_t*)data.data();
    int csize = data.size();

    VnaCommand* cmd = commandQueue[0];

    if(cmd->firstByteIsCommand())
    {
        USB_COMMANDS command;
        if(csize==0)
        {
            //Bad command
            command = (USB_COMMANDS)-1;
        } else
        {
            cdata++;
            csize--;
            command = (USB_COMMANDS)((uint8_t)data[0]);
        }

        if(commandQueue.size()==0)
        {
            onReceiveBadPacket(data);
            return;
        }


        if(cmd->command()!=command)
        {
            onReceiveBadPacket(data);
            return;
        }

        cmd->onPacket(cdata, csize);
    } else
    {
        cmd->onPacket(cdata, csize);
    }

    if(!_restartCurrentCommand)
    {
        commandQueue.removeFirst();
        delete cmd;
    }

    if(_badPacket)
    {
        _badPacket = false;
        _debugRaw = false;
        onReceiveBadPacket(data);
    }

    if(_debugRaw)
    {
        _debugRaw = false;
        printDebug(data);
    }

    _restartCurrentCommand = false;

    startCommand();
}

void VnaCommands::printDebug(const QByteArray& data)
{
    //debug commands
    QString strDebug;
    for(uint8_t c : data)
    {
        strDebug += QString::number(c, 16);
        strDebug += " ";
    }

    qDebug() << "Receive:" << strDebug;
}

void VnaCommands::appendCommand(VnaCommand* command, bool autostart)
{
    commandQueue.append(command);
    if(autostart && commandQueue.size()==1)
    {
        startCommand();
    }
}

void VnaCommands::startCommand(bool wait)
{
    while(!commandQueue.empty())
    {
        VnaCommand* cmd = commandQueue.first();

        if(wait)
        if(cmd->waitBeforeStart()>0)
        {
            QTimer::singleShot(cmd->waitBeforeStart(), this, SLOT(onWaitStart()));
            break;
        }

        wait = true;

        cmd->start();
        if(cmd->command()!=COMMAND_BAD)
        {
            if(com_debug)
                qDebug() << "Start command " << cmd->command();
            break;
        }


        commandQueue.removeFirst();
        delete cmd;
    }
}

void VnaCommands::onWaitStart()
{
    if(com_debug)
        qDebug() << "onWaitStart()";
    startCommand(false);
}

void VnaCommands::onReceiveBadPacket(const QByteArray& data)
{
    //Тут надо очистить всю очередь команд и както восстановить работу системы.
    qDebug() << "Bad packet received!";
    printDebug(data);

    for(VnaCommand* cmd : commandQueue)
    {
        delete cmd;
    }
    commandQueue.clear();

    emit signalBadPacket();
}

void VnaCommands::debugRaw()
{
    _debugRaw = true;
}

void VnaCommands::badPacket()
{
    _badPacket = true;
}

void VnaCommands::setSamplingBufferSize(uint16_t size)
{
    _samplingBufferSize = size;
    _arrayI.resize(_samplingBufferSize);
    _arrayQ.resize(_samplingBufferSize);

    for(int i=0; i<_samplingBufferSize; i++)
    {
        _arrayI[i] = 0;
        _arrayQ[i] = 0;
    }
}

void VnaCommands::commandSampling(uint32_t freq)
{
    appendCommand(new VnaCommandSetFreq(freq), false);
    appendCommand(new VnaCommandStartSampling(), false);
    appendCommand(new VnaCommandSamplingComplete(10), false);

    for(int sampleQ = 0; sampleQ<2; sampleQ++)
    {
        int count = 250; //Т.к. буфер на девайсе у нас 2048 и надо оставить запас на конвертацию FE и FF
        for(int offset=0; offset<_samplingBufferSize; offset+=count)
        {
            appendCommand(new VnaCommandGetSamples(sampleQ?true:false, offset, qMin(count, _samplingBufferSize-offset)), false);
        }
    }

    appendCommand(new VnaCommandEndSampling(), false);

    startCommand();
}

void VnaCommands::setHardData(const HardSamplingData& data)
{
    _hardSamplingData = data;
}

///////////////////////////////////////////////////////////////////////////////////
VnaCommand::VnaCommand()
{
    _command = COMMAND_BAD;
    _waitTime = 0;
}

VnaCommand::~VnaCommand()
{

}

USB_COMMANDS VnaCommand::command()
{
    return _command;
}

uint32_t VnaCommand::waitBeforeStart()
{
    return _waitTime;
}

void VnaCommand::onPacket(uint8_t* cdata, int csize)
{
    (void)cdata;
    (void)csize;
}


void VnaCommand::startCommand(USB_COMMANDS command)
{
    _command = command;
    g_commands->device->startCommand(command);
}

void VnaCommand::endCommand()
{
    g_commands->device->endCommand();
}

void VnaCommand::add(const uint8_t* data, uint32_t size)
{
    g_commands->device->add(data, size);
}

void VnaCommand::add8(uint8_t data)
{
    g_commands->device->add8(data);
}

void VnaCommand::add16(uint16_t data)
{
    g_commands->device->add16(data);
}

void VnaCommand::add32(uint32_t data)
{
    g_commands->device->add32(data);
}


void VnaCommandPing::start()
{
    pingIdx = g_pingIdx++;
    startCommand(COMMAND_PING);
    add32(pingIdx);
    endCommand();
}

void VnaCommandPing::onPacket(uint8_t* cdata, int csize)
{
    if(com_debug)
        g_commands->debugRaw();

    uint32_t* c32data = (uint32_t*)cdata;

    if(csize!=8 || pingIdx!=c32data[0])
    {
        g_commands->badPacket();
        return;
    }

    g_commands->setDeviceSelfID(c32data[1]);
}

VnaCommandBigData::VnaCommandBigData(uint16_t imin, uint16_t imax)
    : imin(imin)
    , imax(imax)
{

}

void VnaCommandBigData::start()
{
    startCommand(COMMAND_BIG_DATA);
    add16(imin);
    add16(imax);
    endCommand();
}

void VnaCommandBigData::onPacket(uint8_t* cdata, int csize)
{
    (void)cdata;
    qDebug() << "Command 'Big data' size="<<csize;
    g_commands->debugRaw();
}

/*
 * Не забыть про!!!!
    if(command==COMMAND_START_SAMPLING)
    {
        //На эту команду не приходит никаких данных в ответ.
        return;
    }
 */

VnaCommandSetFreq::VnaCommandSetFreq(uint32_t freq)
    : freq(freq)
{

}

void VnaCommandSetFreq::start()
{
    startCommand(COMMAND_SET_FREQ);
    add32(freq);
    endCommand();
}

void VnaCommandSetFreq::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=4);
    uint32_t freq = *(uint32_t*)cdata;
    if(com_debug)
        qDebug() << "COM: Set freq" << freq;
    g_commands->setCurrentFreq(freq);
}

void VnaCommandStartSampling::start()
{
    startCommand(COMMAND_START_SAMPLING);
    endCommand();
}

void VnaCommandStartSampling::onPacket(uint8_t* cdata, int csize)
{
    (void)cdata;
    Q_ASSERT(csize==1);
    g_commands->setSamplingStarted(true);
}


VnaCommandSamplingComplete::VnaCommandSamplingComplete(int retryCount)
    : retryCount(retryCount)
{
    _waitTime = 10;
}

void VnaCommandSamplingComplete::start()
{
    startCommand(COMMAND_SAMPLING_COMPLETE);
    endCommand();
}

void VnaCommandSamplingComplete::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=1);
    bool complete = cdata[0]?true:false;
    if(com_debug)
        qDebug() << "COM: Sampling complete" << complete;

    if(!complete)
    {
        if(retryCount>0)
        {
            retryCount--;
            g_commands->restartCurrentCommand();
        }
    } else
    {
        g_commands->setSamplingStarted(false);
    }
}

void VnaCommandSamplingBufferSize::start()
{
    startCommand(COMMAND_SAMPLING_BUFFER_SIZE);
    endCommand();
}

void VnaCommandSamplingBufferSize::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=2);
    uint16_t bufferSize = *(uint16_t*)cdata;
    if(com_debug)
        qDebug() << "COM: Sampling buffer size=" << bufferSize;
    g_commands->setSamplingBufferSize(bufferSize);
}

VnaCommandGetSamples::VnaCommandGetSamples(bool sampleQ, uint16_t offset, uint16_t count)
    : sampleQ(sampleQ)
    , offset(offset)
    , count(count)
{

}

void VnaCommandGetSamples::start()
{
    startCommand(COMMAND_GET_SAMPLES);
    add8(sampleQ?1:0);
    add16(offset);
    add16(count);
    endCommand();
}

void VnaCommandGetSamples::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==count*4);
    int32_t* inData = (int32_t*)cdata;

    if(com_debug)
        qDebug() << "COM: Receive samples "<< (sampleQ?"Q":"I") <<" " <<offset << "," << count;

    QVector<int32_t>& v = sampleQ?g_commands->_arrayQ:g_commands->_arrayI;
    //QString s;

    for(int i=0; i<count; i++)
    {
        v[offset+i] = inData[i];
        //s += " " + QString::number(inData[i]);
    }

    //qDebug() << "samples=" << s;
}

void VnaCommandEndSampling::start()
{
    emit g_commands->signalEndSampling();
}

VnaCommandSetTx::VnaCommandSetTx(bool tx)
    : tx(tx)
{

}

void VnaCommandSetTx::start()
{
    startCommand(COMMAND_SET_TX);
    add8(tx?1:0);
    endCommand();
}

void VnaCommandSetTx::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==1);
    if(com_debug)
        qDebug() << "tx=" << cdata[0];
}

VnaCommandStartSamplingAndCalculate::VnaCommandStartSamplingAndCalculate()
{
    _waitTime = 1;
}

void VnaCommandStartSamplingAndCalculate::start()
{
    startCommand(COMMAND_START_SAMPLING_AND_CALCULATE);
    endCommand();
}

void VnaCommandStartSamplingAndCalculate::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==1);
    bool ok = cdata[0]?true:false;
    Q_ASSERT(ok);
    //qDebug() << "StartSamplingAndCalculate=" << ok;
}

VnaCommandGetCalculated::VnaCommandGetCalculated(int retryCount)
    : retryCount(retryCount)
{
    _waitTime = 20;
}

void VnaCommandGetCalculated::start()
{
    startCommand(COMMAND_GET_CALCULATED);
    endCommand();
}

void VnaCommandGetCalculated::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=1);
    uint8_t jobState = cdata[0];
    if(com_debug)
        qDebug() << "COM: Sampling job " << jobState;

    if(jobState!=JOB_CALCULATING_COMPLETE)
    {
        if(retryCount>0)
        {
            retryCount--;
            g_commands->restartCurrentCommand();
        }
    } else
    {
        Q_ASSERT(csize==1+sizeof(HardSamplingData));

        g_commands->setHardData(*(HardSamplingData*)(cdata+1));
        emit g_commands->signalReceiveHard();
    }

}

HardSamplingData::HardSamplingData()
{
    freq = 0;
    q_cconst = 0;
    q_csin = 0;
    q_ccos = 0;
    q_sqr = 0;
    i_cconst = 0;
    i_csin = 0;
    i_ccos = 0;
    i_sqr = 0;
    result_time = 0;
    nop = 0;
}

complexf HardSamplingData::toComplex() const
{
    if(freq==0)
        return complexf(0,0);

    complexf Zi(i_ccos, i_csin);
    complexf Zq(q_ccos, q_csin);

    if(false)//Test code
    {   //dB(ref channel amplitude)
        float y = abs(Zq)*1e-7/27;
        return 20*log(y)/log(10);
    }

    return Zi/Zq;
}

float HardSamplingData::squareMean() const
{
    float qabs = std::abs(complexf(q_ccos, q_csin));
    return i_sqr/qabs;
}

void VnaCommands::setDeviceSelfID(uint32_t id)
{
    uint32_t oldId = _deviceId;
    _deviceId = id;

    if(_deviceId!=oldId)
        emit signalDeviceIdChaged();
}


VnaCommandStartWriteFlash::VnaCommandStartWriteFlash(uint32_t program_size, const QByteArray& hash)
    : _program_size(program_size)
    , _hash(hash)
{
    Q_ASSERT(_hash.size()==MD5_HASH_SIZE);
}

void VnaCommandStartWriteFlash::start()
{
    startCommand(COMMAND_START_WRITE_FLASH);
    add32(_program_size);
    add((uint8_t*)_hash.data(), _hash.size());
    endCommand();
}

void VnaCommandStartWriteFlash::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=10);
    uint8_t error = cdata[0];
    uint8_t sectors_to_erase = cdata[1];
    cdata+=2;
    uint32_t size = ((uint32_t*)cdata)[0];
    uint32_t fit_size = ((uint32_t*)cdata)[1];

    Q_ASSERT(size==_program_size);

    emit g_commands->signalBootEraseFlash(error, sectors_to_erase, size, fit_size);
}


VnaCommandWriteFlash::VnaCommandWriteFlash(uint8_t* data)
{
    memcpy(_data, data, WRITE_TO_FLASH_SIZE);
}

void VnaCommandWriteFlash::start()
{
    startCommand(COMMAND_WRITE_FLASH);
    add(_data, WRITE_TO_FLASH_SIZE);
    endCommand();
}

void VnaCommandWriteFlash::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize>=5);
    uint8_t ok = cdata[0];
    uint32_t offset = *(uint32_t*)(cdata+1);

    emit g_commands->signalBootWriteFlash(offset, ok);
}

VnaCommandSendBack::VnaCommandSendBack(const std::vector<uint8_t>& data)
    : _data(data)
{

}

void VnaCommandSendBack::start()
{
    startCommand(COMMAND_SEND_BACK);
    add(_data.data(), (uint32_t)_data.size());
    endCommand();
}

void VnaCommandSendBack::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==(int)_data.size());
    Q_ASSERT(memcmp(cdata, _data.data(), _data.size())==0);
    emit g_commands->signalSendBack(csize);
}

void VnaCommandJumpToBootloader::start()
{
    startCommand(COMMAND_JUMP_TO_BOOTLOADER);
    endCommand();
}

void VnaCommandJumpToBootloader::onPacket(uint8_t* cdata, int csize)
{
    (void)cdata;
    (void)csize;
    qDebug() << "VnaCommandJumpToBootloader::onPacket";
}

VnaCommandGetCalibration::VnaCommandGetCalibration(GET_CALIBRATION_ENUM type)
    : _type(type)
{
}

void VnaCommandGetCalibration::start()
{
    startCommand(COMMAND_GET_CALIBRATION);
    add8((uint8_t)_type);
    endCommand();
}

void VnaCommandGetCalibration::onPacket(uint8_t* cdata, int csize)
{
    uint16_t count = *(uint16_t*)cdata; cdata+=2; csize-=2;
    if(count==0)
        return;

    Q_ASSERT(csize==count*8);

    if(_type==GET_CALIBRATION_FREQ)
    {
        _freq.resize(count);
        for(int i=0; i<count; i++)
        {
            _freq[i] = ((uint64_t*)cdata)[i];
        }

        g_commands->_calibrationFreq = _freq;
        emit g_commands->signalCalibrationFreq();
    } else
    {
        Q_ASSERT(sizeof(complexf)==8);
        _cal.resize(count);
        for(int i=0; i<count; i++)
        {
            complexf c;
            memcpy(&c, ((complexf*)cdata)+i, sizeof(complexf));
            _cal[i] = c;
        }

        g_commands->_calibrationS[_type] = _cal;

        emit g_commands->signalCalibrationData(_type);
    }

}

void VnaCommandGetCalibrationName::start()
{
    startCommand(COMMAND_GET_CALIBRATION_NAME);
    endCommand();
}

void VnaCommandGetCalibrationName::onPacket(uint8_t* cdata, int csize)
{
    (void)csize;
    _name = (char*)cdata;
    emit g_commands->signalCalibrationName(_name);
}

void VnaSetRelativeLevelRF::start()
{
    startCommand(COMMAND_SET_RELATIVE_LEVEL_RF);
    add8(level);
    endCommand();
}

void VnaSetRelativeLevelRF::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==1);
    qDebug() << "VnaSetRelativeLevelRF level=" << cdata[0];
}

void VnaGetRelativeLevelRF::start()
{
    startCommand(COMMAND_GET_RELATIVE_LEVEL_RF);
    endCommand();
}

void VnaGetRelativeLevelRF::onPacket(uint8_t* cdata, int csize)
{
    Q_ASSERT(csize==1);
    qDebug() << "VnaGetRelativeLevelRF level=" << cdata[0];
    emit g_commands->signalRelativeLevelRF(cdata[0]);
}
