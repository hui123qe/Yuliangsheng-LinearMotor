#include "ExcelSessionWriter.h"

#include "xlsxdocument.h"
#include "xlsxformat.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>

namespace {

const int kSamplesPerSheet = 100000;
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

bool selectPage(QXlsx::Document* document,
                int pageIndex,
                QString* errorMessage)
{
    const QString sheetName = QStringLiteral("第%1页").arg(pageIndex + 1);
    bool sheetPrepared = false;
    if (pageIndex == 0) {
        const QStringList existingSheets = document->sheetNames();
        sheetPrepared = existingSheets.isEmpty()
            ? document->addSheet(sheetName)
            : document->renameSheet(existingSheets.first(), sheetName);
    } else {
        sheetPrepared = document->addSheet(sheetName);
    }

    if (!sheetPrepared || !document->selectSheet(sheetName)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("无法创建或选择Excel工作表：%1").arg(sheetName);
        }
        qWarning().noquote() << "[ACQ_TRACE][Excel] 工作表准备失败" << sheetName;
        return false;
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 已选择工作表" << sheetName;
    return true;
}

bool saveCurrentWorkbook(const AcquisitionSessionData& session,
                         const QString& filePath,
                         QString* errorMessage)
{
    QXlsx::Document document;
    QXlsx::Format headerFormat;
    headerFormat.setFontBold(true);
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    const int sampleCount = session.currentSamples.size();
    const int pageCount = qMax(1, (sampleCount + kSamplesPerSheet - 1)
                                  / kSamplesPerSheet);
    qInfo().noquote() << "[ACQ_TRACE][Excel] 开始生成电流文件"
                      << "samples=" << sampleCount
                      << "pages=" << pageCount
                      << "path=" << filePath;
    for (int page = 0; page < pageCount; ++page) {
        if (!selectPage(&document, page, errorMessage)) {
            return false;
        }
        writeConditionHeader(&document, session.conditions);
        document.write(kDataHeaderRow, 1, QStringLiteral("时间"), headerFormat);
        document.write(kDataHeaderRow, 2, QStringLiteral("CurrentX1"), headerFormat);
        document.write(kDataHeaderRow, 3, QStringLiteral("CurrentX2"), headerFormat);
        document.setColumnWidth(1, 1, 24.0);
        document.setColumnWidth(2, 3, 16.0);

        const int firstSample = page * kSamplesPerSheet;
        const int lastSample = qMin(firstSample + kSamplesPerSheet, sampleCount);
        for (int index = firstSample; index < lastSample; ++index) {
            const int row = kDataHeaderRow + 1 + index - firstSample;
            const CurrentSample& sample = session.currentSamples.at(index);
            document.write(row, 1, formatTimestamp(sample.timeStampMs));
            document.write(row, 2, sample.currentX1);
            document.write(row, 3, sample.currentX2);
        }
    }

    if (!document.saveAs(filePath)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("电流 Excel 保存失败：%1").arg(filePath);
        }
        return false;
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 电流文件保存成功" << filePath;
    return true;
}

bool saveTemperatureWorkbook(const AcquisitionSessionData& session,
                             const QString& filePath,
                             QString* errorMessage)
{
    QXlsx::Document document;
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

    const int sampleCount = session.temperatureSamples.size();
    const int pageCount = qMax(1, (sampleCount + kSamplesPerSheet - 1)
                                  / kSamplesPerSheet);
    qInfo().noquote() << "[ACQ_TRACE][Excel] 开始生成温度文件"
                      << "samples=" << sampleCount
                      << "pages=" << pageCount
                      << "path=" << filePath;
    for (int page = 0; page < pageCount; ++page) {
        if (!selectPage(&document, page, errorMessage)) {
            return false;
        }
        writeConditionHeader(&document, session.conditions);
        for (int column = 0; column < headers.size(); ++column) {
            document.write(kDataHeaderRow, column + 1, headers.at(column), headerFormat);
        }
        document.setColumnWidth(1, 1, 24.0);
        document.setColumnWidth(2, headers.size(), 16.0);

        const int firstSample = page * kSamplesPerSheet;
        const int lastSample = qMin(firstSample + kSamplesPerSheet, sampleCount);
        for (int index = firstSample; index < lastSample; ++index) {
            const int row = kDataHeaderRow + 1 + index - firstSample;
            const TemperatureSample& sample = session.temperatureSamples.at(index);
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
    }

    if (!document.saveAs(filePath)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("温度 Excel 保存失败：%1").arg(filePath);
        }
        return false;
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 温度文件保存成功" << filePath;
    return true;
}

} // namespace

bool ExcelSessionWriter::save(const AcquisitionSessionData& session,
                              QStringList* savedFiles,
                              QString* errorMessage)
{
    qInfo().noquote() << "[ACQ_TRACE][Excel] 保存会话开始"
                      << "currentSamples=" << session.currentSamples.size()
                      << "temperatureSamples=" << session.temperatureSamples.size()
                      << "directory=" << session.outputDirectory;
    QDir outputDirectory(session.outputDirectory);
    if (!outputDirectory.exists() && !outputDirectory.mkpath(QStringLiteral("."))) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("无法创建采集数据目录：%1")
                                .arg(session.outputDirectory);
        }
        return false;
    }

    const QString timestamp = session.startedAt.toString(QStringLiteral("yyyyMMdd_HHmmss_zzz"));
    const QString currentPath = outputDirectory.filePath(
        timestamp + QStringLiteral("_电流.xlsx"));
    const QString temperaturePath = outputDirectory.filePath(
        timestamp + QStringLiteral("_温度.xlsx"));

    if (!saveCurrentWorkbook(session, currentPath, errorMessage)) {
        return false;
    }
    if (!saveTemperatureWorkbook(session, temperaturePath, errorMessage)) {
        return false;
    }

    if (savedFiles != nullptr) {
        savedFiles->clear();
        savedFiles->append(currentPath);
        savedFiles->append(temperaturePath);
    }
    qInfo().noquote() << "[ACQ_TRACE][Excel] 保存会话完成";
    return true;
}
