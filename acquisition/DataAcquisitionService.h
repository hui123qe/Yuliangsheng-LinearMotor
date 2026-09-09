#pragma once

#include "AcquisitionTypes.h"

#include <QObject>

class AcquisitionWorker;
class QThread;

class DataAcquisitionService final : public QObject
{
    Q_OBJECT

public:
    explicit DataAcquisitionService(QObject* parent = nullptr);
    ~DataAcquisitionService() override;

    void initialize(const AcquisitionConnectionSettings& connectionSettings);
    void shutdown();

    bool startCollection(const QString& outputDirectory,
                         const AcquisitionConditions& conditions,
                         QString* errorMessage = nullptr);
    void appendTemperatureSamples(const QVector<TemperatureSample>& samples);
    void stopCollection();

    bool isInitialized() const;
    bool isCollecting() const;

signals:
    void collectionStarted();
    void collectionStopped(const QStringList& savedFiles);
    void collectionFailed(const QString& message);

private:
    QThread* workerThread_;
    AcquisitionWorker* worker_;
    bool initialized_;
    bool collecting_;
    bool acceptingTemperature_;
};
