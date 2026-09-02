#pragma once
#ifndef TTOOLS_H
#define TTOOLS_H

#endif // TTOOLS_H

#include <random>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <cstdlib> // 用于rand()和RAND_MAX
#include <cmath>   // 用于round函数


class ttools
{
public:
    qint64 return_tttime();
    bool returntrue();
    bool returnfalse();
    int returnint(int a,int b);
    double returndouble(int a,int b);
    QString return_realtime();

};

extern int *loginstate;

QString createcsv();
QString createLog();
QString createxunhuancsv();
bool appendRowsToCsv(const QString &csvFilePath, const QList<QString> &data);
bool appendRowsToCsv(const QString &csvFilePath, const QVector<QList<QString>> &dataVector);
void saveLogToD(const QString& message);
QString gettestnumber();
QString getlogfile();
