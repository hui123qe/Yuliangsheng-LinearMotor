#include "ExcelSessionWriter.h"

#include "xlsxdocument.h"
#include "xlsxformat.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTimer>

namespace {

const int kSamplesPerFile = 50000;
const int kSaveIntervalMilliseconds = 20000;
const int kDataHeaderRow = 7;

QString formatTimestamp(qint64 timeStampMs)
{
    return QDateTime::fromMSecsSinceEpoch(timeStampMs)
        .toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
}

void writeConditionHeader(QXlsx::Document* document,
                          const AcquisitionConditions& conditions)
{
    QXlsx::Format labelFormat;
    labelFormat.setFontBold(true);

    document->write(1, 1, QStringLiteral("设定速度"), labelFormat);
    document->write(1, 2, conditions.velocity);
    document->write(2, 1, QStringLiteral("设定加速度"), labelFormat);
    document->write(2, 2, conditions.acceleration);
    document->write(3, 1, QStringLiteral("等待时间(ms)"), labelFormat);
    document->write(3, 2, conditions.waitTimeMilliseconds);
}

bool prepareDataSheet(QXlsx::Document* document, QString* errorMessage)
{
    const QString sheetName = QStringLiteral("第1页");
    const QStringList existingSheets = document->sheetNames();
    const bool sheetPrepared = existingSheets.isEmpty()
        ? document->addSheet(sheetName)
        : document->renameSheet(existingSheets.first(), sheetName);

    if (!sheetPrepared || !document->selectSheet(sheetName)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("无法创建或选择Excel工作表：%1").arg(sheetName);
        }
        return false;
    }
    return true;
}

bool saveCurrentWorkbook(const QVector<CurrentSample>& samples,
                         const AcquisitionConditions& conditions,
                         const QString& filePath,
                         QString* errorMessage)
{
    QXlsx::Document document;
    if (!prepareDataSheet(&document, errorMessage)) {
        return false;
    }

    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    writeConditionHeader(&document, conditions);
    document.write(kDataHeaderRow, 1, QStringLiteral("时间"), headerFormat);
    document.write(kDataHeaderRow, 2, QStringLiteral("CurrentX1"), headerFormat);
    document.write(kDataHeaderRow, 3, QStringLiteral("CurrentX2"), headerFormat);
    document.setColumnWidth(1, 1, 24.0);
    document.setColumnWidth(2, 3, 16.0);

    for (int index = 0; index < samples.size(); ++index) {
        const int row = kDataHeaderRow + 1 + index;
        const CurrentSample& sample = samples.at(index);
        document.write(row, 1, formatTimestamp(sample.timeStampMs));
        document.write(row, 2, sample.currentX1);
        document.write(row, 3, sample.currentX2);
    }

    if (!document.saveAs(filePath)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("电流 Excel 保存失败：%1").arg(filePath);
        }
        return false;
    }
    return true;
}

bool saveTemperatureWorkbook(const QVector<TemperatureSample>& samples,
                             const AcquisitionConditions& conditions,
                             const QString& filePath,
                             QString* errorMessage)
{
    QXlsx::Document document;
    if (!prepareDataSheet(&document, errorMessage)) {
        return false;
    }

    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    const QStringList headers = {
        QStringLiteral("时间"),
        QStringLiteral("电机1温度1"), QStringLiteral("电机1温度2"),
        QStringLiteral("电机1温度3"), QStringLiteral("电机1温度4"),
        QStringLiteral("电机2温度1"), QStringLiteral("电机2温度2"),
        QStringLiteral("电机2温度3"), QStringLiteral("电机2温度4")
    };

    writeConditionHeader(&document, conditions);
    for (int column = 0; column < headers.size(); ++column) {
        document.write(kDataHeaderRow, column + 1, headers.at(column), headerFormat);
    }
    document.setColumnWidth(1, 1, 24.0);
    document.setColumnWidth(2, headers.size(), 16.0);

    for (int index = 0; index < samples.size(); ++index) {
        const int row = kDataHeaderRow + 1 + index;
        const TemperatureSample& sample = samples.at(index);
        document.write(row, 1, formatTimestamp(sample.timeStampMs));
        document.write(row, 2, sample.motor1Channel1);
        document.write(row, 3, sample.motor1Channel2);
        document.write(row, 4, sample.motor1Channel3);
        document.write(row, 5, sample.motor1Channel4);
        document.write(row, 6, sample.motor2Channel1);
        document.write(row, 7, sample.motor2Channel2);
        document.write(row, 8, sample.motor2Channel3);
        document.write(row, 9, sample.motor2Channel4);
    }

    if (!document.saveAs(filePath)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("温度 Excel 保存失败：%1").arg(filePath);
        }
        return false;
    }
    return true;
}

} // namespace

ExcelSessionWriter::ExcelSessionWriter(QObject* parent)
    : QObject(parent),
      saveTimer_(new QTimer(this)),
      currentPageNumber_(1),
      temperaturePageNumber_(1),
      active_(false)
{
    saveTimer_->setInterval(kSaveIntervalMilliseconds);
    connect(saveTimer_, &QTimer::timeout,
            this, &ExcelSessionWriter::saveCheckpoint);
}

bool ExcelSessionWriter::startSession(const AcquisitionSessionData& session,
                                      QString* errorMessage)
{
    resetSession();

    QDir outputDirectory(session.outputDirectory);
    if (!outputDirectory.exists() && !outputDirectory.mkpath(QStringLiteral("."))) {
        const QString message = QStringLiteral("无法创建采集数据目录：%1")
                                    .arg(session.outputDirectory);
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
        return false;
    }

    session_ = session;
    session_.currentSamples.clear();
    session_.temperatureSamples.clear();

    active_ = true;
    saveTimer_->start();
    qInfo().noquote() << "[ACQ_TRACE][Excel] 会话写入已启动"
                      << "intervalMs=" << kSaveIntervalMilliseconds
                      << "samplesPerFile=" << kSamplesPerFile;
    return true;
}

void ExcelSessionWriter::appendCurrentSamples(const QVector<CurrentSample>& samples)
{
    if (!active_ || samples.isEmpty()) {
        return;
    }

    int sourceIndex = 0;
    while (sourceIndex < samples.size() && active_) {
        const int available = kSamplesPerFile - session_.currentSamples.size();
        const int appendCount = qMin(available, samples.size() - sourceIndex);
        session_.currentSamples.reserve(session_.currentSamples.size() + appendCount);
        for (int index = 0; index < appendCount; ++index) {
            session_.currentSamples.append(samples.at(sourceIndex + index));
        }
        sourceIndex += appendCount;

        if (session_.currentSamples.size() == kSamplesPerFile) {
            if (!saveCurrentPage(true)) {
                return;
            }
            session_.currentSamples.clear();
            ++currentPageNumber_;
            currentPageTimestamp_.clear();
        }
    }
}

void ExcelSessionWriter::appendTemperatureSamples(
    const QVector<TemperatureSample>& samples)
{
    if (!active_ || samples.isEmpty()) {
        return;
    }

    int sourceIndex = 0;
    while (sourceIndex < samples.size() && active_) {
        const int available = kSamplesPerFile - session_.temperatureSamples.size();
        const int appendCount = qMin(available, samples.size() - sourceIndex);
        session_.temperatureSamples.reserve(
            session_.temperatureSamples.size() + appendCount);
        for (int index = 0; index < appendCount; ++index) {
            session_.temperatureSamples.append(samples.at(sourceIndex + index));
        }
        sourceIndex += appendCount;

        if (session_.temperatureSamples.size() == kSamplesPerFile) {
            if (!saveTemperaturePage(true)) {
                return;
            }
            session_.temperatureSamples.clear();
            ++temperaturePageNumber_;
            temperaturePageTimestamp_.clear();
        }
    }
}

bool ExcelSessionWriter::finishSession(QStringList* savedFiles,
                                       QString* errorMessage)
{
    saveTimer_->stop();
    if (!active_) {
        if (errorMessage != nullptr) {
            *errorMessage = lastError_.isEmpty()
                ? QStringLiteral("Excel 写入会话未启动。")
                : lastError_;
        }
        return false;
    }

    const bool forceEmptyCurrentFile = currentPageNumber_ == 1
        && session_.currentSamples.isEmpty();
    const bool forceEmptyTemperatureFile = temperaturePageNumber_ == 1
        && session_.temperatureSamples.isEmpty();
    if (!saveCurrentPage(forceEmptyCurrentFile)
        || !saveTemperaturePage(forceEmptyTemperatureFile)) {
        if (errorMessage != nullptr) {
            *errorMessage = lastError_;
        }
        return false;
    }

    if (savedFiles != nullptr) {
        *savedFiles = savedFiles_;
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 会话写入完成"
                      << "files=" << savedFiles_.size();
    resetSession();
    return true;
}

void ExcelSessionWriter::abortSession()
{
    resetSession();
}

void ExcelSessionWriter::saveCheckpoint()
{
    if (!active_) {
        return;
    }

    qInfo().noquote() << "[ACQ_TRACE][Excel] 执行20秒定时保存"
                      << "currentSamples=" << session_.currentSamples.size()
                      << "temperatureSamples=" << session_.temperatureSamples.size();
    saveCurrentPage(false);
    if (active_) {
        saveTemperaturePage(false);
    }
}

bool ExcelSessionWriter::saveCurrentPage(bool force)
{
    if (!force && session_.currentSamples.isEmpty()) {
        return true;
    }

    if (currentPageTimestamp_.isEmpty()) {
        currentPageTimestamp_ = QDateTime::currentDateTime()
                                    .toString(QStringLiteral("yyyyMMdd_HHmmss"));
    }
    const QString filePath = pageFilePath(currentPageTimestamp_,
                                          QStringLiteral("电流"),
                                          currentPageNumber_);
    if (!savedFiles_.contains(filePath) && QFileInfo::exists(filePath)) {
        failSession(QStringLiteral("电流分页文件已存在，无法覆盖：%1").arg(filePath));
        return false;
    }
    QString errorMessage;
    if (!saveCurrentWorkbook(session_.currentSamples, session_.conditions,
                             filePath, &errorMessage)) {
        failSession(errorMessage);
        return false;
    }
    if (!savedFiles_.contains(filePath)) {
        savedFiles_.append(filePath);
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 电流分页保存成功"
                      << "samples=" << session_.currentSamples.size()
                      << "path=" << filePath;
    return true;
}

bool ExcelSessionWriter::saveTemperaturePage(bool force)
{
    if (!force && session_.temperatureSamples.isEmpty()) {
        return true;
    }

    if (temperaturePageTimestamp_.isEmpty()) {
        temperaturePageTimestamp_ = QDateTime::currentDateTime()
                                        .toString(QStringLiteral("yyyyMMdd_HHmmss"));
    }
    const QString filePath = pageFilePath(temperaturePageTimestamp_,
                                          QStringLiteral("温度"),
                                          temperaturePageNumber_);
    if (!savedFiles_.contains(filePath) && QFileInfo::exists(filePath)) {
        failSession(QStringLiteral("温度分页文件已存在，无法覆盖：%1").arg(filePath));
        return false;
    }
    QString errorMessage;
    if (!saveTemperatureWorkbook(session_.temperatureSamples, session_.conditions,
                                 filePath, &errorMessage)) {
        failSession(errorMessage);
        return false;
    }
    if (!savedFiles_.contains(filePath)) {
        savedFiles_.append(filePath);
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 温度分页保存成功"
                      << "samples=" << session_.temperatureSamples.size()
                      << "path=" << filePath;
    return true;
}

QString ExcelSessionWriter::pageFilePath(const QString& pageTimestamp,
                                         const QString& dataType,
                                         int pageNumber) const
{
    const QString fileName = pageTimestamp
        + QStringLiteral("_") + dataType
        + QStringLiteral("--%1.xlsx").arg(pageNumber, 4, 10, QLatin1Char('0'));
    return QDir(session_.outputDirectory).filePath(fileName);
}

void ExcelSessionWriter::failSession(const QString& message)
{
    if (!active_) {
        return;
    }

    saveTimer_->stop();
    active_ = false;
    lastError_ = message;
    qWarning().noquote() << "[ACQ_TRACE][Excel] 写入失败" << message;
    emit writeFailed(message);
}

void ExcelSessionWriter::resetSession()
{
    saveTimer_->stop();
    session_ = AcquisitionSessionData();
    currentPageTimestamp_.clear();
    temperaturePageTimestamp_.clear();
    savedFiles_.clear();
    currentPageNumber_ = 1;
    temperaturePageNumber_ = 1;
    active_ = false;
    lastError_.clear();
}
