#pragma once

#include "AcquisitionTypes.h"

#include <QObject>
#include <QStringList>

class QTimer;

class ExcelSessionWriter final : public QObject
{
    Q_OBJECT

public:
    explicit ExcelSessionWriter(QObject* parent = nullptr);

    bool startSession(const AcquisitionSessionData& session,
                      QString* errorMessage);
    void appendCurrentSamples(const QVector<CurrentSample>& samples);
    void appendTemperatureSamples(const QVector<TemperatureSample>& samples);
    bool finishSession(QStringList* savedFiles, QString* errorMessage);
    void abortSession();

signals:
    void writeFailed(const QString& message);

private slots:
    void saveCheckpoint();

private:
    bool saveCurrentPage(bool force);
    bool saveTemperaturePage(bool force);
    QString pageFilePath(const QString& pageTimestamp,
                         const QString& dataType,
                         int pageNumber) const;
    void failSession(const QString& message);
    void resetSession();

    QTimer* saveTimer_;
    AcquisitionSessionData session_;
    QString currentPageTimestamp_;
    QString temperaturePageTimestamp_;
    QStringList savedFiles_;
    int currentPageNumber_;
    int temperaturePageNumber_;
    bool active_;
    QString lastError_;
};
