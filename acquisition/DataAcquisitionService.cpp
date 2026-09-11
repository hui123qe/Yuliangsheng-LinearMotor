#include "DataAcquisitionService.h"

#include "ExcelSessionWriter.h"
#include "ACSC.h"

#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>
#include <QMetaObject>
#include <QThread>
#include <QTimer>

namespace {

const int kAcquisitionBlockCount = 5;
const int kMetadataPollIntervalMilliseconds = 20;
const int kStartTimeoutMilliseconds = 2000;
const int kStopTimeoutMilliseconds = 5000;
const int kMaximumBlockSampleCount = 100000;

void acquisitionTrace(const QString& message)
{
    qInfo().noquote() << QStringLiteral("[ACQ_TRACE][thread=%1] %2")
                            .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()),
                                 0, 16)
                            .arg(message);
}

QString acsErrorText()
{
    const int errorCode = acsc_GetLastError();
    char buffer[512] = {};
    int received = 0;
    if (acsc_GetErrorString(ACSC_INVALID,
                            errorCode,
                            buffer,
                            static_cast<int>(sizeof(buffer) - 1),
                            &received)) {
        return QStringLiteral("ACS错误 %1：%2")
            .arg(errorCode)
            .arg(QString::fromLocal8Bit(buffer, received));
    }
    return QStringLiteral("ACS错误 %1").arg(errorCode);
}

} // namespace

class AcquisitionWorker final : public QObject
{
    Q_OBJECT

public:
    AcquisitionWorker(const AcquisitionConnectionSettings& connectionSettings,
                      ExcelSessionWriter* writer)
        : connectionSettings_(connectionSettings),
          controllerHandle_(ACSC_INVALID),
          pollTimer_(new QTimer(this)),
          writer_(writer),
          active_(false),
          stopRequested_(false),
          controllerStarted_(false),
          nextSequence_(1),
          lastLoggedAcquisitionState_(-1),
          lastLoggedPublishedSequence_(-1),
          currentSampleCount_(0),
          temperatureSampleCount_(0)
    {
        pollTimer_->setInterval(kMetadataPollIntervalMilliseconds);
        connect(pollTimer_, &QTimer::timeout, this, &AcquisitionWorker::pollController);
    }

    ~AcquisitionWorker() override
    {
        closeController();
    }

    bool startSession(const QString& outputDirectory,
                      const AcquisitionConditions& conditions,
                      QString* errorMessage)
    {
        acquisitionTrace(QStringLiteral("Worker收到启动请求，输出目录=%1")
                             .arg(outputDirectory));
        if (active_) {
            acquisitionTrace(QStringLiteral("启动被拒绝：已有活动会话"));
            setError(errorMessage, QStringLiteral("采集记录正在运行。"));
            return false;
        }
        if (outputDirectory.isEmpty()) {
            acquisitionTrace(QStringLiteral("启动被拒绝：输出目录为空"));
            setError(errorMessage, QStringLiteral("采集数据保存目录为空。"));
            return false;
        }
        if (!ensureControllerConnected(errorMessage)) {
            acquisitionTrace(QStringLiteral("启动被拒绝：采集专用ACS连接失败"));
            return false;
        }

        int acquisitionRun = 0;
        int acquisitionState = 0;
        acquisitionTrace(QStringLiteral("开始读取Buffer初始状态"));
        if (!readIntegerScalar("ACQ_RUN", &acquisitionRun)
            || !readIntegerScalar("ACQ_STATE", &acquisitionState)) {
            setError(errorMessage,
                     QStringLiteral("无法读取采集Buffer状态：%1").arg(acsErrorText()));
            return false;
        }
        acquisitionTrace(QStringLiteral("Buffer初始状态：ACQ_RUN=%1，ACQ_STATE=%2")
                             .arg(acquisitionRun)
                             .arg(acquisitionState));
        if (acquisitionRun != 0 || acquisitionState != 0) {
            setError(errorMessage,
                     QStringLiteral("采集Buffer未处于空闲状态：ACQ_RUN=%1，ACQ_STATE=%2。")
                         .arg(acquisitionRun)
                         .arg(acquisitionState));
            return false;
        }

        session_ = AcquisitionSessionData();
        session_.startedAt = QDateTime::currentDateTime();
        session_.outputDirectory = outputDirectory;
        session_.conditions = conditions;

        double controllerSamplePeriod = 0.0;
        if (readRealScalar("ACQ_SAMPLE_PERIOD", &controllerSamplePeriod)
            && controllerSamplePeriod > 0.0) {
            session_.conditions.currentSamplePeriodSeconds = controllerSamplePeriod;
        }

        bool writerStarted = false;
        QString writerError;
        QMetaObject::invokeMethod(writer_, [this, &writerStarted, &writerError]() {
            writerStarted = writer_->startSession(session_, &writerError);
        }, Qt::BlockingQueuedConnection);
        if (!writerStarted) {
            setError(errorMessage, writerError);
            session_ = AcquisitionSessionData();
            return false;
        }

        nextSequence_ = 1;
        stopRequested_ = false;
        controllerStarted_ = false;
        lastLoggedAcquisitionState_ = -1;
        lastLoggedPublishedSequence_ = -1;
        currentSampleCount_ = 0;
        temperatureSampleCount_ = 0;
        stopTimer_.invalidate();
        acquisitionTrace(QStringLiteral("准备写入ACQ_RUN=1"));
        if (!writeAcquisitionRun(1)) {
            setError(errorMessage,
                     QStringLiteral("无法启动采集Buffer：%1").arg(acsErrorText()));
            abortWriterSession();
            session_ = AcquisitionSessionData();
            return false;
        }
        acquisitionTrace(QStringLiteral("ACQ_RUN=1写入成功，等待Buffer运行"));

        QElapsedTimer startTimer;
        startTimer.start();
        int runningState = 0;
        while (startTimer.elapsed() < kStartTimeoutMilliseconds) {
            if (!readIntegerScalar("ACQ_STATE", &runningState)) {
                writeAcquisitionRun(0);
                setError(errorMessage,
                         QStringLiteral("确认采集Buffer启动状态失败：%1")
                             .arg(acsErrorText()));
                abortWriterSession();
                session_ = AcquisitionSessionData();
                return false;
            }
            if (runningState == 2) {
                controllerStarted_ = true;
                break;
            }
            if (runningState == 4) {
                break;
            }
            QThread::msleep(10);
        }
        if (!controllerStarted_) {
            writeAcquisitionRun(0);
            setError(errorMessage,
                     QStringLiteral("采集Buffer未在%1毫秒内进入运行状态，当前ACQ_STATE=%2。")
                         .arg(kStartTimeoutMilliseconds)
                         .arg(runningState));
            abortWriterSession();
            session_ = AcquisitionSessionData();
            return false;
        }

        active_ = true;
        pollTimer_->start();
        acquisitionTrace(QStringLiteral("采集会话启动完成，ACQ_STATE=%1")
                             .arg(runningState));
        emit sessionStarted();
        return true;
    }

    void appendTemperatureSamples(const QVector<TemperatureSample>& samples)
    {
        if (active_ && !stopRequested_ && !samples.isEmpty()) {
            temperatureSampleCount_ += samples.size();
            ExcelSessionWriter* writer = writer_;
            QMetaObject::invokeMethod(writer, [writer, samples]() {
                writer->appendTemperatureSamples(samples);
            }, Qt::QueuedConnection);
        }
    }

    void handleWriterFailure(const QString& message)
    {
        if (active_) {
            failSession(message);
        }
    }

    void stopSession()
    {
        if (!active_ || stopRequested_) {
            acquisitionTrace(QStringLiteral("忽略重复停止请求：active=%1，stopRequested=%2")
                                 .arg(active_)
                                 .arg(stopRequested_));
            return;
        }

        stopRequested_ = true;
        stopTimer_.start();
        acquisitionTrace(QStringLiteral("准备写入ACQ_RUN=0"));
        if (!writeAcquisitionRun(0)) {
            failSession(QStringLiteral("无法停止采集Buffer：%1").arg(acsErrorText()));
            return;
        }
        acquisitionTrace(QStringLiteral("ACQ_RUN=0写入成功，等待Buffer排空"));
        if (!pollTimer_->isActive()) {
            pollTimer_->start();
        }
    }

    void shutdownNow()
    {
        acquisitionTrace(QStringLiteral("采集服务开始关闭，active=%1").arg(active_));
        if (active_) {
            stopSession();
            QElapsedTimer deadline;
            deadline.start();
            while (active_ && deadline.elapsed() < kStopTimeoutMilliseconds) {
                pollController();
                if (active_) {
                    QThread::msleep(kMetadataPollIntervalMilliseconds);
                }
            }
            if (active_) {
                failSession(QStringLiteral("关闭程序时采集Buffer排空超时。"));
            }
        }
        closeController();
        acquisitionTrace(QStringLiteral("采集服务关闭完成"));
    }

signals:
    void sessionStarted();
    void sessionStopped(const QStringList& savedFiles);
    void sessionFailed(const QString& message);

private slots:
    void pollController()
    {
        if (!active_) {
            pollTimer_->stop();
            return;
        }

        int acquisitionState = 0;
        int publishedSequence = 0;
        if (!readIntegerScalar("ACQ_STATE", &acquisitionState)
            || !readIntegerScalar("ACQ_PUBLISHED_SEQ", &publishedSequence)) {
            failSession(QStringLiteral("读取采集Buffer元数据失败：%1")
                            .arg(acsErrorText()));
            return;
        }
        if (acquisitionState != lastLoggedAcquisitionState_
            || publishedSequence != lastLoggedPublishedSequence_) {
            acquisitionTrace(QStringLiteral("Buffer状态变化：ACQ_STATE=%1，ACQ_PUBLISHED_SEQ=%2，next=%3")
                                 .arg(acquisitionState)
                                 .arg(publishedSequence)
                                 .arg(nextSequence_));
            lastLoggedAcquisitionState_ = acquisitionState;
            lastLoggedPublishedSequence_ = publishedSequence;
        }

        if (!controllerStarted_) {
            if (acquisitionState == 4) {
                failSession(QStringLiteral("采集Buffer进入错误状态。"));
                return;
            }
            if (acquisitionState == 2) {
                controllerStarted_ = true;
                nextSequence_ = 1;
            } else {
                if (stopRequested_ && acquisitionState == 0) {
                    finishSession();
                }
                return;
            }
        }

        if (publishedSequence >= nextSequence_ + kAcquisitionBlockCount) {
            failSession(QStringLiteral("电流采集读取落后，Buffer数据已被覆盖。"));
            return;
        }

        while (nextSequence_ <= publishedSequence) {
            if (!consumeSequence(nextSequence_)) {
                return;
            }
            ++nextSequence_;
        }

        if (stopRequested_) {
            if (stopTimer_.isValid()
                && stopTimer_.elapsed() > kStopTimeoutMilliseconds) {
                failSession(QStringLiteral("停止采集后等待Buffer排空超时。"));
                return;
            }
            if (acquisitionState == 0 && nextSequence_ > publishedSequence) {
                finishSession();
            }
        }
    }

private:
    bool ensureControllerConnected(QString* errorMessage)
    {
        if (controllerHandle_ != ACSC_INVALID) {
            acquisitionTrace(QStringLiteral("复用已建立的采集专用ACS连接"));
            return true;
        }

        if (connectionSettings_.useEthernetTcp) {
            acquisitionTrace(QStringLiteral("开始建立采集专用EthernetTCP连接：%1:%2")
                                 .arg(connectionSettings_.address)
                                 .arg(connectionSettings_.port));
            QByteArray addressBytes = connectionSettings_.address.toLatin1();
            controllerHandle_ = acsc_OpenCommEthernetTCP(addressBytes.data(),
                                                         connectionSettings_.port);
        } else {
            acquisitionTrace(QStringLiteral("开始建立采集专用Simulator连接"));
            controllerHandle_ = acsc_OpenCommSimulator();
        }

        if (controllerHandle_ == ACSC_INVALID) {
            setError(errorMessage,
                     QStringLiteral("无法建立采集专用ACS连接：%1").arg(acsErrorText()));
            return false;
        }
        acquisitionTrace(QStringLiteral("采集专用ACS连接建立成功"));
        return true;
    }

    void closeController()
    {
        if (controllerHandle_ != ACSC_INVALID) {
            acquisitionTrace(QStringLiteral("关闭采集专用ACS连接"));
            acsc_CloseComm(controllerHandle_);
            controllerHandle_ = ACSC_INVALID;
        }
    }

    static void setError(QString* errorMessage, const QString& message)
    {
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
    }

    bool readIntegerScalar(const char* variableName, int* value) const
    {
        return acsc_ReadInteger(controllerHandle_, ACSC_NONE,
                                const_cast<char*>(variableName),
                                0, 0, 0, 0, value, nullptr) != 0;
    }

    bool readRealScalar(const char* variableName, double* value) const
    {
        return acsc_ReadReal(controllerHandle_, ACSC_NONE,
                            const_cast<char*>(variableName),
                            0, 0, 0, 0, value, nullptr) != 0;
    }

    bool readIntegerArray(const char* variableName,
                          int firstIndex,
                          int lastIndex,
                          int* values) const
    {
        return acsc_ReadInteger(controllerHandle_, ACSC_NONE,
                                const_cast<char*>(variableName),
                                firstIndex, lastIndex, 0, 0, values, nullptr) != 0;
    }

    bool readCurrentChannel(const QByteArray& blockVariable,
                            int channel,
                            int sampleCount,
                            QVector<double>* values) const
    {
        values->resize(sampleCount);
        return acsc_ReadReal(controllerHandle_, ACSC_NONE,
                            const_cast<char*>(blockVariable.constData()),
                            channel, channel, 0, sampleCount - 1,
                            values->data(), nullptr) != 0;
    }

    bool writeAcquisitionRun(int value) const
    {
        return acsc_WriteInteger(controllerHandle_, ACSC_NONE,
                                 const_cast<char*>("ACQ_RUN"),
                                 0, 0, 0, 0, &value, nullptr) != 0;
    }

    bool consumeSequence(int sequence)
    {
        int blockSequences[kAcquisitionBlockCount] = {};
        int validCounts[kAcquisitionBlockCount] = {};
        if (!readIntegerArray("ACQ_BLOCK_SEQ", 0, kAcquisitionBlockCount - 1,
                              blockSequences)
            || !readIntegerArray("ACQ_BLOCK_VALID_COUNT", 0,
                                 kAcquisitionBlockCount - 1, validCounts)) {
            failSession(QStringLiteral("读取采集块索引失败：%1").arg(acsErrorText()));
            return false;
        }

        int blockIndex = -1;
        for (int index = 0; index < kAcquisitionBlockCount; ++index) {
            if (blockSequences[index] == sequence) {
                blockIndex = index;
                break;
            }
        }
        if (blockIndex < 0) {
            failSession(QStringLiteral("找不到电流采集序号 %1 对应的数据块。")
                            .arg(sequence));
            return false;
        }

        const int sampleCount = validCounts[blockIndex];
        if (sampleCount <= 0 || sampleCount > kMaximumBlockSampleCount) {
            failSession(QStringLiteral("电流采集块有效数量异常：%1。")
                            .arg(sampleCount));
            return false;
        }
        acquisitionTrace(QStringLiteral("读取电流数据块：sequence=%1，block=%2，samples=%3")
                             .arg(sequence)
                             .arg(blockIndex + 1)
                             .arg(sampleCount));

        const QByteArray blockVariable = QByteArray("ACQ_DATA_")
                                           + QByteArray::number(blockIndex + 1);
        QVector<double> currentX1;
        QVector<double> currentX2;
        if (!readCurrentChannel(blockVariable, 0, sampleCount, &currentX1)
            || !readCurrentChannel(blockVariable, 1, sampleCount, &currentX2)) {
            failSession(QStringLiteral("读取电流采集块 %1 失败：%2")
                            .arg(blockIndex + 1)
                            .arg(acsErrorText()));
            return false;
        }

        int sequenceAfterRead = 0;
        if (!readIntegerArray("ACQ_BLOCK_SEQ", blockIndex, blockIndex,
                              &sequenceAfterRead)
            || sequenceAfterRead != sequence) {
            failSession(QStringLiteral("读取过程中电流采集块 %1 已被覆盖。")
                            .arg(blockIndex + 1));
            return false;
        }

        const qint64 blockReturnedAtMs = QDateTime::currentMSecsSinceEpoch();
        acquisitionTrace(QStringLiteral("电流块读取完成：sequence=%1，returnedAt=%2")
                             .arg(sequence)
                             .arg(QDateTime::fromMSecsSinceEpoch(blockReturnedAtMs)
                                      .toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"))));

        QVector<CurrentSample> samples;
        samples.reserve(sampleCount);
        for (int index = 0; index < sampleCount; ++index) {
            CurrentSample sample;
            sample.timeStampMs = blockReturnedAtMs - (sampleCount - 1 - index);
            sample.currentX1 = currentX1.at(index);
            sample.currentX2 = currentX2.at(index);
            samples.append(sample);
        }
        currentSampleCount_ += samples.size();
        ExcelSessionWriter* writer = writer_;
        QMetaObject::invokeMethod(writer, [writer, samples]() {
            writer->appendCurrentSamples(samples);
        }, Qt::QueuedConnection);
        return true;
    }

    void finishSession()
    {
        pollTimer_->stop();
        active_ = false;

        acquisitionTrace(QStringLiteral("Buffer排空完成，提交最后分页：电流=%1，温度=%2")
                             .arg(currentSampleCount_)
                             .arg(temperatureSampleCount_));

        QStringList savedFiles;
        QString errorMessage;
        if (!finalizeWriterSession(&savedFiles, &errorMessage)) {
            session_ = AcquisitionSessionData();
            emit sessionFailed(errorMessage);
            return;
        }

        session_ = AcquisitionSessionData();
        acquisitionTrace(QStringLiteral("采集会话保存完成"));
        emit sessionStopped(savedFiles);
    }

    void failSession(const QString& message)
    {
        qWarning().noquote() << "[ACQ_TRACE] 采集会话失败：" << message;
        pollTimer_->stop();
        const bool hadActiveSession = active_;
        if (hadActiveSession) {
            writeAcquisitionRun(0);
        }
        active_ = false;

        QString reportedMessage = message;
        if (hadActiveSession) {
            QStringList savedFiles;
            QString saveError;
            if (finalizeWriterSession(&savedFiles, &saveError)) {
                if (!savedFiles.isEmpty()) {
                    reportedMessage += QStringLiteral("；已保存已接收数据：%1")
                                           .arg(savedFiles.join(QStringLiteral("；")));
                }
            } else {
                abortWriterSession();
                if (!saveError.isEmpty()) {
                    reportedMessage += QStringLiteral("；同时保存失败：%1")
                                           .arg(saveError);
                }
            }
        } else {
            abortWriterSession();
        }
        session_ = AcquisitionSessionData();
        emit sessionFailed(reportedMessage);
    }

    bool finalizeWriterSession(QStringList* savedFiles, QString* errorMessage)
    {
        bool saved = false;
        QMetaObject::invokeMethod(writer_, [this, &saved, savedFiles, errorMessage]() {
            saved = writer_->finishSession(savedFiles, errorMessage);
        }, Qt::BlockingQueuedConnection);
        return saved;
    }

    void abortWriterSession()
    {
        ExcelSessionWriter* writer = writer_;
        QMetaObject::invokeMethod(writer, [writer]() {
            writer->abortSession();
        }, Qt::QueuedConnection);
    }

    AcquisitionConnectionSettings connectionSettings_;
    HANDLE controllerHandle_;
    QTimer* pollTimer_;
    ExcelSessionWriter* writer_;
    bool active_;
    bool stopRequested_;
    bool controllerStarted_;
    int nextSequence_;
    int lastLoggedAcquisitionState_;
    int lastLoggedPublishedSequence_;
    qint64 currentSampleCount_;
    qint64 temperatureSampleCount_;
    QElapsedTimer stopTimer_;
    AcquisitionSessionData session_;
};

DataAcquisitionService::DataAcquisitionService(QObject* parent)
    : QObject(parent),
      workerThread_(nullptr),
      worker_(nullptr),
      writerThread_(nullptr),
      writer_(nullptr),
      initialized_(false),
      collecting_(false),
      acceptingTemperature_(false)
{
}

DataAcquisitionService::~DataAcquisitionService()
{
    shutdown();
}

void DataAcquisitionService::initialize(
    const AcquisitionConnectionSettings& connectionSettings)
{
    if (initialized_) {
        return;
    }

    writerThread_ = new QThread(this);
    writer_ = new ExcelSessionWriter();
    writer_->moveToThread(writerThread_);

    workerThread_ = new QThread(this);
    worker_ = new AcquisitionWorker(connectionSettings, writer_);
    worker_->moveToThread(workerThread_);

    connect(worker_, &AcquisitionWorker::sessionStarted,
            this, &DataAcquisitionService::collectionStarted);
    connect(worker_, &AcquisitionWorker::sessionStopped,
            this, [this](const QStringList& savedFiles) {
                collecting_ = false;
                acceptingTemperature_ = false;
                emit collectionStopped(savedFiles);
            });
    connect(worker_, &AcquisitionWorker::sessionFailed,
            this, [this](const QString& message) {
                collecting_ = false;
                acceptingTemperature_ = false;
                emit collectionFailed(message);
            });
    connect(writer_, &ExcelSessionWriter::writeFailed,
            worker_, [this](const QString& message) {
                worker_->handleWriterFailure(message);
            });
    connect(workerThread_, &QThread::finished, worker_, &QObject::deleteLater);
    connect(writerThread_, &QThread::finished, writer_, &QObject::deleteLater);

    writerThread_->start();
    workerThread_->start();
    initialized_ = true;
    acquisitionTrace(QStringLiteral("采集服务初始化完成：mode=%1，address=%2，port=%3")
                         .arg(connectionSettings.useEthernetTcp
                                  ? QStringLiteral("ethernet_tcp")
                                  : QStringLiteral("simulator"))
                         .arg(connectionSettings.address)
                         .arg(connectionSettings.port));
}

void DataAcquisitionService::shutdown()
{
    if (!initialized_) {
        return;
    }

    acceptingTemperature_ = false;
    QMetaObject::invokeMethod(worker_, [this]() {
        worker_->shutdownNow();
    }, Qt::BlockingQueuedConnection);

    QMetaObject::invokeMethod(writer_, [this]() {
        writer_->abortSession();
    }, Qt::BlockingQueuedConnection);
    workerThread_->quit();
    workerThread_->wait();
    writerThread_->quit();
    writerThread_->wait();

    worker_ = nullptr;
    workerThread_ = nullptr;
    writer_ = nullptr;
    writerThread_ = nullptr;
    initialized_ = false;
    collecting_ = false;
}

bool DataAcquisitionService::startCollection(
    const QString& outputDirectory,
    const AcquisitionConditions& conditions,
    QString* errorMessage)
{
    acquisitionTrace(QStringLiteral("Service开始启动采集：velocity=%1，acceleration=%2，waitMs=%3，directory=%4")
                         .arg(conditions.velocity)
                         .arg(conditions.acceleration)
                         .arg(conditions.waitTimeMilliseconds)
                         .arg(outputDirectory));
    if (!initialized_) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("采集服务尚未初始化。");
        }
        return false;
    }
    if (collecting_) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("采集记录正在运行或保存中。");
        }
        return false;
    }

    bool started = false;
    QString workerError;
    QMetaObject::invokeMethod(worker_, [this, &started, &workerError,
                                       outputDirectory, conditions]() {
        started = worker_->startSession(outputDirectory, conditions, &workerError);
    }, Qt::BlockingQueuedConnection);

    if (!started) {
        acquisitionTrace(QStringLiteral("Service启动采集失败：%1").arg(workerError));
        if (errorMessage != nullptr) {
            *errorMessage = workerError;
        }
        return false;
    }

    collecting_ = true;
    acceptingTemperature_ = true;
    acquisitionTrace(QStringLiteral("Service启动采集成功"));
    return true;
}

void DataAcquisitionService::appendTemperatureSamples(
    const QVector<TemperatureSample>& samples)
{
    if (!initialized_ || !acceptingTemperature_ || samples.isEmpty()) {
        return;
    }

    QMetaObject::invokeMethod(worker_, [this, samples]() {
        worker_->appendTemperatureSamples(samples);
    }, Qt::QueuedConnection);
}

void DataAcquisitionService::stopCollection()
{
    if (!initialized_ || !collecting_) {
        acquisitionTrace(QStringLiteral("Service忽略停止采集：initialized=%1，collecting=%2")
                             .arg(initialized_)
                             .arg(collecting_));
        return;
    }

    acceptingTemperature_ = false;
    acquisitionTrace(QStringLiteral("Service已投递停止采集请求"));
    QMetaObject::invokeMethod(worker_, [this]() {
        worker_->stopSession();
    }, Qt::QueuedConnection);
}

bool DataAcquisitionService::isInitialized() const
{
    return initialized_;
}

bool DataAcquisitionService::isCollecting() const
{
    return collecting_;
}

#include "DataAcquisitionService.moc"
