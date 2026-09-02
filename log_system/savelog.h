#ifndef SAVELOG_H
#define SAVELOG_H

#include <QObject>

class QFile;
class QTcpSocket;
class QTcpServer;

//消息类型
enum MsgType {
    //调试信息，通常用于开发过程中，提供程序运行的详细状态信息。qDebug
    MsgType_Debug = 0x0001,
    //普通信息，用于通知用户或开发者程序运行的一般状态。 qInfo
    MsgType_Info = 0x0002,
    //警告信息，表示程序遇到了一些问题，但这些问题不会阻止程序的正常运行。qWarning()
    MsgType_Warning = 0x0004,
    //严重错误信息，表示程序遇到了一些关键性的问题，这些问题可能会影响程序的部分功能。qCritical() 系统消息也属于这个
    MsgType_Critical = 0x0008,
    //致命错误信息，表示程序遇到了无法恢复的问题，需要立即终止运行。qFatal()
    MsgType_Fatal = 0x0010,
};

///主要内容是捕获所有输出然后重新分配,会默认发个信号,可以用其它东西捕获
/// 可选是否发送到网络，是否保存到txt，以及输出到终端
#ifdef quc
class Q_DECL_EXPORT SaveLog : public QObject
        #else
class SaveLog : public QObject
        #endif

{
    Q_OBJECT
public:
    static SaveLog *Instance();
    explicit SaveLog(QObject *parent = 0);
    ~SaveLog();

    //上下文类型
    MsgType contentType;

    QList<QString> debugList;
    QList<QString> infoList;
    QList<QString> warningList;
    QList<QString> criticalList;
    QList<QString> fatalList;
    QList<QString> allTypeList;
    QList<QString> chooseTypeList;

private:
    static QScopedPointer<SaveLog> self;

    //是否在运行
    bool isRun = false;

    //文件最大行数 0表示不启用
    int maxRow = 0;
    int currentRow = 0;
    int currentRowDebug = 0;
    int currentRowInfo = 0;
    int currentRowWarning = 0;
    int currentRowCritical = 0;
    int currentRowFatal = 0;
    //文件最大大小 0表示不启用 单位kb
    int maxSize = 12800;
    //储存行数
    int saveNum = 100;
    //是否重定向到网络
    bool toNet = false;
    //是否输出到日志
    bool toTxT = true;
    //是否原样输出到终端
    bool toTerminal = true;
    //是否分不同文件输出
    bool isSaveDifferent = false;
    //是否输出日志上下文
    bool useContext = false;
    //是否显示前坠(消息类型）
    bool usePrefix = true;
    //是否使用修改的前缀
    bool useChangePrefix = false;
    //文件对象
    QFile *file;
    QFile *fileDebug;
    QFile *fileInfo;
    QFile *fileWarning;
    QFile *fileCritical;
    QFile *fileFatal;
    //日志文件路径
    QString Path;
    QString pathDebug;
    QString pathInfo;
    QString pathWarning;
    QString pathCritical;
    QString pathFatal;
    //日志文件名称
    QString name;
    //日志文件完整名称
    QString fileName = "";
    QString fileNameDebug = "";
    QString fileNameInfo = "";
    QString fileNameWarning = "";
    QString fileNameCritical = "";
    QString fileNameFatal = "";

    //捕获的消息类型,默认只输出有用消息,不输出qt本身报错
    MsgType msgType = MsgType(MsgType_Info | MsgType_Warning | MsgType_Fatal);
    //MsgType(MsgType_Debug | MsgType_Info | MsgType_Warning | MsgType_Critical | MsgType_Fatal);
private:
    void openFile(const QString &fileName);
    void openFile(const QString &fileName,MsgType type);

public:
    bool getUseContext();
    bool getToTerminal();
    bool getUsePreFix();
    bool getUseChangePrefix();
    int getSaveNum();
    MsgType getMsgType();

Q_SIGNALS:
    //发送内容信号到网络
    void send(const QString &content);
    //发送内容到其它地方
    void sendMsg(const QString &content);
    void sendMsgWithType(const QString &content , MsgType type);

public Q_SLOTS:
    //启动日志服务
    void start(const QString &path = "");
    //暂停日志服务
    void stop();

    //清空状态
    void clear();
    //保存日志
    void save(const QString &content);
    //设置日志文件最大行数
    void setMaxRow(int maxrow);
    //设置日志文件最大大小 单位kb
    void setMaxSize(int maxsize);
    //设置保存在类中行数
    void setSaveNum(int num);

    //设置监听端口
    void setListenPort(int listenport);
    //设置是否重定向到网络
    void setToNet(bool tonet);
    //设置是否保存TXT
    void setToTxT(bool totxt);
    //设置是否原样输出到终端（主要是调试使用）
    void setToTerminal(bool toterminal);
    //是否分不同文件输出
    void setIsSaveDifferent(bool issavedifferent);
    //设置是否输出日志上下文
    void setUseContext(bool usecontext);
    //是否显示前缀(消息类型）
    void setUsePrefix(bool useprefix);
    //是否使用修改的前缀
    void setUseChangePrefix(bool usechangeprefix);

    //设置日志文件存放路径
    void setPath(const QString &path);
    //设置日志文件名称
    void setName(const QString &nam);
    //设置捕获的消息类型
    void setMsgType(const MsgType &msgtype);
};

#ifdef quc
class Q_DECL_EXPORT SendLog : public QObject
        #else
class SendLog : public QObject
        #endif

{
    Q_OBJECT
public:
    static SendLog *Instance();
    explicit SendLog(QObject *parent = 0);
    ~SendLog();

private:
    static QScopedPointer<SendLog> self;

    //监听端口
    int listenPort;
    //网络通信对象
    QTcpSocket *socket;
    //网络监听服务器
    QTcpServer *server;

private slots:
    //新连接到来
    void newConnection();

public Q_SLOTS:
    //设置监听端口
    void setListenPort(int listenPort);

    //启动和停止服务
    void start();
    void stop();

    //发送日志
    void send(const QString &content);
};

#endif // SAVELOG_H
