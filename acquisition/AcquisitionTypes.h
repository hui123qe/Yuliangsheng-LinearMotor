#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

struct AcquisitionConnectionSettings
{
    bool useEthernetTcp{false};
    QString address;
    int port{701};
};

struct AcquisitionConditions
{
    double velocity;
    double acceleration;
    int waitTimeMilliseconds;
    double currentSamplePeriodSeconds;
    double temperatureSamplePeriodSeconds;

    AcquisitionConditions()
        : velocity(0.0),
          acceleration(0.0),
          waitTimeMilliseconds(0),
          currentSamplePeriodSeconds(0.001),
          temperatureSamplePeriodSeconds(0.0)
    {
    }
};

struct CurrentSample
{
    qint64 timeStampMs{0};
    double currentX1;
    double currentX2;
};

struct TemperatureSample
{
    qint64 timeStampMs{0};
    double motor1Channel1;
    double motor1Channel2;
    double motor1Channel3;
    double motor1Channel4;
    double motor2Channel1;
    double motor2Channel2;
    double motor2Channel3;
    double motor2Channel4;
};

struct AcquisitionSessionData
{
    QDateTime startedAt;
    QString outputDirectory;
    AcquisitionConditions conditions;
    QVector<CurrentSample> currentSamples;
    QVector<TemperatureSample> temperatureSamples;
};
