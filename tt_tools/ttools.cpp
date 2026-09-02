#include "ttools.h"
#include <QDateTime>
int* loginstate = nullptr;
// 获取毫秒级时间戳的函数
qint64 ttools::return_tttime()
{
    // 获取当前时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 获取毫秒级时间戳
    qint64 timestamp = currentDateTime.toMSecsSinceEpoch();
    return timestamp;
}

bool ttools::returntrue()
{
    return false;
}
bool ttools::returnfalse()
{
    return true;
}


int ttools::returnint(int a, int b)
{
    // 确定最小和最大值
    int min = (a < b) ? a : b;
    int max = (a < b) ? b : a;

    // 生成区间内的随机整数（包含边界）
    return rand() % (max - min + 1) + min;
}

double ttools::returndouble(int a, int b)
{
    // 确定最小和最大值
    int min = (a < b) ? a : b;
    int max = (a < b) ? b : a;

    // 生成[0,1]范围内的随机浮点数
    double random = static_cast<double>(rand()) / RAND_MAX;

    // 扩展并偏移到[min, max]范围
    double value = min + random * (max - min);

    // 保留两位小数（四舍五入）
    return round(value * 100) / 100.0;
}

QString ttools::return_realtime()
{
    QDateTime now = QDateTime::currentDateTime();

    // 格式化时间字符串
    QString timeString = now.toString("yyyy.MM.dd.HH.mm.ss");

    return timeString;
}


QString createcsv()
{




    QString currentDir2 = QCoreApplication::applicationDirPath();
    QString filePath = currentDir2 + "/savepath.txt";
    QFile file(filePath);

    QString dataPath, imagePath;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line;
            } else if (line.startsWith("imagepath=")) {
                imagePath = line;
            }
        }
        file.close();
        if (!dataPath.isEmpty()) {
            qDebug() << "Data path:" << dataPath;
        }
        if (!imagePath.isEmpty()) {
            qDebug() << "Image path:" << imagePath;
        }
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy.MM.dd.HH.mm.ss");

    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);

    QDir dataDir(datapath2);
    QString dataFolderPath = dataDir.absoluteFilePath("data");
    qDebug() << "dataFolderPath=" << dataFolderPath;

    if (!QDir(dataFolderPath).exists()) {
        if (!dataDir.mkdir("data")) {
            qWarning() << "Failed to create directory:" << dataFolderPath;
        } else {
            qDebug() << "Directory created:" << dataFolderPath;
        }
    } else {
        qDebug() << "Directory already exists:" << dataFolderPath;
    }

    QString dataFolderPath2 = dataFolderPath+"/"+gettestnumber();


    //累计计数文件
    if(1){
        QString dirPath666 = dataFolderPath2;
        QString fileName666 = "test_num.txt";  // 明确指定 .txt 后缀
        QString filePath666 = QDir(dirPath666).filePath(fileName666);

        QFile file666(filePath666);

        if (file666.exists()) {
            // 文件存在，打印完整路径
            qDebug() << "文件已存在，路径：" << filePath666;
        } else {
            // 文件不存在，创建并写入默认内容
            if (file666.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file666);
                out << "testtime=0\n";  // 第一行
                out << "testnum=0\n";   // 第二行
                file666.close();
                qDebug() << "文件已创建，路径：" << filePath666;
            } else {
                qDebug() << "无法创建文件：" << filePath666;
            }
        }
    }



    QDir dataDir2(dataFolderPath);
    if (!QDir(dataFolderPath2).exists()) {
        if (!dataDir2.mkdir(gettestnumber())) {
            qWarning() << "Failed to create directory2:" << dataFolderPath2;
        } else {
            qDebug() << "Directory created:2" << dataFolderPath2;
        }
    } else {
        qDebug() << "Directory already exists2:" << dataFolderPath2;
    }
    qDebug()<<"新文件路径"<<dataFolderPath2;
    QString csvFilePath = QDir(dataFolderPath2).filePath(timestamp + ".csv");
    qDebug() << "CSV file path:" << csvFilePath;

    QFile csvFile(csvFilePath);
    if (!csvFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create CSV file:" << csvFilePath;
        return QString();
    }
    csvFile.close();
    qDebug() << "CSV file created:" << csvFilePath;

//    QList<QString> data = { "测试时间", "运行时间", "位置1", "位置2", "速度1", "速度2",
//                            "加速度1", "加速度2", "电流1", "电流2", "电压1", "电压2",
//                            "温度1", "温度2", "电阻1", "电阻2", "流量1", "流量2", "进口水压1", "进口水压2",
//                            "出口水压1", "出口水压2", "气压", "气流" };

//    QList<QString> data2 = { "     ms", "     ms", "     mm", "     mm", "     mm/s", "     mm/s",
//                             "     mm/s^2", "     mm/s^2", "     mA", "     mA", "     V", "     V",
//                             "     ℃", "     ℃","     Ω", "     Ω", "     L/min", "     L/min", "     MPa", "     MPa",
//                             "     MPa", "     MPa", "     MPa", "     L/min" };
    QList<QString> data = { "测试时间", "位置1", "位置2", "速度1", "速度2",
                            "加速度1", "加速度2", "电流1", "电流2",
                            "温度1", "温度2", "电阻1", "电阻2", "水流量1", "水流量2", "进口水压1", "进口水压2",
                            "出口水压1", "出口水压2", "气压", "气流" };

    QList<QString> data2 = { "     ms", "     mm", "     mm", "     mm/s", "     mm/s",
                             "     mm/s^2", "     mm/s^2", "      A", "      A",
                             "     ℃", "     ℃","     Ω", "     Ω", "     L/min", "     L/min", "     MPa", "     MPa",
                             "     MPa", "     MPa", "     MPa", "     L/min" };
    appendRowsToCsv(csvFilePath, data);
    appendRowsToCsv(csvFilePath, data2);

    return csvFilePath;
}




QString createxunhuancsv()
{
    QString currentDir2 = QCoreApplication::applicationDirPath();
    QString filePath = currentDir2 + "/savepath.txt";
    QFile file(filePath);

    QString dataPath, imagePath;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line;
            } else if (line.startsWith("imagepath=")) {
                imagePath = line;
            }
        }
        file.close();
        if (!dataPath.isEmpty()) {
            qDebug() << "Data path:" << dataPath;
        }
        if (!imagePath.isEmpty()) {
            qDebug() << "Image path:" << imagePath;
        }
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy.MM.dd.HH.mm.ss");

    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);

    QDir dataDir(datapath2);
    QString dataFolderPath = dataDir.absoluteFilePath("data");
    qDebug() << "dataFolderPath=" << dataFolderPath;

    if (!QDir(dataFolderPath).exists()) {
        if (!dataDir.mkdir("data")) {
            qWarning() << "Failed to create directory:" << dataFolderPath;
        } else {
            qDebug() << "Directory created:" << dataFolderPath;
        }
    } else {
        qDebug() << "Directory already exists:" << dataFolderPath;
    }
    QString dataFolderPath2 = dataFolderPath+"/"+gettestnumber();
    QDir dataDir2(dataFolderPath);
    if (!QDir(dataFolderPath2).exists()) {
        if (!dataDir2.mkdir(gettestnumber())) {
            qWarning() << "Failed to create directory2:" << dataFolderPath2;
        } else {
            qDebug() << "Directory created:2" << dataFolderPath2;
        }
    } else {
        qDebug() << "Directory already exists2:" << dataFolderPath2;
    }
    QString csvFileName = timestamp + "Cyclic_Motion.csv";
    QString csvFilePath = QDir(dataFolderPath2).filePath(csvFileName);
    qDebug() << "CSV file path:" << csvFilePath;

    QFile csvFile(csvFilePath);
    if (!csvFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create CSV file:" << csvFilePath;
        return QString();
    }
    csvFile.close();
    qDebug() << "CSV file created:" << csvFilePath;

    QList<QString> data = { "测试时间","温度1max", "温度2max", "电流1max", "电流2max"};
    QList<QString> data2 = { "     ms","     ℃", "     ℃", "     mA", "     mA"};

    appendRowsToCsv(csvFilePath, data);
    appendRowsToCsv(csvFilePath, data2);

    return csvFilePath;
}
QString createLog()
{/*
    // 获取应用程序的当前目录
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/savepath.txt";
    QFile file(filePath);

    QString dataPath, imagePath;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        // 读取文件内容，解析 datapath 和 imagepath
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line;
            } else if (line.startsWith("imagepath=")) {
                imagePath = line;
            }
        }
        file.close();

        // 打印 datapath 和 imagepath
        if (!dataPath.isEmpty()) {
            qDebug() << "Data path:" << dataPath;
        }
        if (!imagePath.isEmpty()) {
            qDebug() << "Image path:" << imagePath;
        }
    }

    // 获取当前时间并格式化为字符串
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy.MM.dd.HH.mm.ss");

    // 提取 datapath 的实际路径（去掉前缀）
    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);

    // 创建 QDir 对象
    QDir dataDir(datapath2);

    // 构建名为 "logs" 的文件夹路径
    QString logsFolderPath = dataDir.absoluteFilePath("logs");
    qDebug() << "Logs folder path:" << logsFolderPath;

    // 检查 "logs" 文件夹是否存在，如果不存在则创建
    if (!QDir(logsFolderPath).exists()) {
        if (!dataDir.mkdir("logs")) {
            qWarning() << "Failed to create directory:" << logsFolderPath;
        } else {
            qDebug() << "Directory created:" << logsFolderPath;
        }
    } else {
        qDebug() << "Directory already exists:" << logsFolderPath;
    }

    // 构建日志文件的完整路径
    QString logFilePath = QDir(logsFolderPath).absoluteFilePath(timestamp + ".log.txt");
    qDebug() << "Log file path:" << logFilePath;

    // 创建日志文件（这里只是创建空文件，不写入数据）
    QFile logFile(logFilePath);
    if (!logFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create log file:" << logFilePath;
        return QString(); // 返回空字符串表示失败
    }
    logFile.close(); // 文件已成功创建，关闭它
    qDebug() << "Log file created:" << logFilePath;

    // 返回日志文件的路径
    return logFilePath;*/
    return "123";
}


bool appendRowsToCsv(const QString &csvFilePath, const QList<QString> &data)
{
    // 打开文件用于追加写入
    QFile file(csvFilePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        //qWarning() << "Failed to open CSV file for appending:" << csvFilePath;
        return false;
    }

    // 创建一个文本流来写入文件
    QTextStream out(&file);
    out.setCodec("GBK");
    // 构建行字符串，字段之间用逗号分隔
    QString rowString;
    for (int i = 0; i < data.size(); ++i) {
        rowString.append(data[i]);
        if (i < data.size() - 1) {
            rowString.append(","); // 不是最后一个字段时添加逗号
        }
    }

    // 将行字符串写入文件，并添加换行符
    out << rowString << "\n";

    // 文件会在QTextStream析构时自动关闭，但显式关闭也是一个好习惯（可选）
    file.close();

    // 写入成功
    return true;
}
bool appendRowsToCsv(const QString &csvFilePath, const QVector<QList<QString>> &dataVector)
{
    // 打开文件用于追加写入
    QFile file(csvFilePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open CSV file for appending:" << csvFilePath;
        return false;
    }

    // 创建一个文本流来写入文件
    QTextStream out(&file);
    out.setCodec("GBK"); // 设置编码

    // 遍历所有 QList<QString> 并构建 CSV 行
    for (const QList<QString> &data : dataVector) {
        // 构建行字符串，字段之间用逗号分隔
        QString rowString;
        for (int i = 0; i < data.size(); ++i) {
            rowString.append(data[i]);
            if (i < data.size() - 1) {
                rowString.append(","); // 不是最后一个字段时添加逗号
            }
        }
        // 写入行字符串，并添加换行符
        out << rowString << "\n";
    }

    // 文件会在 QTextStream 析构时自动关闭，但显式关闭也是一个好习惯
    file.close();

    // 写入成功
    return true;
}
void saveLogToD(const QString& message) {
    // 文件路径
    const QString filePath = "D:/log.txt";

    // 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Failed to open log file:" << filePath;
        return;
    }

    // 写入日志
    QTextStream out(&file);
    out << message << "\n";

    // 关闭文件
    file.close();
}


QString gettestnumber()
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/testnumber.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for reading:" << filePath;
        return "wrong";
    }
    QTextStream in(&file);
    QString imagepathLine;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testnumber=")) {
            imagepathLine = line;
        }
    }
    file.close();
    QString imagepathLine2=imagepathLine.mid(imagepathLine.indexOf('=') + 1);
    return imagepathLine2;
}
QString getlogfile()
{
    QString currentDir2 = QCoreApplication::applicationDirPath();
    QString filePath = currentDir2 + "/savepath.txt";
    QFile file(filePath);

    QString dataPath, imagePath;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line;
            } else if (line.startsWith("imagepath=")) {
                imagePath = line;
            }
        }
        file.close();
        if (!dataPath.isEmpty()) {
            qDebug() << "Data path:" << dataPath;
        }
        if (!imagePath.isEmpty()) {
            qDebug() << "Image path:" << imagePath;
        }
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy.MM.dd.HH.mm.ss");

    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);

    QDir dataDir(datapath2);
    QString dataFolderPath = dataDir.absoluteFilePath("log");
    qDebug() << "dataFolderPath=" << dataFolderPath;

    if (!QDir(dataFolderPath).exists()) {
        if (!dataDir.mkdir("log")) {
            qWarning() << "Failed to create directory:" << dataFolderPath;
        } else {
            qDebug() << "Directory created:" << dataFolderPath;
        }
    } else {
        qDebug() << "Directory already exists:" << dataFolderPath;
    }
    return dataFolderPath;
}
