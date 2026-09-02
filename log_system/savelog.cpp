//#pragma execution_character_set("utf-8")

#include "savelog.h"
#include "qmutex.h"
#include "qdir.h"
#include "qfile.h"
#include "qtcpsocket.h"
#include "qtcpserver.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qtextstream.h"
#include "qstringlist.h"

#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define QDATETIMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
void Log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#else
void Log(QtMsgType type, const char *msg)
#endif
{
    //加锁,防止多线程中qdebug太频繁导致崩溃
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QString content;

    //根据不同类型设置不同的类型
    int msgType = SaveLog::Instance()->getMsgType();
    //是否捕获
    bool catchmessage = false;
    switch (type) {
    case QtDebugMsg:
        if ((msgType & MsgType_Debug) == MsgType_Debug){
            catchmessage = true;
            SaveLog::Instance()->contentType = MsgType_Debug;
            if(SaveLog::Instance()->getUsePreFix())
            {
                if(SaveLog::Instance()->getUseChangePrefix())
                   content = QString("[信息] - %1").arg(msg);
                else
                    content = QString("[Debug] - %1").arg(msg);
            }
        }
        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5,5,0))
    case QtInfoMsg:
        if ((msgType & MsgType_Info) == MsgType_Info) {
            catchmessage = true;
            SaveLog::Instance()->contentType = MsgType_Info;
            if(SaveLog::Instance()->getUsePreFix())
            {
                if(SaveLog::Instance()->getUseChangePrefix())
                    content = QString("[操作] - %1").arg(msg);
                else
                    content = QString("[Info] - %1").arg(msg);
            }
        }
        break;
#endif
    case QtWarningMsg:
        if ((msgType & MsgType_Warning) == MsgType_Warning) {
            catchmessage = true;
            SaveLog::Instance()->contentType = MsgType_Warning;
            if(SaveLog::Instance()->getUsePreFix())
            {
                if(SaveLog::Instance()->getUseChangePrefix())
                    content = QString("[警告] - %1").arg(msg);
                else
                    content = QString("[Warning] - %1").arg(msg);
            }
        }
        break;
    case QtCriticalMsg:
        if ((msgType & MsgType_Critical) == MsgType_Critical) {
            catchmessage = true;
            SaveLog::Instance()->contentType = MsgType_Critical;
            if(SaveLog::Instance()->getUsePreFix())
            {
                if(SaveLog::Instance()->getUseChangePrefix())
                    content = QString("[报错] - %1").arg(msg);
                else
                    content = QString("[Critical] - %1").arg(msg);
            }
        }
        break;
    case QtFatalMsg:
        if ((msgType & MsgType_Fatal) == MsgType_Fatal) {
            catchmessage = true;
            SaveLog::Instance()->contentType = MsgType_Fatal;
            if(SaveLog::Instance()->getUsePreFix())
            {
                if(SaveLog::Instance()->getUseChangePrefix())
                    content = QString("[错误] - %1").arg(msg);
                else
                    content = QString("[Fatal] - %1").arg(msg);
            }
        }
        break;
    }



    //加上打印代码所在代码文件、行号、函数名
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    if (SaveLog::Instance()->getUseContext()) {
        int line = context.line;
        QString file = context.file;
        QString function = context.function;
        if (line > 0)
            content = QString("行号: %1  文件: %2  函数: %3\n%4").arg(line).arg(file).arg(function).arg(content);
    }
#endif

    //原样输出到终端
    if(SaveLog::Instance()->getToTerminal())
    {
        fprintf(stdout, "%s\n", content.toLocal8Bit().constData());
        fflush(stdout);
    }

    //储存一下
    int savenum = SaveLog::Instance()->getSaveNum();
    if(SaveLog::Instance()->allTypeList.count() >= savenum)
        SaveLog::Instance()->allTypeList.removeFirst();
    SaveLog::Instance()->allTypeList.append(msg);
    //不捕获则返回
    if (catchmessage) {
        if(SaveLog::Instance()->chooseTypeList.count() >= savenum)
            SaveLog::Instance()->chooseTypeList.removeFirst();
        SaveLog::Instance()->chooseTypeList.append(msg);
    }else
        return;

    //将内容传给函数进行处理
    SaveLog::Instance()->save(content);
}

QScopedPointer<SaveLog> SaveLog::self;
SaveLog *SaveLog::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull())
            self.reset(new SaveLog);
    }

    return self.data();
}

SaveLog::SaveLog(QObject *parent) : QObject(parent)
{
    //必须用信号槽形式,不然提示 QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread
    //估计日志钩子可能单独开了线程
    connect(this, SIGNAL(send(QString)), SendLog::Instance(), SLOT(send(QString)));

    //全局的文件对象,在需要的时候打开而不是每次添加日志都打开
    file = new QFile(this);
    fileDebug = new QFile(this);
    fileInfo = new QFile(this);
    fileWarning = new QFile(this);
    fileCritical = new QFile(this);
    fileFatal = new QFile(this);
    //默认取应用程序根目录
    //Path = qApp->applicationDirPath();
    setPath("../Log");
    //默认取应用程序可执行文件名称
    QString str = qApp->applicationFilePath();
    QStringList list = str.split("/");
    name = list.at(list.count() - 1).split(".").at(0);
}

SaveLog::~SaveLog()
{
    file->close();
}

void SaveLog::openFile(const QString &fileName)
{
    //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
    if (this->fileName != fileName) {
        this->fileName = fileName;
        //先关闭之前的
        if (file->isOpen())
            file->close();
        //重新设置新的日志文件
        file->setFileName(fileName);
        //以 Append 追加的形式打开
        file->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    }
}

void SaveLog::openFile(const QString &fileName,MsgType type)
{
    switch (type) {
    case MsgType_Debug:
        //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
        if (this->fileNameDebug != fileName) {
            this->fileNameDebug = fileName;
            //先关闭之前的
            if (fileDebug->isOpen())
                fileDebug->close();
            //重新设置新的日志文件
            fileDebug->setFileName(fileName);
            //以 Append 追加的形式打开
            fileDebug->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        }
        break;
    case MsgType_Info:
        //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
        if (this->fileNameInfo != fileName) {
            this->fileNameInfo = fileName;
            //先关闭之前的
            if (fileInfo->isOpen())
                fileInfo->close();
            //重新设置新的日志文件
            fileInfo->setFileName(fileName);
            //以 Append 追加的形式打开
            fileInfo->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        }

        break;
    case MsgType_Warning:
        //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
        if (this->fileNameWarning != fileName) {
            this->fileNameWarning = fileName;
            //先关闭之前的
            if (fileWarning->isOpen())
                fileWarning->close();
            //重新设置新的日志文件
            fileWarning->setFileName(fileName);
            //以 Append 追加的形式打开
            fileWarning->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        }
        break;
    case MsgType_Critical:
        //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
        if (this->fileNameCritical != fileName) {
            this->fileNameCritical = fileName;
            //先关闭之前的
            if (fileCritical->isOpen())
                fileCritical->close();
            //重新设置新的日志文件
            fileCritical->setFileName(fileName);
            //以 Append 追加的形式打开
            fileCritical->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        }
        break;
    case MsgType_Fatal:
        //当文件名改变时才新建和打开文件而不是每次都打开文件(效率极低)或者一开始打开文件
        if (this->fileNameFatal != fileName) {
            this->fileNameFatal = fileName;
            //先关闭之前的
            if (fileFatal->isOpen())
                fileFatal->close();
            //重新设置新的日志文件
            fileFatal->setFileName(fileName);
            //以 Append 追加的形式打开
            fileFatal->open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        }
        break;
    }
}

bool SaveLog::getUseContext()
{
    return useContext;
}

bool SaveLog::getToTerminal()
{
    return toTerminal;
}

bool SaveLog::getUsePreFix()
{
    return usePrefix;
}

bool SaveLog::getUseChangePrefix()
{
    return useChangePrefix;
}

int SaveLog::getSaveNum()
{
    return saveNum;
}

MsgType SaveLog::getMsgType()
{
    return msgType;
}

//安装日志钩子,注册自定义日志处理函数，输出调试信息到文件,便于调试
void SaveLog::start(const QString &path)
{
    if (isRun) {
        return;
    }

    isRun = true;
    if(path != "")
        setPath(path);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qInstallMessageHandler(Log);
#else
    qInstallMsgHandler(Log);
#endif
}

//卸载日志钩子
void SaveLog::stop()
{
    if (!isRun) {
        return;
    }

    isRun = false;
    this->clear();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    qInstallMessageHandler(0);
#else
    qInstallMsgHandler(0);
#endif
}

void SaveLog::clear()
{
    currentRow = 0;
    currentRowDebug = 0;
    currentRowInfo = 0;
    currentRowWarning = 0;
    currentRowCritical = 0;
    currentRowFatal = 0;
    fileName.clear();
    fileNameDebug.clear();
    fileNameInfo.clear();
    fileNameWarning.clear();
    fileNameCritical.clear();
    fileNameFatal.clear();
    if (file->isOpen()) {
        file->close();
    }
    if (fileDebug->isOpen())
        fileDebug->close();
    if (fileInfo->isOpen())
        fileInfo->close();
    if (fileWarning->isOpen())
        fileWarning->close();
    if (fileCritical->isOpen())
        fileCritical->close();
    if (fileFatal->isOpen())
        fileFatal->close();
}

void SaveLog::save(const QString &content)
{
    //加日期
    QString date=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString msg = QString("%1 %2").arg(date).arg(content);
    //发送信号，如果外部想使用
    emit sendMsg(msg);
    emit sendMsgWithType(msg,contentType);
    //如果重定向输出到网络则通过网络发出去
    if (toNet) {
        emit send(msg);
    }
    //输出到日志文件
    if (toTxT) {
        //创建目录
        QDir dir(Path);
        if (!dir.exists())
            dir.mkpath(Path);
        //保存并且判断分目录
        switch (contentType) {
        case MsgType_Debug:
            if(debugList.count() >= saveNum)
                debugList.removeFirst();
            debugList.append(msg);
            if(isSaveDifferent)
            {
                QDir dir(pathDebug);
                if (!dir.exists())
                    dir.mkpath(pathDebug);
            }
            break;
        case MsgType_Info:
            if(infoList.count() >= saveNum)
                infoList.removeFirst();
            infoList.append(msg);
            if(isSaveDifferent)
            {
                QDir dir(pathInfo);
                if (!dir.exists())
                    dir.mkpath(pathInfo);
            }
            break;
        case MsgType_Warning:
            if(warningList.count() >= saveNum)
                warningList.removeFirst();
            warningList.append(msg);
            if(isSaveDifferent)
            {
                QDir dir(pathWarning);
                if (!dir.exists())
                    dir.mkpath(pathWarning);
            }
            break;
        case MsgType_Critical:
            if(criticalList.count() >= saveNum)
                criticalList.removeFirst();
            criticalList.append(msg);
            if(isSaveDifferent)
            {
                QDir dir(pathCritical);
                if (!dir.exists())
                    dir.mkpath(pathCritical);
            }
            break;
        case MsgType_Fatal:
            if(fatalList.count() >= saveNum)
                fatalList.removeFirst();
            fatalList.append(msg);
            if(isSaveDifferent)
            {
                QDir dir(pathFatal);
                if (!dir.exists())
                    dir.mkpath(pathFatal);
            }
            break;
        }

        //日志存储规则有多种策略 优先级 行数>大小>日期
        //1: 设置了最大行数限制则按照行数限制来
        //2: 设置了大小则按照大小来控制日志文件
        //3: 都没有设置都存储到日期命名的文件,只有当日期变化了才会切换到新的日志文件
        //目前如果设置了最大行数和大小则需要重新打开，最好别设置这些
        bool needOpen = false;
        if(!isSaveDifferent)
        {
            if (maxRow > 0)
            {
                currentRow++;
                if (fileName.isEmpty())
                    needOpen = true;
                else if (currentRow >= maxRow)
                    needOpen = true;
            }
            else if (maxSize > 0) {
                //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
                //首次需要重新打开文件以及超过大小需要重新打开文件
                if (fileName.isEmpty()) {
                    needOpen = true;
                } else if (file->size() > (maxSize * 1024)) {
                    needOpen = true;
                }
            } else {
                //日期改变了才会触发
                QString fileName = QString("%1/%2_log_%3.txt").arg(Path).arg(name).arg(QDATE);
                openFile(fileName);
            }

            if ((maxRow > 0 || maxSize > 0) && needOpen) {
                currentRow = 0;
                QString fileName = QString("%1/%2_log_%3.txt").arg(Path).arg(name).arg(QDATETIMS);
                openFile(fileName);
            }
            //用文本流的输出速度更快
            QTextStream stream(file);
            stream << msg << "\n";
        }
        else
        {
            switch (contentType) {
            case MsgType_Debug:
            {
                if (maxRow > 0) {
                    currentRowDebug++;
                    if (fileNameDebug.isEmpty()) {
                        needOpen = true;
                    } else if (currentRowDebug >= maxRow) {
                        needOpen = true;
                    }
                } else if (maxSize > 0) {
                    //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
                    //首次需要重新打开文件以及超过大小需要重新打开文件
                    if (fileNameDebug.isEmpty()) {
                        needOpen = true;
                    } else if (fileDebug->size() > (maxSize * 1024)) {
                        needOpen = true;
                    }
                } else {
                    //日期改变了才会触发
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathDebug).arg(name).arg(QDATE);
                    openFile(fileName, MsgType_Debug);
                }
                if ((maxRow > 0 || maxSize > 0) && needOpen) {
                    currentRowDebug = 0;
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathDebug).arg(name).arg(QDATETIMS);
                    openFile(fileName, MsgType_Debug);
                }
                //用文本流的输出速度更快
                QTextStream stream(fileDebug);
                stream << msg << "\n";
            }
            break;
            case MsgType_Info:
            {
                if (maxRow > 0) {
                    currentRowInfo++;
                    if (fileNameInfo.isEmpty()) {
                        needOpen = true;
                    } else if (currentRowInfo >= maxRow) {
                        needOpen = true;
                    }
                } else if (maxSize > 0) {
                    //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
                    //首次需要重新打开文件以及超过大小需要重新打开文件
                    if (fileNameInfo.isEmpty()) {
                        needOpen = true;
                    } else if (fileInfo->size() > (maxSize * 1024)) {
                        needOpen = true;
                    }
                } else {
                    //日期改变了才会触发
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathInfo).arg(name).arg(QDATE);
                    openFile(fileName, MsgType_Info);
                }

                if ((maxRow > 0 || maxSize > 0) && needOpen) {
                    currentRowInfo = 0;
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathInfo).arg(name).arg(QDATETIMS);
                    openFile(fileName, MsgType_Info);
                }
                //用文本流的输出速度更快
                QTextStream stream(fileInfo);
                stream << msg << "\n";
            }
            break;
            case MsgType_Warning:
            {
                if (maxRow > 0) {
                    currentRowWarning++;
                    if (fileNameWarning.isEmpty()) {
                        needOpen = true;
                    } else if (currentRowWarning >= maxRow) {
                        needOpen = true;
                    }
                } else if (maxSize > 0) {
                    //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
                    //首次需要重新打开文件以及超过大小需要重新打开文件
                    if (fileNameWarning.isEmpty()) {
                        needOpen = true;
                    } else if (fileWarning->size() > (maxSize * 1024)) {
                        needOpen = true;
                    }
                } else {
                    //日期改变了才会触发
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathWarning).arg(name).arg(QDATE);
                    openFile(fileName,MsgType_Warning);
                }

                if ((maxRow > 0 || maxSize > 0) && needOpen) {
                    currentRowWarning = 0;
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathWarning).arg(name).arg(QDATETIMS);
                    openFile(fileName,MsgType_Warning);
                }
                //用文本流的输出速度更快
                QTextStream stream(fileWarning);
                stream << msg << "\n";
            }
            break;
            case MsgType_Critical:
            {
                if (maxRow > 0) {
                    currentRowCritical++;
                    if (fileNameCritical.isEmpty()) {
                        needOpen = true;
                    } else if (currentRowCritical >= maxRow) {
                        needOpen = true;
                    }
                } else if (maxSize > 0) {
                    //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
                    //首次需要重新打开文件以及超过大小需要重新打开文件
                    if (fileNameCritical.isEmpty()) {
                        needOpen = true;
                    } else if (fileCritical->size() > (maxSize * 1024)) {
                        needOpen = true;
                    }
                } else {
                    //日期改变了才会触发
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathCritical).arg(name).arg(QDATE);
                    openFile(fileName, MsgType_Critical);
                }

                if ((maxRow > 0 || maxSize > 0) && needOpen) {
                    currentRowCritical= 0;
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathCritical).arg(name).arg(QDATETIMS);
                    openFile(fileName, MsgType_Critical);
                }
                //用文本流的输出速度更快
                QTextStream stream(fileCritical);
                stream << msg << "\n";
            }
            break;
            case MsgType_Fatal:
            {
                if (maxRow > 0) {
                    currentRowFatal++;
                    if (fileNameFatal.isEmpty()) {
                        needOpen = true;
                    } else if (currentRowFatal >= maxRow) {
                        needOpen = true;
                    }
                } else if (maxSize > 0) {
                    //1MB=1024*1024 经过大量测试 QFile().size() 方法速度非常快
                    //首次需要重新打开文件以及超过大小需要重新打开文件
                    if (fileNameFatal.isEmpty()) {
                        needOpen = true;
                    } else if (fileFatal->size() > (maxSize * 1024)) {
                        needOpen = true;
                    }
                } else {
                    //日期改变了才会触发
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathFatal).arg(name).arg(QDATE);
                    openFile(fileName, MsgType_Fatal);
                }

                if ((maxRow > 0 || maxSize > 0) && needOpen) {
                    currentRowFatal = 0;
                    QString fileName = QString("%1/%2_log_%3.txt").arg(pathFatal).arg(name).arg(QDATETIMS);
                    openFile(fileName, MsgType_Fatal);
                }
                //用文本流的输出速度更快
                QTextStream stream(fileFatal);
                stream << msg << "\n";
            }
            break;
            }
        }
    }
}

void SaveLog::setMaxRow(int maxrow)
{
    //这里可以限定最大最小值
    if (maxrow >= 0) {
        maxRow = maxrow;
        clear();
    }
}

void SaveLog::setMaxSize(int maxsize)
{
    //这里可以限定最大最小值
    if (maxsize >= 0) {
        maxSize = maxsize;
        clear();
    }
}

void SaveLog::setSaveNum(int num)
{
    if (num >= 0)
    {
        saveNum = num;
        clear();
    }
}

void SaveLog::setListenPort(int listenport)
{
    SendLog::Instance()->setListenPort(listenport);
}

void SaveLog::setToNet(bool tonet)
{
    toNet = tonet;
    if (tonet) {
        SendLog::Instance()->start();
    } else {
        SendLog::Instance()->stop();
    }
}

void SaveLog::setToTxT(bool totxt)
{
    toTxT = totxt;
}

void SaveLog::setToTerminal(bool toterminal)
{
    toTerminal = toterminal;
}

void SaveLog::setIsSaveDifferent(bool issavedifferent)
{
    isSaveDifferent = issavedifferent;
}

void SaveLog::setUseContext(bool usecontext)
{
    useContext = usecontext;
}

void SaveLog::setUsePrefix(bool useprefix)
{
    usePrefix = useprefix;
}

void SaveLog::setUseChangePrefix(bool usechangeprefix)
{
    useChangePrefix =usechangeprefix;
}

void SaveLog::setPath(const QString &path)
{
    QString absolutePath = QDir(path).absolutePath();
    Path = absolutePath;
    pathDebug=absolutePath + "/Debug";
    pathInfo=absolutePath + "/Info";
    pathWarning=absolutePath + "/Warning";
    pathCritical=absolutePath + "/Critical";
    pathWarning=absolutePath + "/Fatal";
}

void SaveLog::setName(const QString &nam)
{
    name = nam;
}

void SaveLog::setMsgType(const MsgType &msgtype)
{
    msgType = msgtype;
}

//网络发送日志数据类
QScopedPointer<SendLog> SendLog::self;
SendLog *SendLog::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new SendLog);
        }
    }

    return self.data();
}

SendLog::SendLog(QObject *parent) : QObject(parent)
{
    listenPort = 6000;
    socket = NULL;

    //实例化网络通信服务器对象
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

SendLog::~SendLog()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
    }

    server->close();
}

void SendLog::newConnection()
{
    //限定就一个连接
    while (server->hasPendingConnections()) {
        socket = server->nextPendingConnection();
    }
}

void SendLog::setListenPort(int listenPort)
{
    this->listenPort = listenPort;
}

void SendLog::start()
{
    //启动端口监听
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    server->listen(QHostAddress::AnyIPv4, listenPort);
#else
    server->listen(QHostAddress::Any, listenPort);
#endif
}

void SendLog::stop()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
        socket = NULL;
    }

    server->close();
}

void SendLog::send(const QString &content)
{
    if (socket != NULL && socket->isOpen()) {
        socket->write(content.toUtf8());
        //socket->flush();
    }
}
