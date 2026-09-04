#include "widget.h"
#include "ui_widget.h"
//#include "tt_tem.hpp"

#include "widget.h"
#include "ui_widget.h"
#include "tt_tem.hpp"
#include <QDebug>


QString SampleFrame::toCsvLine() const
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(timeStampMs);
    QString timeStr = QString("%1.%2.%3.%4")
            .arg(dt.toString("yyyyMMdd"))
            .arg(dt.toString("HH"))
            .arg(dt.toString("mm"))
            .arg(dt.toString("ss.zzz"));
    return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21\n")
            .arg(timeStr)
            .arg(pos1)        //位置1
            .arg(pos2)        //位置2
            .arg(vel1)        //速度1
            .arg(vel2)        //速度2
            .arg(acc1)        //加速度1
            .arg(acc2)        //加速度2
            .arg(current1)    //电流1
            .arg(current2)    //电流2
            .arg(tem1)        //温度1
            .arg(tem2)        //温度2
            .arg(temptc1)     //电阻1
            .arg(temptc2)     //电阻2
            .arg(Flowrate1)   //水流量1
            .arg(Flowrate2)   //水流量2
            .arg(inwater1)    //进口水压1
            .arg(inwater2)    //进口水压2
            .arg(outwater1)   //出口水压1
            .arg(outwater2)   //出口水压2
            .arg(qiya)        //气压
            .arg(qiliu);      //气流
}

AcquireThread::AcquireThread(QObject *parent)
    : QThread(parent), m_ringBuf(2000)
{

}

AcquireThread::~AcquireThread()
{
    requestInterruption();
    wait();
}

void AcquireThread::setHandle(HANDLE h)
{
    m_hComm = h;
}

void AcquireThread::setDevicePtr(ITCPDevice *dev)
{
    m_device = dev;
}

RingBuffer<SampleFrame> *AcquireThread::getRingBuf()
{
    return &m_ringBuf;
}

bool AcquireThread::acquireOneFrame(SampleFrame &frame)
{
    if(m_hComm == ACSC_INVALID || !m_device)
        return false;

    frame.timeStampMs = QDateTime::currentDateTime().toMSecsSinceEpoch();

    //使能状态
    acsc_GetMotorState(m_hComm,ACSC_AXIS_0,&frame.state0, NULL);
    acsc_GetMotorState(m_hComm,ACSC_AXIS_1,&frame.state1, NULL);

    //位置
    acsc_GetFPosition(m_hComm,ACSC_AXIS_0,&frame.pos1,NULL);
    acsc_GetFPosition(m_hComm,ACSC_AXIS_1,&frame.pos2,NULL);

    //速度
    acsc_GetFVelocity(m_hComm,ACSC_AXIS_0,&frame.vel1, NULL );
    acsc_GetFVelocity(m_hComm,ACSC_AXIS_1,&frame.vel2, NULL );
    //加速度
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FeedAcc0", 0, 0, 0, 0, &frame.acc1, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FeedAcc1", 0, 0, 0, 0, &frame.acc2, nullptr);

    //电流
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"CurrentX1", 0, 0, 0, 0, &frame.current1, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"CurrentX2", 0, 0, 0, 0, &frame.current2, nullptr);

    //电压
    frame.vol1=getAInvalue(m_hComm,const_cast<char*>("?A\r"));
    frame.vol2=getAInvalue(m_hComm,const_cast<char*>("?A2\r"));

    //NTC
    frame.ntc1_1=m_device->readNTCRawChannel(0)/100.0;
    frame.ntc1_2=m_device->readNTCRawChannel(1)/100.0;
    frame.ntc1_3=m_device->readNTCRawChannel(2)/100.0;
    frame.ntc1_4=m_device->readNTCRawChannel(3)/100.0;
    frame.ntc2_1=m_device->readNTCRawChannel(4)/100.0;
    frame.ntc2_2=m_device->readNTCRawChannel(5)/100.0;
    frame.ntc2_3=m_device->readNTCRawChannel(6)/100.0;
    frame.ntc2_4=m_device->readNTCRawChannel(7)/100.0;

    frame.tem1=std::max(std::max(frame.ntc1_1,frame.ntc1_2),std::max(frame.ntc1_3,frame.ntc1_4));
    frame.tem2=std::max(std::max(frame.ntc2_1,frame.ntc2_2),std::max(frame.ntc2_3,frame.ntc2_4));

    //PTC
    frame.ptc1_1=m_device->readPTCRawChannel(0)*3.0518;
    frame.ptc1_2=m_device->readPTCRawChannel(1)*3.0518;
    frame.ptc2_1=m_device->readPTCRawChannel(2)*3.0518;
    frame.ptc2_2=m_device->readPTCRawChannel(3)*3.0518;

    frame.temptc1=std::max(frame.ptc1_1,frame.ptc1_2);
    frame.temptc2=std::max(frame.ptc2_1,frame.ptc2_2);

    //流量压力
    //    frame.Flowrate1=getAInvalue(m_hComm,const_cast<char*>("?X1_Flow\r"));
    //    frame.Flowrate2=getAInvalue(m_hComm,const_cast<char*>("?X2_Flow\r"));
    //    frame.inwater1=getAInvalue(m_hComm,const_cast<char*>("?X1_Pressure1\r"));
    //    frame.inwater2=getAInvalue(m_hComm,const_cast<char*>("?X2_Pressure1\r"));
    //    frame.outwater1=getAInvalue(m_hComm,const_cast<char*>("?X1_Pressure2\r"));
    //    frame.outwater2=getAInvalue(m_hComm,const_cast<char*>("?X1_Pressure2\r"));
    //    frame.qiya=getAInvalue(m_hComm,const_cast<char*>("?Main_Pressure\r"));
    //    frame.qiliu=getAInvalue(m_hComm,const_cast<char*>("?Main_Flow\r"));


    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.Flowrate1, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.Flowrate2, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.inwater1, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.inwater2, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.outwater1, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.outwater2, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.qiya, nullptr);
    acsc_ReadReal(m_hComm, ACSC_NONE, (char*)"FACC0", 0, 0, 0, 0, &frame.qiliu, nullptr);

    return true;
}


void AcquireThread::run()
{
    QElapsedTimer sampleTimer;
    sampleTimer.start();
    const int BATCH_WRITE_COUNT = 50;
    QVector<SampleFrame> batchCache;
    batchCache.reserve(BATCH_WRITE_COUNT);

    while(!isInterruptionRequested())
    {
        qint64 loopStart = sampleTimer.elapsed();

        SampleFrame frame;
        bool ok = acquireOneFrame(frame);
        if(ok)
        {
            m_ringBuf.push(frame);
            batchCache.append(frame);
        }

        if(batchCache.size() >= BATCH_WRITE_COUNT)
        {
            batchCache.clear();
        }

        qint64 usedMs = sampleTimer.elapsed() - loopStart;
        qint64 sleepUs = 1000 - usedMs*1000;
        if(sleepUs > 0)
        {
            usleep(sleepUs);
        }
    }
}

bool Widget::openCsvFile(const QString &filePath)
{
    closeCsvFile();
    m_csvFile = new QFile(filePath);
    if(!m_csvFile->open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
    {
        t_addlog(2,QString("CSV文件打开失败:%1").arg(filePath));
        delete m_csvFile;
        m_csvFile=nullptr;
        return false;
    }
    return true;
}

void Widget::closeCsvFile()
{
    if(m_csvFile)
    {
        if(m_csvFile->isOpen())
            m_csvFile->close();
        delete m_csvFile;
        m_csvFile=nullptr;
    }
}

void Widget::appendBatchSampleToCsv(const QVector<SampleFrame> &frames)
{
    if(!m_csvFile || !m_csvFile->isOpen() || !issaveincsv)
        return;
    QString buf;
    buf.reserve(frames.size()*256);
    for(const auto& f : frames)
    {
        buf += f.toCsvLine();
    }
    m_csvFile->write(buf.toUtf8());
    m_csvFile->flush();
}

static volatile bool flag = true;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),
      mPlot1(nullptr),
      mGraph1_1(nullptr),
      mGraph1_2(nullptr),
      mPlot2(nullptr),
      mGraph2_1(nullptr),
      mGraph2_2(nullptr),
      mGraph2_3(nullptr),
      mGraph2_4(nullptr),
      mPlot3(nullptr),
      mGraph3_1(nullptr),
      mGraph3_2(nullptr),
      mPlot4(nullptr),
      mGraph4_1(nullptr),
      mGraph4_2(nullptr),
      mPlot5(nullptr),
      mGraph5_1(nullptr),
      mGraph5_2(nullptr),
      mGraph5_3(nullptr),
      mGraph5_4(nullptr),
      mGraph5_5(nullptr),
      mGraph5_6(nullptr),
      mGraph5_7(nullptr),
      mGraph5_8(nullptr)
{
    ui->setupUi(this);
    configureResponsiveLayout();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
    log_init();    //日志初始化
    t_addlog(1,"日志系统初始化完成");
    chart_init();  //图表初始化
    t_addlog(1,"图表初始化完成");
    widget_init(); //控件设置
    t_addlog(1,"控件设置完成");
    style_init();  //控件样式
    t_addlog(1,"控件样式初始化完成");
    system_init(); //系统设置
    t_addlog(1,"系统设置完成");
    connect_init();//信号槽连接
    t_addlog(1,"信号槽连接完成");

    m_acquireThread = new AcquireThread(this);


    connect_ACS(); //连接控制器

}

void Widget::configureResponsiveLayout()
{
    // Page changes must not raise the top-level window's minimum size.
    ui->gridLayout_114->setSizeConstraint(QLayout::SetNoConstraint);

    const QList<QWidget *> dimensionLockedWidgets = {
        ui->tabWidget,
        ui->system_min,
        ui->system_max,
        ui->system_exit,
        ui->start_stop,
        ui->start_tips_normal,
        ui->start_tips_warning,
        ui->start_tips_wrong,
        ui->start_tips_stop,
        ui->zsd_enable1,
        ui->zsd_enable2,
        ui->zsd_move1,
        ui->zsd_move2,
        ui->zsd_tem1,
        ui->zsd_tem2,
        ui->zsd_current1,
        ui->zsd_current2,
        ui->zsd_flow1,
        ui->zsd_flow2,
        ui->zsd_air1,
        ui->zsd_air2,
        ui->zsd_gm1,
        ui->zsd_gm2,
        ui->label_5_1,
        ui->label_5_2,
        ui->label_5_3,
        ui->label_5_4,
        ui->label_5_5,
        ui->label_5_6,
        ui->label_5_7,
        ui->label_5_8,
        ui->label_1_1,
        ui->label_1_2,
        ui->label_2_1,
        ui->label_2_2,
        ui->label_2_3,
        ui->label_2_4,
        ui->label_3_1,
        ui->label_3_2,
        ui->label_4_1,
        ui->label_4_2
    };

    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    const QList<QWidget *> childWidgets = findChildren<QWidget *>();
    for (QWidget *childWidget : childWidgets)
    {
        if (dimensionLockedWidgets.contains(childWidget)
                || childWidget->objectName().startsWith(QStringLiteral("qt_")))
        {
            continue;
        }

        childWidget->setMinimumSize(0, 0);
        childWidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }

    const QList<QWidget *> testPageWidgets = ui->page_4->findChildren<QWidget *>();
    for (QWidget *testPageWidget : testPageWidgets)
    {
        if (dimensionLockedWidgets.contains(testPageWidget)
                || testPageWidget->objectName().startsWith(QStringLiteral("qt_")))
        {
            continue;
        }

        QSizePolicy testPageSizePolicy = testPageWidget->sizePolicy();
        const bool isLayoutContainer = testPageWidget->layout() != nullptr
                || testPageWidget == ui->tabWidget_2;
        testPageSizePolicy.setVerticalPolicy(
                    isLayoutContainer ? QSizePolicy::Expanding : QSizePolicy::Preferred);
        testPageWidget->setSizePolicy(testPageSizePolicy);
    }

    const QList<QStackedWidget *> stackedWidgets = findChildren<QStackedWidget *>();
    for (QStackedWidget *stackedWidget : stackedWidgets)
    {
        if (stackedWidget->objectName().startsWith(QStringLiteral("qt_")))
        {
            continue;
        }

        QSizePolicy stackedSizePolicy = stackedWidget->sizePolicy();
        stackedSizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
        stackedSizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
        stackedWidget->setSizePolicy(stackedSizePolicy);

        for (int pageIndex = 0; pageIndex < stackedWidget->count(); ++pageIndex)
        {
            QWidget *pageWidget = stackedWidget->widget(pageIndex);
            QSizePolicy pageSizePolicy = pageWidget->sizePolicy();
            pageSizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
            pageSizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
            pageWidget->setSizePolicy(pageSizePolicy);
        }
    }
}

Widget::~Widget()
{
    if (logWindow) {
        logWindow->close();
        delete logWindow;
    }
    getinfotimer1->stop();
    if(m_acquireThread)
    {
        m_acquireThread->requestInterruption();
        m_acquireThread->wait();
        delete m_acquireThread;
    }

    closeCsvFile();


    delete ui;
}

void Widget::connect_ACS()
{
    //连接设备
    hComm = ACSC_INVALID;
    int port =701;
    hComm = acsc_OpenCommEthernetTCP(const_cast<char*>("10.0.0.100"), port);
    if (hComm == ACSC_INVALID)
    {
        ui->zsd_enable1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_enable2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->test_enable_label_0->setStyleSheet("background-color: black;");
        ui->test_enable_label_1->setStyleSheet("background-color: black;");
        ui->zsd_move1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_move2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_tem1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_tem2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_current1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_current2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_flow1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_flow2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_air1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_air2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_gm1->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_gm2->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; """"   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
        QMessageBox msgBox;
        msgBox.setWindowTitle("提示");
        msgBox.setText("控制器连接失败，请检查通讯");
        msgBox.setIcon(QMessageBox::Warning);
        // 设置整体样式和字体
        msgBox.setStyleSheet(
                    "QMessageBox {"
            "   min-width: 1600px;"
            "   min-height:1300px;"
            "}"
            "QMessageBox QLabel {"
            "   font-size: 25px;"
                    "}"
            "QMessageBox QPushButton {"
            "   font-size: 25px;"
                    "   min-width: 100px;"
                    "   min-height: 50px;"
                    "}"
        );
        msgBox.exec();
        t_addlog(1,"控制器连接失败，请检查通讯");
        return;
    }
    else
    {
        ui->zsd_enable1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_enable2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->test_enable_label_0->setStyleSheet("background-color: gray;");
        ui->test_enable_label_1->setStyleSheet("background-color: gray;");
        ui->zsd_move1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_move2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_tem1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_tem2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_current1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_current2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_flow1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_flow2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_air1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_air2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_gm1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        ui->zsd_gm2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; """"   border: 2px solid gray; ""}").arg(ui->zsd_enable1->width() / 2));
        double Velocity2;
        acsc_GetVelocity(hComm,ACSC_AXIS_0,&Velocity2, NULL );
        ui->line_test_vel->setText(QString::number(Velocity2));
        double Acceleration;
        acsc_GetAcceleration( hComm, ACSC_AXIS_0, &Acceleration, NULL);
        ui->line_test_accdec->setText(QString::number(Acceleration));
        t_addlog(1,"控制器连接成功");
        if(!connect_NtcPtc())//连接ntcptc
        {
            t_addlog(1,"温度模块连接失败，请检查通讯");
            QMessageBox msgBox;
            msgBox.setWindowTitle("提示");
            msgBox.setText("控制器连接失败，请检查通讯");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setStyleSheet(
                        "QMessageBox {"
                "   min-width: 1600px;"
                "   min-height:1300px;"
                "}"
                "QMessageBox QLabel {"
                "   font-size: 25px;"
                        "}"
                "QMessageBox QPushButton {"
                "   font-size: 25px;"
                        "   min-width: 100px;"
                        "   min-height: 50px;"
                        "}"
            );
            msgBox.exec();
            return;
        }

        m_acquireThread->setHandle(hComm);
        m_acquireThread->setDevicePtr(device);


        QMutexLocker locker(&m_filenameMutex);
        openCsvFile(filename);

        getinfotimer1->start(100); //UI刷新保护判断100ms一次
        m_acquireThread->start();


    }



}

bool Widget::connect_NtcPtc()
{
    device = new ModbusTemperatureServer(this);
    QObject::connect(device, &ITCPDevice::stateChanged,
                     [](ITCPDevice::State state) {
        switch (state) {
        case ITCPDevice::Stopped:
            qDebug() << "[Server] State: Stopped";
            break;
        case ITCPDevice::Listening:
            qDebug() << "[Server] State: Listening, waiting for device to connect...";
            break;
        case ITCPDevice::ClientConnected:
            qDebug() << "[Server] State: Device connected! Starting polling.";
            break;
        case ITCPDevice::Error:
            qDebug() << "[Server] State: Error";
            break;
        }
    });

    QObject::connect(device, &ITCPDevice::errorOccurred,
                     [](const QString& err) {
        qDebug() << "[Server] Error:" << err;
    });


    const QString serverIP    = "192.168.0.155";
    const uint16_t serverPort = 20108;

    if (!device->startServer(serverIP, serverPort)) {
        qDebug() << "[Server] Failed to start on" << serverIP << ":" << serverPort;
        return false;
    }
    return true;
}

void Widget::log_init()
{
    logWindow = new LogWindow(nullptr);
    logWindow->hide();
    SaveLog::Instance()->start(getlogfile());
}

void Widget::t_addlog(int level,const QString& message)
{
    if(level==1)
    {
        qInfo()<<message;
        logWindow->addLog("Info",message);
    }else if(level==2)
    {
        qWarning()<<message;
        logWindow->addLog("Warning",message);
    }else
    {
        return;
    }
}


void Widget::chart_init()
{
    QFont axisFont,axisFont_start;
    axisFont.setPointSize(15);
    axisFont_start.setPointSize(22);
    ///----------------------------------开始界面图表----------------------------------///
    mPlot5 = new QCustomPlot(ui->frame_start);
    ui->frame_start->setLayout(new QVBoxLayout());
    ui->frame_start->layout()->setContentsMargins(0, 0, 0, 0);
    ui->frame_start->layout()->addWidget(mPlot5);
    ui->frame_start->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPlot5->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始化曲线
    mGraph5_1 = mPlot5->addGraph();
    mGraph5_2 = mPlot5->addGraph();
    mGraph5_1->setPen(QPen(Qt::red, 2));
    mGraph5_2->setPen(QPen(Qt::blue, 2));
    mGraph5_3 = mPlot5->addGraph();
    mGraph5_4 = mPlot5->addGraph();
    mGraph5_3->setPen(QPen(QColor(255, 165, 0), 2));
    mGraph5_4->setPen(QPen(Qt::darkGreen, 2));
    mGraph5_5 = mPlot5->addGraph();
    mGraph5_6 = mPlot5->addGraph();
    mGraph5_5->setPen(QPen(Qt::red, 2));
    mGraph5_6->setPen(QPen(Qt::blue, 2));
    mGraph5_7 = mPlot5->addGraph();
    mGraph5_8 = mPlot5->addGraph();
    mGraph5_7->setPen(QPen(QColor(255, 165, 0), 2));
    mGraph5_8->setPen(QPen(Qt::darkGreen, 2));

    // 坐标轴设置
    mPlot5->xAxis->setLabel("次数");
    mPlot5->xAxis->setLabelFont(axisFont_start);
    mPlot5->yAxis->setLabel("                                      电流(A)");
    mPlot5->yAxis->setLabelFont(axisFont_start);
    mPlot5->yAxis2->setLabel("温度(℃)                                      ");
    mPlot5->yAxis2->setLabelFont(axisFont_start);
    mPlot5->yAxis2->setVisible(true);
    mPlot5->xAxis->setRange(0, 20);
    mPlot5->yAxis->setRange(-20, 150);
    mPlot5->yAxis2->setRange(-20, 150);


    ///----------------------------------测试界面图表1----------------------------------///
    mPlot1 = new QCustomPlot(ui->frame_test1);
    ui->frame_test1->setLayout(new QVBoxLayout());
    ui->frame_test1->layout()->setContentsMargins(0, 0, 0, 0);
    ui->frame_test1->layout()->addWidget(mPlot1);
    ui->frame_test1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPlot1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始化曲线
    mGraph1_1 = mPlot1->addGraph();
    mGraph1_2 = mPlot1->addGraph();
    mGraph1_1->setPen(QPen(Qt::red, 2));
    mGraph1_2->setPen(QPen(Qt::blue, 2));
    // 坐标轴设置
    mPlot1->xAxis->setLabel("          时间 (s)");
    mPlot1->yAxis->setLabel("                           位置 (mm)");
    mPlot1->xAxis->setLabelFont(axisFont);
    mPlot1->yAxis->setLabelFont(axisFont);
    mPlot1->xAxis->setRange(0, 20);
    mPlot1->yAxis->setRange(-800, 100);


    ///----------------------------------测试界面图表2----------------------------------///
    mPlot2 = new QCustomPlot(ui->frame_test2);
    ui->frame_test2->setLayout(new QVBoxLayout());
    ui->frame_test2->layout()->setContentsMargins(0, 0, 0, 0);
    ui->frame_test2->layout()->addWidget(mPlot2);
    ui->frame_test2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPlot2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始化位置曲线
    mGraph2_1 = mPlot2->addGraph();
    mGraph2_2 = mPlot2->addGraph();
    mGraph2_3 = mPlot2->addGraph();
    mGraph2_4 = mPlot2->addGraph();
    mGraph2_1->setPen(QPen(Qt::red, 2));
    mGraph2_2->setPen(QPen(Qt::blue, 2));
    mGraph2_3->setPen(QPen(QColor(255, 165, 0), 2));
    mGraph2_4->setPen(QPen(Qt::darkGreen, 2));
    // 坐标轴设置
    mPlot2->xAxis->setLabel("时间 (s)");
    mPlot2->yAxis->setLabel("           速度/加速度 (mm/s)/(mm/s²)");
    mPlot2->xAxis->setLabelFont(axisFont);
    mPlot2->yAxis->setLabelFont(axisFont);
    mPlot2->xAxis->setRange(0, 20);
    mPlot2->yAxis->setRange(-10000, 10000);

    ///----------------------------------测试界面图表3----------------------------------///
    mPlot3 = new QCustomPlot(ui->frame_test3);
    ui->frame_test3->setLayout(new QVBoxLayout());
    ui->frame_test3->layout()->setContentsMargins(0, 0, 0, 0);
    ui->frame_test3->layout()->addWidget(mPlot3);
    ui->frame_test3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPlot3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始化位置曲线
    mGraph3_1 = mPlot3->addGraph();
    mGraph3_2 = mPlot3->addGraph();
    mGraph3_1->setPen(QPen(Qt::red, 2));
    mGraph3_2->setPen(QPen(Qt::blue, 2));

    // 坐标轴设置
    mPlot3->xAxis->setLabel("次数");
    mPlot3->yAxis->setLabel("                           温度 (℃)");
    mPlot3->xAxis->setLabelFont(axisFont);
    mPlot3->yAxis->setLabelFont(axisFont);
    mPlot3->xAxis->setRange(0, 20);
    mPlot3->yAxis->setRange(-20, 150);



    ///----------------------------------测试界面图表4----------------------------------///
    mPlot4 = new QCustomPlot(ui->frame_test4);
    ui->frame_test4->setLayout(new QVBoxLayout());
    ui->frame_test4->layout()->setContentsMargins(0, 0, 0, 0);
    ui->frame_test4->layout()->addWidget(mPlot4);
    ui->frame_test4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mPlot4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始化
    mGraph4_1 = mPlot4->addGraph();
    mGraph4_2 = mPlot4->addGraph();
    mGraph4_1->setPen(QPen(Qt::red, 2));
    mGraph4_2->setPen(QPen(Qt::blue, 2));

    // 坐标轴
    mPlot4->xAxis->setLabel("次数");
    mPlot4->yAxis->setLabel("                            电流 (A)");
    mPlot4->xAxis->setLabelFont(axisFont);
    mPlot4->yAxis->setLabelFont(axisFont);
    mPlot4->xAxis->setRange(0, 20);
    mPlot4->yAxis->setRange(-10, 200);

    // 交互
    mPlot1->setInteractions(QCP::iRangeDrag);
    mPlot2->setInteractions(QCP::iRangeDrag);
    mPlot3->setInteractions(QCP::iRangeDrag);
    mPlot4->setInteractions(QCP::iRangeDrag);
    mPlot5->setInteractions(QCP::iRangeDrag);

    // 事件过滤
    mPlot1->installEventFilter(this);
    mPlot2->installEventFilter(this);
    mPlot3->installEventFilter(this);
    mPlot4->installEventFilter(this);
    mPlot5->installEventFilter(this);

    // 信号连接
    connect(mPlot1, &QCustomPlot::mousePress, this, &Widget::onMousePress);
    connect(mPlot1, &QCustomPlot::mouseRelease, this, &Widget::onMouseRelease);
    connect(mPlot2, &QCustomPlot::mousePress, this, &Widget::onMousePress);
    connect(mPlot2, &QCustomPlot::mouseRelease, this, &Widget::onMouseRelease);
    connect(mPlot3, &QCustomPlot::mousePress, this, &Widget::onMousePress);
    connect(mPlot3, &QCustomPlot::mouseRelease, this, &Widget::onMouseRelease);
    connect(mPlot4, &QCustomPlot::mousePress, this, &Widget::onMousePress);
    connect(mPlot4, &QCustomPlot::mouseRelease, this, &Widget::onMouseRelease);
    connect(mPlot5, &QCustomPlot::mousePress, this, &Widget::onMousePress);
    connect(mPlot5, &QCustomPlot::mouseRelease, this, &Widget::onMouseRelease);


    // 数据初始化
    mTimeData.reserve(3000);
    mnumData.reserve(3000);
    mData1_1.reserve(3000);
    mData1_2.reserve(3000);
    mData2_1.reserve(3000);
    mData2_2.reserve(3000);
    mData3_1.reserve(3000);
    mData3_2.reserve(3000);
    mData4_1.reserve(3000);
    mData4_2.reserve(3000);
    mData5_1.reserve(3000);
    mData5_2.reserve(3000);
    mData5_3.reserve(3000);
    mData5_4.reserve(3000);
    mData5_5.reserve(3000);
    mData5_6.reserve(3000);
    mData5_7.reserve(3000);
    mData5_8.reserve(3000);


}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    QCustomPlot *currentPlot = qobject_cast<QCustomPlot*>(obj);
    if (!currentPlot) return QWidget::eventFilter(obj, event);

    if (event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

        mIsUserInteracting = true;
        QTimer::singleShot(1000, [this](){ mIsUserInteracting = false; });

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        QPoint pos = wheelEvent->pos();
#else
        QPoint pos = wheelEvent->position().toPoint();
#endif

        double xCenter = qBound(currentPlot->xAxis->range().lower,
                                currentPlot->xAxis->pixelToCoord(pos.x()),
                                currentPlot->xAxis->range().upper);
        double yCenter = currentPlot->yAxis->pixelToCoord(pos.y());

        double factor = (wheelEvent->angleDelta().y() > 0) ? 0.85 : 1.15;

        // X轴缩放
        QCPRange xRange = currentPlot->xAxis->range();
        currentPlot->xAxis->setRange(
                    xCenter - (xCenter - xRange.lower) * factor,
                    xCenter + (xRange.upper - xCenter) * factor
                    );

        // Y轴缩放
        QCPRange yRange = currentPlot->yAxis->range();
        currentPlot->yAxis->setRange(
                    yCenter - (yCenter - yRange.lower) * factor,
                    yCenter + (yRange.upper - yCenter) * factor
                    );

        // 双Y轴图表
        if (currentPlot == mPlot5) {
            mPlot5->yAxis2->setRange(mPlot5->yAxis->range() * 2);
        }

        currentPlot->replot();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void Widget::onMousePress(QMouseEvent *event)
{
    QCustomPlot *currentPlot = qobject_cast<QCustomPlot*>(sender());
    if (!currentPlot) return;

    movesign1 = movesign2 = movesign3 = movesign4 = movesign5 = 1;
    if (event->button() == Qt::RightButton && !mTimeData.isEmpty())
    {
        const double initialXRange = 20.0;
        double currentEnd = mTimeData.last();

        double newXMin = qMax(currentEnd - initialXRange, 0.0);
        double newXMax = qMax(currentEnd, initialXRange);



        if (currentPlot == mPlot5) {
            double lastX;
            if (mGraph5_1) {
                QSharedPointer<QCPGraphDataContainer> dataContainer = mGraph5_1->data();
                if (!dataContainer->isEmpty()) {
                    QCPGraphDataContainer::const_iterator lastPointIterator = dataContainer->constEnd();
                    --lastPointIterator;

                    lastX = lastPointIterator->key;
                    //lastY = lastPointIterator->value;

                    //qDebug() << "最后一个点的坐标: (" << lastX << ", " << lastY << ")";
                } else {
                    qDebug() << "数据容器为空，没有数据点";
                }
            } else {
                qDebug() << "mGraph3_1 未初始化或为空";
            }

            currentPlot->xAxis->setRange(lastX-20, lastX);
            currentPlot->yAxis->setRange(-20, 150);
            currentPlot->yAxis2->setRange(-20, 150);

        }else if (currentPlot == mPlot1) {
            currentPlot->xAxis->setRange(newXMin, newXMax);
            currentPlot->yAxis->setRange(-800, 100);
        }else if (currentPlot == mPlot2) {
            currentPlot->xAxis->setRange(newXMin, newXMax);
            currentPlot->yAxis->setRange(-10000, 10000);
        }else if (currentPlot == mPlot3) {
            double lastX;
            if (mGraph3_1) {
                QSharedPointer<QCPGraphDataContainer> dataContainer = mGraph3_1->data();
                if (!dataContainer->isEmpty()) {
                    QCPGraphDataContainer::const_iterator lastPointIterator = dataContainer->constEnd();
                    --lastPointIterator;


                    lastX = lastPointIterator->key;
                    //lastY = lastPointIterator->value;

                    //qDebug() << "最后一个点的坐标: (" << lastX << ", " << lastY << ")";
                } else {
                    qDebug() << "数据容器为空，没有数据点";
                }
            } else {
                qDebug() << "mGraph3_1 未初始化或为空";
            }

            currentPlot->xAxis->setRange(lastX-20, lastX);
            currentPlot->yAxis->setRange(-20, 150);
        }else if (currentPlot == mPlot4) {
            double lastX;
            if (mGraph4_1) {
                QSharedPointer<QCPGraphDataContainer> dataContainer = mGraph4_1->data();
                if (!dataContainer->isEmpty()) {
                    QCPGraphDataContainer::const_iterator lastPointIterator = dataContainer->constEnd();
                    --lastPointIterator;

                    lastX = lastPointIterator->key;
                    //lastY = lastPointIterator->value;

                    //qDebug() << "最后一个点的坐标: (" << lastX << ", " << lastY << ")";
                } else {
                    qDebug() << "数据容器为空，没有数据点";
                }
            } else {
                qDebug() << "mGraph3_1 未初始化或为空";
            }

            currentPlot->xAxis->setRange(lastX-20, lastX);
            currentPlot->yAxis->setRange(-40, 150);
        }


        currentPlot->replot();
        mIsUserInteracting = true;
        QTimer::singleShot(1000, [this](){ mIsUserInteracting = false; });
    }
    else
    {
        mIsUserInteracting = true;
    }
}

void Widget::onMouseRelease(QMouseEvent *)
{
    mIsUserInteracting = false;
}

//系统设置初始化
void Widget::system_init()
{
    //界面初始化测试工程次数
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/testnumber.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;

    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testnumber=")) {
            bool ok;
            QString number = line.mid(11);
            file.close();
            ui->label_19->setText(number);
        }
    }


    init_protcet_xml();

    filename=createcsv();
    xunhuanfilename=createxunhuancsv();
    gettestnumber();

    QString a6=readTestNum();
    allrecyclenum=(a6).toInt();

    ui->above_all_num->setText(a6);
    oldtime=readTestTime();
    ui->above_time->setText(oldtime);

    csvsize=(ui->line_set_csv_size->text().toInt())*1000;
    logsize=(ui->line_set_log_size->text().toInt())*1000;


    currentTime = QDateTime::currentDateTime();
    millisecondsSinceEpoch = currentTime.toMSecsSinceEpoch();



    //生成查找字典
    //lookupDict=createLookupDict(R,T,sizeof(R) / sizeof(R[0]));

    issaveincsv=1;


    if(*loginstate==1)
    {
        user_enableall();
    }else if(*loginstate==0)
    {
        user_disableall();
    }
    else
    {
        qDebug()<<"账户未成功初始化";
    }



    m_TempAlarm1={0};
    m_TempAlarm2={0};
    m_CurrAlarm1={0};
    m_CurrAlarm2={0};
    m_shuiliul1={0};
    m_shuiliul2={0};


    //initspdlog();
    tempmodbus=new TemperatureModule();
}

//控件设置初始化
void Widget::widget_init()
{
    ui->tabWidget->setTabText(0, tr("开始"));
    ui->tabWidget->setTabText(1, tr("测试"));
    ui->tabWidget->setTabText(2, tr("设置"));
    ui->tabWidget->setTabText(3, tr("帮助"));

    //所有checkbox默认摁下
    ui->checkBox_start_current1->click();
    ui->checkBox_start_current2->click();
    ui->checkBox_start_tem1->click();
    ui->checkBox_start_tem2->click();
    ui->checkBox_test1_pos1->click();
    ui->checkBox_test1_pos2->click();
    ui->checkBox_test2_acc1->click();
    ui->checkBox_test2_acc2->click();
    ui->checkBox_test2_vel1->click();
    ui->checkBox_test2_vel2->click();
    ui->checkBox_test3_tem1->click();
    ui->checkBox_test3_tem2->click();
    ui->checkBox_test4_current1->click();
    ui->checkBox_test4_current2->click();

    ui->tabWidget_2->setTabText(0, tr("参数设置"));
    ui->tabWidget_2->setTabText(1, tr("点动设置"));

    ui->set_protect_key->click();
    ui->set_save_key->click();
    ui->comboBox->setCurrentIndex(2);

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
    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);
    QString imagePath2 = imagePath.mid(imagePath.indexOf('=') + 1);
    ui->label_set_csv_path->setText(datapath2);
    ui->label_set_image_path->setText(imagePath2);
}

//控件样式初始化
void Widget::style_init()
{
    ///----------------------------------------------------总样式----------------------------------------------------///
    //总样式
    if(1){
        QString globalStyle = R"(
/* 基础全局样式 */
QWidget {
    font-family: "Microsoft YaHei";
    background-color: #f5f6fa;
}

/* 主标题样式 */
QLabel#label_5 {
    color: #2c3e50;
    font-weight: bold;
}

/* 选项卡样式 */
QTabWidget::pane {
    border: none;
}

QTabBar::tab {
    background: #dcdde1;
    color: #2c3e50;

}

QTabBar::tab:selected {
    background: #487eb0;
    color: white;
    font-weight: bold;
}

/* 按钮通用样式 */
QPushButton {
    background-color: #487eb0;
    color: white;
    border: none;
    border-radius: 8px;
}

QPushButton:hover {
    background-color: #40739e;
}

QPushButton:pressed {
    background-color: #2c3e50;
}

/* 特殊按钮样式 */
QPushButton#system_min {
    background-color: #e6f0ff;
}
QPushButton#system_max {
    background-color: #e6f0ff;
}
QPushButton#system_exit {
    background-color: #e6f0ff;
}
/* 输入框样式 */
QLineEdit {
    background: white;
    border: 2px solid #dcdde1;
    border-radius: 8px;
    selection-background-color: #487eb0;
}

QLineEdit:focus {
    border-color: #487eb0;
}

/* 搜索框特殊样式 */
QLineEdit#searchEdit {
    background-image: url(:/icons/search.png);
    background-repeat: no-repeat;
    background-position: left center;
    background-origin: content;
}

QLineEdit::placeholder {
    color: #95a5a6;
    font-style: italic;
}

/* 数据展示框样式 */
QFrame[frameShape="4"] {  /* StyledPanel */
    background: white;
    border: 2px solid #dcdde1;
    border-radius: 8px;
}

/* 表格样式 */
QTableView {
    background: white;
    border: 2px solid #dcdde1;
    gridline-color: #ecf0f1;
    selection-background-color: #487eb0;
}

QHeaderView::section {
    background-color: #487eb0;
    color: white;
}

/* 标签样式 */
QLabel[objectName^="label_"] {
    color: #2c3e50;
}

/* 状态标签样式 */
QLabel[objectName^="zsd_"] {
    font-weight: bold;
    color: #27ae60;
}

/* 文本编辑区样式 */
QTextEdit {
    background: white;
    border: 2px solid #dcdde1;
    border-radius: 8px;
}

/* 滚动条样式 */
QScrollBar:vertical {
    background: #ecf0f1;
}

QScrollBar::handle:vertical {
    background: #bdc3c7;
    border-radius: 6px;
}
)";
        this->setStyleSheet(globalStyle);
    }
    //checkbox样式
    if(1)
    {
        QString checkbox_style="QCheckBox::indicator {"
        "   width: 20px;"              // 宽度
                "   height: 20px;"             // 高度
                "   border-radius: 10px;"      // 圆形（半径=宽度/2）
                "   border: 2px solid #999999;" // 边框颜色
                "   background-color: white;"  // 背景颜色
                "}"

        // 选中状态
                "QCheckBox::indicator:checked {"
        "   background-color: #0078D7;" // 选中时的背景颜色
                "   border: 2px solid #0078D7;" // 选中时的边框颜色
                "}"

        // 悬停状态
                "QCheckBox::indicator:hover {"
        "   border: 2px solid #0078D7;" // 悬停时的边框颜色
                "}"

        // 禁用状态
                "QCheckBox::indicator:disabled {"
        "   background-color: #CCCCCC;" // 禁用时的背景颜色
                "   border: 2px solid #999999;" // 禁用时的边框颜色
                "}";
        ui->checkBox_start_current1->setStyleSheet(checkbox_style);
        ui->checkBox_start_current2->setStyleSheet(checkbox_style);
        ui->checkBox_start_tem1->setStyleSheet(checkbox_style);
        ui->checkBox_start_tem2->setStyleSheet(checkbox_style);
        ui->checkBox_start_current_old1->setStyleSheet(checkbox_style);
        ui->checkBox_start_current_old2->setStyleSheet(checkbox_style);
        ui->checkBox_start_tem_old1->setStyleSheet(checkbox_style);
        ui->checkBox_start_tem_old2->setStyleSheet(checkbox_style);
        ui->checkBox_test1_pos1->setStyleSheet(checkbox_style);
        ui->checkBox_test1_pos2->setStyleSheet(checkbox_style);
        ui->checkBox_test2_acc1->setStyleSheet(checkbox_style);
        ui->checkBox_test2_acc2->setStyleSheet(checkbox_style);
        ui->checkBox_test2_vel1->setStyleSheet(checkbox_style);
        ui->checkBox_test2_vel2->setStyleSheet(checkbox_style);
        ui->checkBox_test3_tem1->setStyleSheet(checkbox_style);
        ui->checkBox_test3_tem2->setStyleSheet(checkbox_style);
        ui->checkBox_test4_current1->setStyleSheet(checkbox_style);
        ui->checkBox_test4_current2->setStyleSheet(checkbox_style);

        //开启保存
        ui->set_save_key->setStyleSheet(checkbox_style);
        //开启保护
        ui->set_protect_key->setStyleSheet(checkbox_style);

    }
    ///----------------------------------------------------页眉样式----------------------------------------------------///
    //设置新凯来图标
    QPixmap pixmap(":/image/sicarrier.png");
    ui->label_sicarrier->setPixmap(pixmap);
    ui->label_sicarrier->setAlignment(Qt::AlignCenter); // 设置图标居中显示

    //设置最小化最大化关闭图标
    QIcon icon1(":/image/min.png");
    QIcon icon2(":/image/max.png");
    QIcon icon3(":/image/close.png");
    ui->system_min->setIcon(icon1);
    ui->system_max->setIcon(icon2);
    ui->system_exit->setIcon(icon3);



    ///----------------------------------------------------开始界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------开始界面各状态组件设置----------------------------------------------------///

    ui->gridLayout_28->setSpacing(0); // 设置frame之间的间距
    ui->gridLayout_28->setContentsMargins(0, 0, 0, 0); // 设置整体边距
    for (int i = 1; i <= 24; ++i) {
        // 动态获取 QFrame 对象
        QFrame *frame = findChild<QFrame*>(QString("frame_%1").arg(i));
        if (frame) {
            // 设置样式表
            frame->setStyleSheet(
                        QString("QFrame#frame_%1 { border: 1px solid black; } "
                        "QFrame#frame_%1 QLabel { border: none; }").arg(i));
        }
    }

    //4个提示状态灯label设置，固定颜色
    ui->start_tips_normal->setFixedSize(50, 50);
    ui->start_tips_normal->setText(" ");
    ui->start_tips_normal->setStyleSheet(QString("QLabel { ""   background-color: green; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->start_tips_normal->width() / 2));
    ui->start_tips_warning->setFixedSize(50, 50);
    ui->start_tips_warning->setText(" ");
    ui->start_tips_warning->setStyleSheet(QString("QLabel { ""   background-color: yellow; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->start_tips_warning->width() / 2));
    ui->start_tips_wrong->setFixedSize(50, 50);
    ui->start_tips_wrong->setText(" ");
    ui->start_tips_wrong->setStyleSheet(QString("QLabel { ""   background-color: red; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->start_tips_wrong->width() / 2));
    ui->start_tips_stop->setFixedSize(50, 50);
    ui->start_tips_stop->setText(" ");
    ui->start_tips_stop->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->start_tips_stop->width() / 2));
    ui->zsd_enable1->setFixedSize(30, 30);
    ui->zsd_enable1->setText(" ");
    ui->zsd_enable1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
    ui->zsd_enable2->setFixedSize(30, 30);
    ui->zsd_enable2->setText(" ");
    ui->zsd_enable2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_enable2->width() / 2));
    ui->zsd_move1->setFixedSize(30, 30);
    ui->zsd_move1->setText(" ");
    ui->zsd_move1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_move1->width() / 2));
    ui->zsd_move2->setFixedSize(30, 30);
    ui->zsd_move2->setText(" ");
    ui->zsd_move2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_move2->width() / 2));
    ui->zsd_tem1->setFixedSize(30, 30);
    ui->zsd_tem1->setText(" ");
    ui->zsd_tem1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_tem1->width() / 2));
    ui->zsd_tem2->setFixedSize(30, 30);
    ui->zsd_tem2->setText(" ");
    ui->zsd_tem2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_tem2->width() / 2));
    ui->zsd_current1->setFixedSize(30, 30);
    ui->zsd_current1->setText(" ");
    ui->zsd_current1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_current1->width() / 2));
    ui->zsd_current2->setFixedSize(30, 30);
    ui->zsd_current2->setText(" ");
    ui->zsd_current2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_current2->width() / 2));
    ui->zsd_flow1->setFixedSize(30, 30);
    ui->zsd_flow1->setText(" ");
    ui->zsd_flow1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_flow1->width() / 2));
    ui->zsd_flow2->setFixedSize(30, 30);
    ui->zsd_flow2->setText(" ");
    ui->zsd_flow2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_flow2->width() / 2));
    ui->zsd_air1->setFixedSize(30, 30);
    ui->zsd_air1->setText(" ");
    ui->zsd_air1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_air1->width() / 2));
    ui->zsd_air2->setFixedSize(30, 30);
    ui->zsd_air2->setText(" ");
    ui->zsd_air2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_air2->width() / 2));
    ui->zsd_gm1->setFixedSize(30, 30);
    ui->zsd_gm1->setText(" ");
    ui->zsd_gm1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_gm1->width() / 2));
    ui->zsd_gm2->setFixedSize(30, 30);
    ui->zsd_gm2->setText(" ");
    ui->zsd_gm2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_gm2->width() / 2));





    ui->start_stop->setStyleSheet(
                "QPushButton {"
                "   min-width: 260px;"          // 按钮最小宽度
                "   min-height: 260px;"         // 按钮最小高度
                "   border-radius: 140px;"       // 圆形（半径=宽度/2）
                "   border: 10px solid #FFD700;" // 外圈黄色边框
                "   background-color: #FF0000;" // 内圆红色背景
                "   color: #FFD700;"            // 文字颜色
                "   font-family: 'Microsoft YaHei';" // 字体
                "   font-weight: bold;"         // 加粗
                "   font-size: 100px;"           // 字号
                "   padding: 0px;"              // 清除原有偏移
                "   background-clip: padding-box;"
                "   text-align: center;"        // 水平居中
                "   vertical-align: middle;"     // 垂直居中
                "   margin: 0px;"               // 清除外边距
                "}"
        "QPushButton:hover {"
        "   border-color: #FFEE00;"
        "   color: #FFEE00;"
        "}"
        "QPushButton:pressed {"
        "   border-color: #CCB300;"
        "   background-color: #CC0000;"
        "}"
    );




    ///----------------------------------------------------测试界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------测试界面各状态组件设置----------------------------------------------------///
    ui->gridLayout_33->setSpacing(0); // 设置frame之间的间距
    ui->gridLayout_33->setContentsMargins(0, 0, 0, 0); // 设置整体边距
    for (int i = 26; i <= 49; ++i) {
        QFrame *frame = findChild<QFrame*>(QString("frame_%1").arg(i));
        if (frame) {
            frame->setStyleSheet(
                        QString("QFrame#frame_%1 { border: 1px solid #487eb0; } "
                    "QFrame#frame_%1 QLabel { border: none; }").arg(i));}}


    ///----------------------------------------------------设置界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------设置界面各状态组件设置----------------------------------------------------///


    ui->gridLayout_82->setSpacing(0);
    ui->gridLayout_82->setContentsMargins(0, 0, 0, 0);
    ui->gridLayout_95->setSpacing(0);
    ui->gridLayout_95->setContentsMargins(0, 0, 0, 0);
    ui->gridLayout_96->setSpacing(0);
    ui->gridLayout_96->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout_10->setSpacing(0);
    ui->verticalLayout_10->setContentsMargins(0, 0, 0, 0);
    ui->verticalLayout_16->setSpacing(0);
    ui->verticalLayout_16->setContentsMargins(0, 0, 0, 0);
    for (int i = 50; i <= 99; ++i) {
        QFrame *frame = findChild<QFrame*>(QString("frame_%1").arg(i));
        if (frame) {
            frame->setStyleSheet(
                        QString("QFrame#frame_%1 { border: 1px solid #487eb0; } "
                    "QFrame#frame_%1 QLabel { border: none; }").arg(i));}}


    ///----------------------------------------------------帮助界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------帮助界面各状态组件设置----------------------------------------------------///


    ui->searchEdit->addAction(QIcon(":/image/R-C.png"),QLineEdit::LeadingPosition);

    ui->searchEdit->setPlaceholderText("输入关键字");

    ui->searchEdit->setStyleSheet(R"(
        QLineEdit {
            color: black;                   /* 正常文本颜色 */
            border-radius: 10px;            /* 可选圆角 */
            padding: 5px;                   /* 内边距 */
        }
        QLineEdit::placeholder {
            color: #999999;                /* 暗淡的灰色 */
            font-style: italic;            /* 可选斜体 */
        }
    )");


    // ui->widget_2->setStyleSheet("background-color: #E0F7FA;");


    ui->widget_2->setStyleSheet(
                "QWidget { background-color: #E0E0E0; } " // 设置背景颜色为淡灰色
                "QPushButton { "
        "    background-color: #487eb0; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #40739e; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #2c3e50; "
        "} "
        "QPushButton#system_min { "
        "    background-color: #e6f0ff; "
        "} "
        "QPushButton#system_max { "
        "    background-color: #e6f0ff; "
        "} "
        "QPushButton#system_exit { "
        "    background-color: #e6f0ff; "
        "}"
    );

    ui->frame->setStyleSheet(
                "QFrame {"
                "    border: 2px solid #487eb0;"  // 外框样式
                "    border-radius: 4px;"         // 可选圆角
                "}"
            );
    ui->frame_25->setStyleSheet(
                "QFrame {"
                "    border: 2px solid #487eb0;"  // 外框样式
                "    border-radius: 4px;"         // 可选圆角
                "}"
            );
    ui->comboBox->setStyleSheet(R"(
        /* 主控件 - 全部白色 */
        QComboBox {
            border: 1px solid #cccccc;
            border-radius: 8px;
            padding: 2px 5px;
            background: white;  /* 整体背景色 */
            color: #000000;    /* 文字颜色 */
            min-width: 6em;
        }

        /* 下拉按钮区域 - 也强制白色 */
        QComboBox::drop-down {
            background: white;  /* 右侧按钮背景 */
            border-left: 1px solid #cccccc;  /* 保留分隔线 */
            width: 20px;
            border-top-right-radius: 7px;
            border-bottom-right-radius: 7px;
        }

        /* 下拉箭头图标 - 灰色保持可见 */
        QComboBox::down-arrow {
            image: url(:/icons/down-arrow-gray.png);  /* 使用灰色箭头 */
            width: 12px;
            height: 12px;
        }

        /* 交互状态 */
        QComboBox:hover {
            border-color: #487eb0;  /* 悬停时边框变蓝 */
        }

        /* 下拉列表样式 */
        QComboBox QAbstractItemView {
            border: 1px solid #cccccc;
            background: white;  /* 列表背景 */
            selection-background-color: #487eb0;  /* 选中项高亮 */
        }
    )");


    ui->combox_set_save_hz->setStyleSheet(R"(
        /* 主控件 - 全部白色 */
        QComboBox {
            border: 1px solid #cccccc;
            border-radius: 8px;
            padding: 2px 5px;
            background: white;  /* 整体背景色 */
            color: #000000;    /* 文字颜色 */
            min-width: 6em;
        }

        /* 下拉按钮区域 - 也强制白色 */
        QComboBox::drop-down {
            background: white;  /* 右侧按钮背景 */
            border-left: 1px solid #cccccc;  /* 保留分隔线 */
            width: 20px;
            border-top-right-radius: 7px;
            border-bottom-right-radius: 7px;
        }

        /* 下拉箭头图标 - 灰色保持可见 */
        QComboBox::down-arrow {
            image: url(:/icons/down-arrow-gray.png);  /* 使用灰色箭头 */
            width: 12px;
            height: 12px;
        }

        /* 交互状态 */
        QComboBox:hover {
            border-color: #487eb0;  /* 悬停时边框变蓝 */
        }

        /* 下拉列表样式 */
        QComboBox QAbstractItemView {
            border: 1px solid #cccccc;
            background: white;  /* 列表背景 */
            selection-background-color: #487eb0;  /* 选中项高亮 */
        }
    )");

    ui->label_36->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_37->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_48->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_49->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_68->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_71->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_80->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
    ui->label_83->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");



    ui->label_5_1->setFixedSize(20, 20);
    ui->label_5_1->setText(" ");
    ui->label_5_1->setStyleSheet("QLabel {"" background-color: red;""}");
    ui->label_5_2->setFixedSize(20, 20);
    ui->label_5_2->setText(" ");
    ui->label_5_2->setStyleSheet("QLabel {"" background-color: blue;""}");
    ui->label_5_3->setFixedSize(20, 20);
    ui->label_5_3->setText(" ");
    ui->label_5_3->setStyleSheet("QLabel { background-color: #FFA500; }");
    ui->label_5_4->setFixedSize(20, 20);
    ui->label_5_4->setText(" ");
    ui->label_5_4->setStyleSheet("QLabel {"" background-color: darkGreen;""}");
    ui->label_5_5->setFixedSize(20, 20);
    ui->label_5_5->setText(" ");
    ui->label_5_5->setStyleSheet("QLabel {"" background-color: red;""}");
    ui->label_5_6->setFixedSize(20, 20);
    ui->label_5_6->setText(" ");
    ui->label_5_6->setStyleSheet("QLabel {"" background-color: blue;""}");
    ui->label_5_7->setFixedSize(20, 20);
    ui->label_5_7->setText(" ");
    ui->label_5_7->setStyleSheet("QLabel { background-color: #FFA500; }");
    ui->label_5_8->setFixedSize(20, 20);
    ui->label_5_8->setText(" ");
    ui->label_5_8->setStyleSheet("QLabel {"" background-color: darkGreen;""}");
    ui->label_1_1->setFixedSize(20, 20);
    ui->label_1_1->setText(" ");
    ui->label_1_1->setStyleSheet("QLabel {"" background-color: red;""}");
    ui->label_1_2->setFixedSize(20, 20);
    ui->label_1_2->setText(" ");
    ui->label_1_2->setStyleSheet("QLabel {"" background-color: blue;""}");
    ui->label_2_1->setFixedSize(20, 20);
    ui->label_2_1->setText(" ");
    ui->label_2_1->setStyleSheet("QLabel {"" background-color: red;""}");
    ui->label_2_2->setFixedSize(20, 20);
    ui->label_2_2->setText(" ");
    ui->label_2_2->setStyleSheet("QLabel {"" background-color: blue;""}");
    ui->label_2_3->setFixedSize(20, 20);
    ui->label_2_3->setText(" ");
    ui->label_2_3->setStyleSheet("QLabel { background-color: #FFA500; }");
    ui->label_2_4->setFixedSize(20, 20);
    ui->label_2_4->setText(" ");
    ui->label_2_4->setStyleSheet("QLabel {"" background-color: darkGreen;""}");
    ui->label_3_1->setFixedSize(20, 20);
    ui->label_3_1->setText(" ");
    ui->label_3_1->setStyleSheet("QLabel {"" background-color: red;""}");
    ui->label_3_2->setFixedSize(20, 20);
    ui->label_3_2->setText(" ");
    ui->label_3_2->setStyleSheet("QLabel {"" background-color: blue;""}");
    ui->label_4_1->setFixedSize(20, 20);
    ui->label_4_1->setText(" ");
    ui->label_4_1->setStyleSheet("QLabel {"" background-color: red;""}");
    ui->label_4_2->setFixedSize(20, 20);
    ui->label_4_2->setText(" ");
    ui->label_4_2->setStyleSheet("QLabel {"" background-color: blue;""}");
}

//信号槽初始化
void Widget::connect_init()
{
    getinfotimer1 =new QTimer(this);
    connect(getinfotimer1,&QTimer::timeout,this,&Widget::getinfo);


    connect(this, &Widget::updateWaveData, this, &Widget::handleWaveDataUpdate);//循环运行更新图表信号连接

    this->xunhuantimer = new QTimer;   //循环运动累计时间计时器
    connect(this->xunhuantimer,SIGNAL(timeout()),this,SLOT(updatexunhuanTime()));


    //每秒钟检查一次csv文件大小
    checkcsv_timer =new QTimer(this);  //最后开始循环检查csv大小
    connect(checkcsv_timer,&QTimer::timeout,this,&Widget::checkcsv);
    checkcsv_timer->start(1000);



    connect(ui->set_save_key, &QCheckBox::clicked, this, [this](bool checked) {
        m_shouldSaveCsv = checked;
    });
}















void Widget::updateUI()
{

}

void Widget::getinfo()
{

    SampleFrame frame;
    bool hasData = m_acquireThread->getRingBuf()->getLatest(frame);
    if(!hasData)
        return;

    const int BATCH_CONSUME = 50;
    QVector<SampleFrame> batch = m_acquireThread->getRingBuf()->popBatch(BATCH_CONSUME);
    if(!batch.isEmpty() && issaveincsv && ui->set_save_key->isChecked())
    {
        QMutexLocker locker(&m_filenameMutex);
        appendBatchSampleToCsv(batch);
    }

    int State  = frame.state0;
    int State1 = frame.state1;
    double pos1 = frame.pos1;
    double pos2 = frame.pos2;
    double vel1 = frame.vel1;
    double vel2 = frame.vel2;
    double acc1 = frame.acc1;
    double acc2 = frame.acc2;
    double current1 = frame.current1;
    double current2 = frame.current2;
    double vol1 = frame.vol1;
    double vol2 = frame.vol2;
    double tem1 = frame.tem1;
    double tem2 = frame.tem2;
    double temptc1 = frame.temptc1;
    double temptc2 = frame.temptc2;
    double Flowrate1 = frame.Flowrate1;
    double Flowrate2 = frame.Flowrate2;
    double inwater1 = frame.inwater1;
    double inwater2 = frame.inwater2;
    double outwater1 = frame.outwater1;
    double outwater2 = frame.outwater2;
    double qiya = frame.qiya;
    double qiliu = frame.qiliu;




    QString line = QString::asprintf(
                "【NTC1】%.1f  %.1f  %.1f  %.1f | "
                "【NTC2】%.1f  %.1f  %.1f  %.1f\n"
                "【PTC1】%.1f  %.1f | 【PTC2】%.1f  %.1f\n"
                "T_tem1:%.1f  T_tem2:%.1f | T_ptc1:%.1f  T_ptc2:%.1f\n",
                frame.ntc1_1,frame.ntc1_2,frame.ntc1_3,frame.ntc1_4,
                frame.ntc2_1,frame.ntc2_2,frame.ntc2_3,frame.ntc2_4,
                frame.ptc1_1,frame.ptc1_2,frame.ptc2_1,frame.ptc2_2,
                tem1,tem2,temptc1,temptc2
                );
    ui->textEdit->setText(line);


    //判断各种状态
    //使能状态

    if(State!=16)
    {
        ui->zsd_enable1->setStyleSheet("QLabel { background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->test_enable_label_0->setStyleSheet("background-color: green;");
    }
    else if(State==16)
    {
        ui->zsd_enable1->setStyleSheet("QLabel { background-color: gray; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->test_enable_label_0->setStyleSheet("background-color: gray;");
    }

    if(State1!=16)
    {
        ui->zsd_enable2->setStyleSheet("QLabel { background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->test_enable_label_1->setStyleSheet("background-color: green;");
    }
    else if(State1==16)
    {
        ui->zsd_enable2->setStyleSheet("QLabel { background-color: gray; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->test_enable_label_1->setStyleSheet("background-color: gray;");
    }


    //运动状态
    //    if(t_movestate0 == 33)
    //    {
    //        ui->zsd_move1->setStyleSheet("QLabel { background-color: red; ""border-radius: 15px; " "border: 2px solid black; ""}");
    //    }else
    //    {
    //        ui->zsd_move1->setStyleSheet("QLabel { background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
    //    }
    //    if(t_movestate1 == 33)
    //    {
    //        ui->zsd_move2->setStyleSheet("QLabel { background-color: red; ""border-radius: 15px; " "border: 2px solid black; ""}");
    //    }else
    //    {
    //        ui->zsd_move2->setStyleSheet("QLabel { background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
    //    }



    //电流1
    ui->label_36->setText(QString::number(current1,'f',2));
    //电流2
    ui->label_37->setText(QString::number(current1,'f',2));

    //流量1
    ui->label_50->setText(QString::number(Flowrate1,'f',2));
    //流量2
    ui->label_55->setText(QString::number(Flowrate2,'f',2));
    //进口水压1
    ui->label_48->setText(QString::number(inwater1,'f',2));
    //进口水压2
    ui->label_49->setText(QString::number(inwater2,'f',2));
    //出口水压1
    ui->label_68->setText(QString::number(outwater1,'f',2));
    //出口水压2
    ui->label_71->setText(QString::number(outwater2,'f',2));

    //气压1
    ui->label_63->setText(QString::number(qiya,'f',2));
    //气流
    ui->label_80->setText(QString::number(qiliu,'f',2));

    static qint64 startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double currentTime = (now - startTime) / 1000.0;
    mTimeData.append(currentTime);
    mData1_1.append(pos1);
    mData1_2.append(pos2);
    mData2_1.append(vel1);
    mData2_2.append(vel2);
    mData2_3.append(acc1);
    mData2_4.append(acc2);
    if (mTimeData.size() > 2000) {
        mTimeData.removeFirst();
        mData1_1.removeFirst();
        mData1_2.removeFirst();
        mData2_1.removeFirst();
        mData2_2.removeFirst();
        mData2_3.removeFirst();
        mData2_4.removeFirst();
    }
    if (!mIsUserInteracting && !mTimeData.isEmpty())
    {
        double currentEnd = mTimeData.last();

        // 测试1图表
        if (currentEnd > mPlot1->xAxis->range().upper) {
            if ((currentEnd - mPlot1->xAxis->range().upper) > 5) {
                movesign1 = 0;
            }
            if (movesign1) {
                double range = mPlot1->xAxis->range().size();
                mPlot1->xAxis->setRange(currentEnd - range, currentEnd);
            }
        }

        // 测试2图表
        if (currentEnd > mPlot2->xAxis->range().upper) {
            if ((currentEnd - mPlot2->xAxis->range().upper) > 5) {
                movesign2 = 0;
            }
            if (movesign2) {
                double range = mPlot2->xAxis->range().size();
                mPlot2->xAxis->setRange(currentEnd - range, currentEnd);
            }
        }

    }
    if (!mIsUserInteracting && !mnumData.isEmpty())
    {
        double currentEnd2 = mnumData.last();
        // 测试3图表
        if (currentEnd2 > mPlot3->xAxis->range().upper) {
            if ((currentEnd2 - mPlot3->xAxis->range().upper) > 5) {
                movesign3 = 0;
            }
            if (movesign3) {
                double range = mPlot3->xAxis->range().size();
                mPlot3->xAxis->setRange(currentEnd2 - range, currentEnd2);
            }
        }
        // 测试4图表
        if (currentEnd2 > mPlot4->xAxis->range().upper) {
            if ((currentEnd2 - mPlot4->xAxis->range().upper) > 5) {
                movesign4 = 0;
            }
            if (movesign4) {
                double range = mPlot4->xAxis->range().size();
                mPlot4->xAxis->setRange(currentEnd2 - range, currentEnd2);
            }
        }
        // 开始图表
        if (currentEnd2 > mPlot5->xAxis->range().upper) {
            if ((currentEnd2 - mPlot5->xAxis->range().upper) > 5) {
                movesign5 = 0;
            }
            if (movesign5) {
                double range = mPlot5->xAxis->range().size();
                mPlot5->xAxis->setRange(currentEnd2 - range, currentEnd2);
            }
        }
    }
    mGraph1_1->setData(mTimeData, mData1_1);
    mGraph1_2->setData(mTimeData, mData1_2);
    mGraph2_1->setData(mTimeData, mData2_1);
    mGraph2_2->setData(mTimeData, mData2_2);
    mGraph2_3->setData(mTimeData, mData2_3);
    mGraph2_4->setData(mTimeData, mData2_4);

    mPlot1->replot();
    mPlot2->replot();

    //timeCount += 1.0;








    if(max_current1 ==-0.111){max_current1=current1;}if(max_current1<current1){max_current1=current1;}
    if(max_current2 ==-0.111){max_current2=current2;}if(max_current2<current2){max_current2=current2;}
    if(max_tem1 ==-0.111){max_tem1=tem1;}if(max_tem1<tem1){max_tem1=tem1;}
    if(max_tem2 ==-0.111){max_tem2=tem2;}if(max_tem2<tem2){max_tem2=tem2;}
    if(final_max_current1 ==-0.111){final_max_current1=current1;}if(final_max_current1<current1){final_max_current1=current1;}
    if(final_max_current2 ==-0.111){final_max_current2=current2;}if(final_max_current2<current2){final_max_current2=current2;}
    if(final_max_tem1 ==-0.111){final_max_tem1=tem1;}if(final_max_tem1<tem1){final_max_tem1=tem1;}
    if(final_max_tem2 ==-0.111){final_max_tem2=tem2;}if(final_max_tem2<tem2){final_max_tem2=tem2;}


    ui->above_current1->setText(QString::number(final_max_current1,'f',4));
    ui->above_current2->setText(QString::number(final_max_current2,'f',4));
    ui->above_tem1->setText(QString::number(final_max_tem1,'f',4));
    ui->above_tem2->setText(QString::number(final_max_tem2,'f',4));
    ui->above_pos1->setText(QString::number(pos1, 'f', 3));
    ui->above_pos2->setText(QString::number(pos2, 'f', 3));




    m_abnorTemperature=ui->spin_set_wrong_tem1->text().toDouble();   //异常温度
    m_warnTemperature=ui->spin_set_warn_tem1->text().toDouble();  //告警温度
    m_abnorCurrent=ui->spin_set_wrong_current1->text().toDouble();   //异常电流
    m_warnCurrent=ui->spin_set_warn_current1->text().toDouble();  //告警电流
    m_abnorTemperature2=ui->spin_set_wrong_tem2->text().toDouble();   //异常温度2
    m_warnTemperature2=ui->spin_set_warn_tem2->text().toDouble();  //告警温度2
    m_abnorCurrent2=ui->spin_set_wrong_current2->text().toDouble();   //异常电流2
    m_warnCurrent2=ui->spin_set_warn_current2->text().toDouble();  //告警电流2
    m_shuiliuliang1=ui->spin_set_flow1->text().toDouble();
    m_shuiliuliang2=ui->spin_set_flow2->text().toDouble();
    m_atm1=ui->spin_set_press1->text().toDouble();
    m_atm2=ui->spin_set_press2->text().toDouble();



    if(ui->set_protect_key->isChecked())
    {
        //这里判断电流
        if ((m_warnCurrent < std::abs(current1)) || (m_warnCurrent2 < std::abs(current2))) {
            bool isCritical = false;  // 标记是否触发断电

            // 处理电机1断电逻辑
            if (m_abnorCurrent < std::abs(current1)) {

                isCritical = true;
                ui->zsd_current1->setStyleSheet("QLabel { background-color: red; border-radius: 15px; border: 2px solid black; }");
                if(currentlogsign1)
                {
                    qWarning()<<"电机1当前电流为"+QString::number(current1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前电流为"+QString::number(current1)+",设备停机下使能");
                    currentlogsign1=false;
                }
            }

            // 处理电机2断电逻辑
            if (m_abnorCurrent2 < std::abs(current2)) {

                isCritical = true;
                ui->zsd_current2->setStyleSheet("QLabel { background-color: red; border-radius: 15px; border: 2px solid black; }");
                if(currentlogsign2)
                {
                    qWarning()<<"电机2当前电流为"+QString::number(current2)+",设备停机下使能";
                    logWindow->addLog("Warning","电机2当前电流为"+QString::number(current2)+",设备停机下使能");
                    currentlogsign2=false;
                }
            }

            // 如果触发断电条件
            if (isCritical) {
                ui->test_status4->setStyleSheet("color: red;");
                ui->test_status4->setText("停机");
                stopstop();
                acsc_DisableAll(hComm, nullptr);
                acsc_KillAll(hComm, nullptr);
                return;  // 直接返回避免后续状态覆盖
            }
            // 未触发断电但存在告警
            ui->test_status4->setStyleSheet("color: yellow;");
            ui->test_status4->setText("告警");
            ui->zsd_current1->setStyleSheet((m_warnCurrent < std::abs(current1)) ?
                                                "QLabel { background-color: yellow; border-radius: 15px; border: 2px solid black; }" :
                                                "QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            ui->zsd_current2->setStyleSheet((m_warnCurrent2 < std::abs(current2)) ?
                                                "QLabel { background-color: yellow; border-radius: 15px; border: 2px solid black; }" :
                                                "QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            if(currentlogsign3)
            {
                qWarning()<<"电机1当前电流为"+QString::number(current1);
                qWarning()<<"电机2当前电流为"+QString::number(current2);
                qWarning()<<"电流过高";
                logWindow->addLog("Warning","电机1当前电流为"+QString::number(current1));
                logWindow->addLog("Warning","电机2当前电流为"+QString::number(current2));
                logWindow->addLog("Warning","电流过高");
                currentlogsign3=false;
            }
        }
        else {
            // 完全正常状态
            ui->test_status4->setStyleSheet("color: blue;");
            ui->test_status4->setText("正常");
            ui->zsd_current1->setStyleSheet("QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            ui->zsd_current2->setStyleSheet("QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            if((!currentlogsign1)||(!currentlogsign2)||(!currentlogsign3))
            {
                qInfo()<<"电机电流恢复正常";
                logWindow->addLog("Info","电机电流恢复正常");
                currentlogsign1=true;
                currentlogsign2=true;
                currentlogsign3=true;
            }
        }

        //这里判断温度
        if((m_warnTemperature<tem1)||(m_warnTemperature2<tem2)||(temptc1>3420)||(temptc2>3420)){
            bool isCritical = false;  // 标记是否触发断电

            // 处理电机1断电逻辑
            if (m_abnorTemperature < std::abs(tem1)) {

                isCritical = true;
                ui->zsd_tem1->setStyleSheet("QLabel { background-color: red; border-radius: 15px; border: 2px solid black; }");
                if(temlogsign1)
                {
                    qWarning()<<"电机1当前温度为"+QString::number(tem1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前温度为"+QString::number(tem1)+",设备停机下使能");
                    temlogsign1=false;
                }

            }
            // 处理电机2断电逻辑
            if (m_abnorTemperature2 < std::abs(tem2)) {
                isCritical = true;
                ui->zsd_tem2->setStyleSheet("QLabel { background-color: red; border-radius: 15px; border: 2px solid black; }");
                if(temlogsign2)
                {
                    qWarning()<<"电机2当前温度为"+QString::number(tem2)+",设备停机下使能";
                    logWindow->addLog("Warning","电机2当前温度为"+QString::number(tem2)+",设备停机下使能");
                    temlogsign2=false;
                }

            }
            // 如果触发断电条件
            if (isCritical) {
                ui->test_status3->setStyleSheet("color: red;");
                ui->test_status3->setText("停机");
                stopstop();
                acsc_DisableAll(hComm, nullptr);
                acsc_KillAll(hComm, nullptr);
                return;  // 直接返回避免后续状态覆盖
            }
            // 未触发断电但存在告警
            ui->test_status3->setStyleSheet("color: yellow;");
            ui->test_status3->setText("告警");
            ui->zsd_tem1->setStyleSheet((m_warnTemperature < std::abs(tem1)) ?
                                            "QLabel { background-color: yellow; border-radius: 15px; border: 2px solid black; }" :
                                            "QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            ui->zsd_tem2->setStyleSheet((m_warnTemperature2 < std::abs(tem2)) ?
                                            "QLabel { background-color: yellow; border-radius: 15px; border: 2px solid black; }" :
                                            "QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            if(temlogsign3)
            {
                qWarning()<<"电机1当前温度为"+QString::number(tem1);
                qWarning()<<"电机2当前温度为"+QString::number(tem2);
                qWarning()<<"温度过高";
                logWindow->addLog("Warning","电机1当前温度为"+QString::number(tem1));
                logWindow->addLog("Warning","电机2当前温度为"+QString::number(tem2));
                logWindow->addLog("Warning","温度过高");
                temlogsign3=false;
            }
        }
        else {
            // 完全正常状态
            ui->test_status3->setStyleSheet("color: blue;");
            ui->test_status3->setText("正常");
            ui->zsd_tem1->setStyleSheet("QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            ui->zsd_tem2->setStyleSheet("QLabel { background-color: green; border-radius: 15px; border: 2px solid black; }");
            if((!temlogsign1)||(!temlogsign2)||(!temlogsign3))
            {
                qInfo()<<"电机温度恢复正常";
                logWindow->addLog("Info","电机温度恢复正常");
                temlogsign1=true;
                temlogsign2=true;
                temlogsign3=true;
            }
        }

        //这里判断水流量
        if((m_shuiliuliang1>Flowrate1)||(m_shuiliuliang2>Flowrate2)){
            if(m_shuiliuliang1>Flowrate1){
                //disableall2();
                stopstop();
                acsc_DisableAll(hComm,NULL);
                acsc_KillAll(hComm, NULL);
                ui->label_50->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");

                ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: red; ""border-radius: 15px; " "border: 2px solid black; ""}");
                if(flowlogsign1)
                {
                    qWarning()<<"电机1当前水流量为"+QString::number(Flowrate1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前水流量为"+QString::number(Flowrate1)+"设备停机下使能");
                    flowlogsign1=false;
                }
            }else
            {
                ui->label_50->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
                ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
                if(!flowlogsign1)
                {
                    qInfo()<<"电机1当前水流量恢复正常";
                    logWindow->addLog("Info","电机1当前水流量恢复正常");
                    flowlogsign1=true;
                }
            }

            if(m_shuiliuliang2>Flowrate2){
                //disableall2();
                stopstop();
                acsc_DisableAll(hComm,NULL);
                acsc_KillAll(hComm, NULL);
                ui->label_55->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");
                ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: red; ""border-radius: 15px; " "border: 2px solid black; ""}");
                if(flowlogsign2)
                {
                    qWarning()<<"电机2当前水流量为"+QString::number(Flowrate2)+"设备停机下使能";
                    logWindow->addLog("Warning","电机2当前水流量为"+QString::number(Flowrate2)+"设备停机下使能");
                    flowlogsign2=false;
                }
            }
            else
            {
                ui->label_55->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
                ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
                if(!flowlogsign2)
                {
                    qInfo()<<"电机2当前水流量恢复正常";
                    logWindow->addLog("Info","电机2当前水流量恢复正常");
                    flowlogsign2=true;
                }
            }
        }else
        {
            ui->label_50->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
            ui->label_55->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
            ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");

            ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
        }





        //这里判断气压
        //        if((m_atm1>t_qiya_1)||(m_atm2>t_qiya_2)){
        //            if(m_atm1>t_qiya_1){
        //                //disableall2();
        //                stopstop();
        //                acsc_DisableAll(hComm,NULL);
        //                acsc_KillAll(hComm, NULL);
        //                ui->label_63->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");
        //                ui->zsd_air1->setStyleSheet("QLabel { ""background-color: red; ""border-radius: 15px; " "border: 2px solid black; ""}");
        //                if(atmlogsign1)
        //                {
        //                    qWarning()<<"电机1当前气压为"+QString::number(t_qiya_1)+",设备停机下使能";
        //                    logWindow->addLog("Warning","电机1当前气压为"+QString::number(t_qiya_1)+",设备停机下使能");
        //                    atmlogsign1=false;
        //                }
        //            }else
        //            {
        //                ui->label_63->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        //                if(!atmlogsign1)
        //                {
        //                    qInfo()<<"电机1当前气压恢复正常";
        //                    logWindow->addLog("Info","电机1当前气压恢复正常");
        //                    atmlogsign1=true;
        //                }
        //            }

        //            if(m_atm2>t_qiya_2){
        //                //disableall2();
        //                stopstop();
        //                acsc_DisableAll(hComm,NULL);
        //                acsc_KillAll(hComm, NULL);
        //                ui->label_65->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");
        //                ui->zsd_air2->setStyleSheet("QLabel {"" background-color: red;""border-radius: 15px; " "border: 2px solid black; ""}");
        //                if(atmlogsign2)
        //                {
        //                    qWarning()<<"电机2当前气压为"+QString::number(t_qiya_2)+",设备停机下使能";
        //                    logWindow->addLog("Warning","电机2当前气压为"+QString::number(t_qiya_2)+",设备停机下使能");
        //                    atmlogsign2=false;
        //                }
        //            }
        //            else
        //            {
        //                ui->label_65->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        //                if(!atmlogsign2)
        //                {
        //                    qInfo()<<"电机2当前气压恢复正常";
        //                    logWindow->addLog("Info","电机2当前气压恢复正常");
        //                    atmlogsign2=true;
        //                }
        //            }
        //        }else
        //        {
        //            ui->label_63->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        //            ui->zsd_air1->setStyleSheet("QLabel {"" background-color: green;""border-radius: 15px; " "border: 2px solid black; ""}");
        //            ui->label_65->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        //            ui->zsd_air2->setStyleSheet("QLabel {"" background-color: green;""border-radius: 15px; " "border: 2px solid black; ""}");
        //        }
    }
    else
    {
        ui->test_status4->setStyleSheet("color:blue;");
        ui->test_status4->setText("正常");
        ui->zsd_current1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->zsd_current2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");        //恢复正常


        ui->test_status3->setStyleSheet("color:blue;");
        ui->test_status3->setText("正常");
        ui->zsd_tem1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->zsd_tem2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");
        ui->label_50->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        ui->label_55->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");

        ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 15px; " "border: 2px solid black; ""}");

        ui->label_63->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");

        ui->zsd_air1->setStyleSheet("QLabel {"" background-color: green;""border-radius: 15px; " "border: 2px solid black; ""}");

        ui->label_65->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");

        ui->zsd_air2->setStyleSheet("QLabel {"" background-color: green;""border-radius: 15px; " "border: 2px solid black; ""}");
    }
}

void Widget::on_checkBox_start_tem1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph5_1->setVisible(false);mPlot1->replot();
    }else{
        mGraph5_5->setVisible(false);
        mGraph5_6->setVisible(false);
        mGraph5_7->setVisible(false);
        mGraph5_8->setVisible(false);
        ui->checkBox_start_tem_old1->setChecked(false);
        ui->checkBox_start_tem_old2->setChecked(false);
        ui->checkBox_start_current_old1->setChecked(false);
        ui->checkBox_start_current_old2->setChecked(false);
        mGraph5_1->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_tem2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph5_2->setVisible(false);mPlot1->replot();
    }else{
        mGraph5_5->setVisible(false);
        mGraph5_6->setVisible(false);
        mGraph5_7->setVisible(false);
        mGraph5_8->setVisible(false);
        ui->checkBox_start_tem_old1->setChecked(false);
        ui->checkBox_start_tem_old2->setChecked(false);
        ui->checkBox_start_current_old1->setChecked(false);
        ui->checkBox_start_current_old2->setChecked(false);
        mGraph5_2->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_current1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph5_3->setVisible(false);mPlot1->replot();
    }else{
        mGraph5_5->setVisible(false);
        mGraph5_6->setVisible(false);
        mGraph5_7->setVisible(false);
        mGraph5_8->setVisible(false);
        ui->checkBox_start_tem_old1->setChecked(false);
        ui->checkBox_start_tem_old2->setChecked(false);
        ui->checkBox_start_current_old1->setChecked(false);
        ui->checkBox_start_current_old2->setChecked(false);
        mGraph5_3->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_current2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph5_4->setVisible(false);mPlot1->replot();
    }else{
        mGraph5_5->setVisible(false);
        mGraph5_6->setVisible(false);
        mGraph5_7->setVisible(false);
        mGraph5_8->setVisible(false);
        ui->checkBox_start_tem_old1->setChecked(false);
        ui->checkBox_start_tem_old2->setChecked(false);
        ui->checkBox_start_current_old1->setChecked(false);
        ui->checkBox_start_current_old2->setChecked(false);
        mGraph5_4->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_tem_old1_stateChanged(int arg1)
{
    QVector<double>  a;
    int pointIdx = 0;
    a.reserve(3000);
    a.clear();
    mData5_5.clear();
    mGraph5_5->data()->clear();
    if(arg1==0)
    {
        mGraph5_5->setVisible(false);
        mPlot5->replot();
    }else{
        mGraph5_1->setVisible(false);
        mGraph5_2->setVisible(false);
        mGraph5_3->setVisible(false);
        mGraph5_4->setVisible(false);
        ui->checkBox_start_tem1->setChecked(false);
        ui->checkBox_start_tem2->setChecked(false);
        ui->checkBox_start_current1->setChecked(false);
        ui->checkBox_start_current2->setChecked(false);
        mPlot5->replot();



        refresh_olddata();
        for (int i = 0; i < result.size(); i++) {          // i: 文件索引
            for (int j = 0; j < result[i].size(); j++) {   // j: 行索引

                mData5_5.append(result[i][j][0]);
                a.append(pointIdx++);
            }
        }
        qDebug()<<a.size();
        qDebug()<<mData5_5.size();
        mPlot5->xAxis->setRange(-100, 2200);
        mPlot5->yAxis->setRange(-50, 150);
        mGraph5_5->setData(a, mData5_5);
        mGraph5_5->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_tem_old2_stateChanged(int arg1)
{
    QVector<double>  a;
    int pointIdx = 0;
    a.reserve(3000);
    a.clear();
    mData5_6.clear();
    mGraph5_6->data()->clear();
    if(arg1==0)
    {
        mGraph5_6->setVisible(false);
        mPlot5->replot();
    }else{
        mGraph5_1->setVisible(false);
        mGraph5_2->setVisible(false);
        mGraph5_3->setVisible(false);
        mGraph5_4->setVisible(false);
        ui->checkBox_start_tem1->setChecked(false);
        ui->checkBox_start_tem2->setChecked(false);
        ui->checkBox_start_current1->setChecked(false);
        ui->checkBox_start_current2->setChecked(false);
        mPlot5->replot();



        refresh_olddata();
        for (int i = 0; i < result.size(); i++) {          // i: 文件索引
            for (int j = 0; j < result[i].size(); j++) {   // j: 行索引

                mData5_6.append(result[i][j][1]);
                a.append(pointIdx++);
            }
        }
        mPlot5->xAxis->setRange(-100, 2200);
        mPlot5->yAxis->setRange(-50, 150);
        mGraph5_6->setData(a, mData5_6);
        mGraph5_6->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_current_old1_stateChanged(int arg1)
{
    QVector<double>  a;
    int pointIdx = 0;
    a.reserve(3000);
    a.clear();
    mData5_7.clear();
    mGraph5_7->data()->clear();
    if(arg1==0)
    {
        mGraph5_7->setVisible(false);
        mPlot5->replot();
    }else{
        mGraph5_1->setVisible(false);
        mGraph5_2->setVisible(false);
        mGraph5_3->setVisible(false);
        mGraph5_4->setVisible(false);
        ui->checkBox_start_tem1->setChecked(false);
        ui->checkBox_start_tem2->setChecked(false);
        ui->checkBox_start_current1->setChecked(false);
        ui->checkBox_start_current2->setChecked(false);
        mPlot5->replot();



        refresh_olddata();
        for (int i = 0; i < result.size(); i++) {          // i: 文件索引
            for (int j = 0; j < result[i].size(); j++) {   // j: 行索引

                mData5_7.append(result[i][j][2]);
                a.append(pointIdx++);
            }
        }
        mPlot5->xAxis->setRange(-100, 2200);
        mPlot5->yAxis->setRange(-50, 150);
        mGraph5_7->setData(a, mData5_7);
        mGraph5_7->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_start_current_old2_stateChanged(int arg1)
{
    QVector<double>  a;
    int pointIdx = 0;
    a.reserve(3000);
    a.clear();
    mData5_8.clear();
    mGraph5_8->data()->clear();
    if(arg1==0)
    {
        mGraph5_8->setVisible(false);
        mPlot5->replot();
    }else{
        mGraph5_1->setVisible(false);
        mGraph5_2->setVisible(false);
        mGraph5_3->setVisible(false);
        mGraph5_4->setVisible(false);
        ui->checkBox_start_tem1->setChecked(false);
        ui->checkBox_start_tem2->setChecked(false);
        ui->checkBox_start_current1->setChecked(false);
        ui->checkBox_start_current2->setChecked(false);
        mPlot5->replot();



        refresh_olddata();
        for (int i = 0; i < result.size(); i++) {          // i: 文件索引
            for (int j = 0; j < result[i].size(); j++) {   // j: 行索引

                mData5_8.append(result[i][j][3]);
                a.append(pointIdx++);
            }
        }
        mPlot5->xAxis->setRange(-100, 2200);
        mPlot5->yAxis->setRange(-50, 150);
        mGraph5_8->setData(a, mData5_8);
        mGraph5_8->setVisible(true);
        mPlot5->replot();
    }
}

void Widget::on_checkBox_test1_pos1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph1_1->setVisible(false);mPlot1->replot();
    }else{
        mGraph1_1->setVisible(true);
        mPlot1->replot();
    }
}

void Widget::on_checkBox_test1_pos2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph1_2->setVisible(false);mPlot1->replot();
    }else{
        mGraph1_2->setVisible(true);
        mPlot1->replot();
    }
}

void Widget::on_checkBox_test2_vel1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph2_1->setVisible(false);mPlot1->replot();
    }else{
        mGraph2_1->setVisible(true);
        mPlot2->replot();
    }
}

void Widget::on_checkBox_test2_vel2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph2_2->setVisible(false);mPlot1->replot();
    }else{
        mGraph2_2->setVisible(true);
        mPlot2->replot();
    }
}

void Widget::on_checkBox_test2_acc1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph2_3->setVisible(false);mPlot1->replot();
    }else{
        mGraph2_3->setVisible(true);
        mPlot2->replot();
    }
}

void Widget::on_checkBox_test2_acc2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph2_4->setVisible(false);mPlot1->replot();
    }else{
        mGraph2_4->setVisible(true);
        mPlot2->replot();
    }
}

void Widget::on_checkBox_test3_tem1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph3_1->setVisible(false);
        mPlot1->replot();
    }else{
        mGraph3_1->setVisible(true);
        mPlot3->replot();
    }
}

void Widget::on_checkBox_test3_tem2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph3_2->setVisible(false);
        mPlot1->replot();
    }else{
        mGraph3_2->setVisible(true);
        mPlot3->replot();
    }
}

void Widget::on_checkBox_test4_current1_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph4_1->setVisible(false);
        mPlot1->replot();
    }else{
        mGraph4_1->setVisible(true);
        mPlot4->replot();
    }
}

void Widget::on_checkBox_test4_current2_stateChanged(int arg1)
{
    if(arg1==0)
    {
        mGraph4_2->setVisible(false);
        mPlot1->replot();
    }else{
        mGraph4_2->setVisible(true);
        mPlot4->replot();
    }
}

void Widget::on_dingwei5_clicked()
{
    double yMax;
    double yMin;
    double xMax;
    double xMin;
    QDialog dialog(this);
    dialog.setWindowTitle("设置坐标范围");  // 添加对话框标题

    QGridLayout *mainLayout = new QGridLayout(&dialog);

    mainLayout->addWidget(new QLabel("X轴范围:", &dialog), 0, 0);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    mainLayout->addWidget(xMinEdit, 0, 1);
    mainLayout->addWidget(new QLabel("~", &dialog), 0, 2);
    mainLayout->addWidget(xMaxEdit, 0, 3);

    mainLayout->addWidget(new QLabel("Y轴范围:", &dialog), 1, 0);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    mainLayout->addWidget(yMinEdit, 1, 1);
    mainLayout->addWidget(new QLabel("~", &dialog), 1, 2);
    mainLayout->addWidget(yMaxEdit, 1, 3);

    QPushButton *okButton = new QPushButton("确定", &dialog);
    okButton->setFixedHeight(40);
    okButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QFont font = okButton->font();
    font.setPointSize(12);
    okButton->setFont(font);
    okButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; }"
                           "QPushButton:hover { background-color: #45a049; }");  // 添加悬停效果
    mainLayout->addWidget(okButton, 2, 0, 1, 4);

    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        qDebug() << "X轴范围:" << xMin << "~" << xMax;
        qDebug() << "Y轴范围:" << yMin << "~" << yMax;

        dialog.accept();
    });

    if (dialog.exec() == QDialog::Accepted) {
        mPlot5->xAxis->setRange(xMin, xMax);
        mPlot5->yAxis->setRange(yMin, yMax);
        mPlot5->yAxis2->setRange(yMin, yMax);
        mPlot5->replot();
    }
}

void Widget::on_dingwei1_clicked()
{
    double yMax;
    double yMin;
    double xMax;
    double xMin;
    QDialog dialog(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    QObject::connect(okButton, &QPushButton::clicked, [&]() {

        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept();
    });

    dialog.exec();
    mPlot1->xAxis->setRange(xMin, xMax);
    mPlot1->yAxis->setRange(yMin, yMax);
    mPlot1->replot();
}

void Widget::on_dingwei2_clicked()
{
    double yMax;
    double yMin;
    double xMax;
    double xMin;
    QDialog dialog(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept();
    });

    dialog.exec();
    mPlot2->xAxis->setRange(xMin, xMax);
    mPlot2->yAxis->setRange(yMin, yMax);
    mPlot2->replot();
}

void Widget::on_dingwei3_clicked()
{
    double yMax;
    double yMin;
    double xMax;
    double xMin;
    QDialog dialog(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    QObject::connect(okButton, &QPushButton::clicked, [&]() {

        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept();
    });


    dialog.exec();
    mPlot3->xAxis->setRange(xMin, xMax);
    mPlot3->yAxis->setRange(yMin, yMax);
    mPlot3->replot();
}

void Widget::on_dingwei4_clicked()
{
    double yMax;
    double yMin;
    double xMax;
    double xMin;
    QDialog dialog(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    QObject::connect(okButton, &QPushButton::clicked, [&]() {

        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept();
    });

    dialog.exec();
    mPlot4->xAxis->setRange(xMin, xMax);
    mPlot4->yAxis->setRange(yMin, yMax);
    mPlot4->replot();
}

void Widget::handleWaveDataUpdate()
{
    //ui->above_num->setText(QString::number(lastrecyclenum));
    QString a=QString::number(allrecyclenum);
    ui->above_all_num->setText(a);
    writeTestNum(a);

    QList<QString> datalist;
    QDateTime currentTime1 = QDateTime::currentDateTime(); //当前时间
    quint8 hour1 = currentTime1.time().hour();			   //时
    quint8 minute1 = currentTime1.time().minute();          //分
    quint8 second1 = currentTime1.time().second();          //秒
    qint64 milliseconds1 = currentTime1.time().msec();
    QString formattedTime = QString("%1.%2.%3.%4")
            .arg(hour1, 2, 10, QChar('0'))
            .arg(minute1, 2, 10, QChar('0'))
            .arg(second1, 2, 10, QChar('0'))
            .arg(milliseconds1, 3, 10, QChar('0'));
    datalist.append(formattedTime);

    //几组最大数据
    if(max_tem1==-0.111||max_tem2==-0.111||max_current1==-0.111||max_current1==-0.111)
    {return;}
    else{
        datalist.append(QString::number(max_tem1));
        datalist.append(QString::number(max_tem2));
        datalist.append(QString::number(max_current1));
        datalist.append(QString::number(max_current2));
        if (issaveincsv&&ui->set_save_key->isChecked())
        {
            appendRowsToCsv(xunhuanfilename,datalist);
        }

        static double timeCount2 = 0;
        //    const double timeStep = 0.02;
        //    double currentTime = timeCount * timeStep;
        mnumData.append(timeCount2);

        mData3_1.append(max_tem1);
        mData3_2.append(max_tem2);
        mData4_1.append(max_current1);
        mData4_2.append(max_current2);
        mData5_1.append(max_tem1);
        mData5_2.append(max_tem2);
        mData5_3.append(max_current1);
        mData5_4.append(max_current2);

        max_tem1=-0.111;
        max_tem2=-0.111;
        max_current1=-0.111;
        max_current2=-0.111;






        if (mnumData.size() > 2000) {
            mnumData.removeFirst();
            mData3_1.removeFirst();
            mData3_2.removeFirst();
            mData4_1.removeFirst();
            mData4_2.removeFirst();
            mData5_1.removeFirst();
            mData5_2.removeFirst();
            mData5_3.removeFirst();
            mData5_4.removeFirst();
        }



        mGraph3_1->setData(mnumData, mData3_1);
        mGraph3_2->setData(mnumData, mData3_2);
        mGraph4_1->setData(mnumData, mData4_1);
        mGraph4_2->setData(mnumData, mData4_2);
        mGraph5_1->setData(mnumData, mData5_1);
        mGraph5_2->setData(mnumData, mData5_2);
        mGraph5_3->setData(mnumData, mData5_3);
        mGraph5_4->setData(mnumData, mData5_4);


        mPlot3->replot();
        mPlot4->replot();
        mPlot5->replot();

        timeCount2 += 1.0;

    }






















}

void Widget::updatexunhuanTime()
{
    QTime current = QTime::currentTime();
    int currentMsecs = this->xunhuanstarttime.msecsTo(current);

    QStringList oldParts = oldtime.split('.');
    int oldDays = 0;
    int oldMsecs = 0;

    if (oldParts.size() == 2) {
        oldDays = oldParts[0].toInt();

        QTime oldTime = QTime::fromString(oldParts[1], "hh:mm:ss:zzz");
        if (oldTime.isValid()) {
            oldMsecs = oldTime.msecsSinceStartOfDay();
        }
    }

    int totalMsecs = currentMsecs + oldMsecs;
    int totalDays = oldDays + (totalMsecs / (1000 * 60 * 60 * 24));
    totalMsecs %= (1000 * 60 * 60 * 24);

    QTime showTime = QTime(0, 0, 0, 0).addMSecs(totalMsecs);
    QString showStr = QString::number(totalDays) + "." + showTime.toString("hh:mm:ss:zzz");

    ui->above_time->setText(showStr);
    writeTestTime(showStr);
}

void Widget::checkcsv()
{
    QMutexLocker locker(&m_filenameMutex);
    QFileInfo fileInfo2(logname);

    if (fileInfo2.exists())
    {
        qint64 fileSize = fileInfo2.size();
        if(fileSize>logsize)
        {

            logname=createLog();
        }

    }

    QFileInfo fileInfo(filename);

    if (fileInfo.exists())
    {
        qint64 fileSize = fileInfo.size();
        csvsize=(ui->line_set_csv_size->text().toInt())*1000;
        if(fileSize>csvsize)
        {
            checkcsv_timer->stop();
            issaveincsv=0;
            int a=ui->line_set_save_s->text().toDouble();
            QTimer::singleShot(a*60 * 1000, this, [this]() {
                filename = createcsv();
                xunhuanfilename=createxunhuancsv();
                issaveincsv=1;
                {
                    QMutexLocker locker(&m_filenameMutex);
                    openCsvFile(filename);
                }
                checkcsv_timer->start(1000);
            });
        }

    }

}

static double getAInvalue(HANDLE hComm, char* str)
{
    if(hComm == ACSC_INVALID)
    {
        return 0.0;
    }
    char buf[100];
    int Received = 0;
    int ret = acsc_Transaction(hComm,str,strlen(str),buf,100,&Received,NULL);
    if(ret != 0 || Received <=0)
    {
        return 0.0;
    }
    buf[Received] = '\0';
    QString dataStr = QString::fromUtf8(buf).trimmed().replace("\r", "");
    bool ok;
    double value = dataStr.toDouble(&ok);
    if(!ok)
    {
        return 0.0;
    }
    return value;
}


//double Widget::getAInvalue(char* str)
//{
//    char* cmd=str;
//    char buf[100];
//    int Received;
//    acsc_Transaction(hComm,cmd,strlen(cmd),buf,100,&Received,NULL);
//    //qDebug()<<"Received = "<<Received;
//    buf[Received] = '\0';
//    QString dataStr = QString::fromUtf8(buf).trimmed().replace("\r", "");
//    // 尝试将清理后的字符串转换为 double 类型
//    bool ok;
//    double value = dataStr.toDouble(&ok);
//    return value;
//}

//页眉切换界
void Widget::on_tabWidget_currentChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_2->setCurrentIndex(0);
        break;
    case 1:
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_2->setCurrentIndex(1);break;
    case 2:
        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_2->setCurrentIndex(2);break;
    case 3:
        ui->stackedWidget->setCurrentIndex(1);break;

    }
    return;
}


void Widget::mousePressEvent(QMouseEvent *event)
{
    QWidget *topLevelWindow = window();
    if (!topLevelWindow->isFullScreen() && event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPosition = event->globalPos() - topLevelWindow->pos();
        event->accept();
    }
    QWidget::mousePressEvent(event);
}


void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget *topLevelWindow = window();
    if (!topLevelWindow->isFullScreen() && m_isDragging && event->buttons() & Qt::LeftButton) {
        const QPoint newPosition = event->globalPos() - m_dragStartPosition;
        topLevelWindow->move(newPosition);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!window()->isFullScreen() && event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
    QWidget::mouseReleaseEvent(event);
}

void Widget::on_system_min_clicked()
{
    window()->showMinimized();
}

void Widget::on_system_max_clicked()
{
    QWidget *topLevelWindow = window();
    if (topLevelWindow->windowState() & Qt::WindowFullScreen) {
        topLevelWindow->showNormal();
        QIcon icon(":/image/restore.png");
        ui->system_max->setIcon(icon);
    } else {
        topLevelWindow->showFullScreen();
        QIcon icon(":/image/max.png");
        ui->system_max->setIcon(icon);
    }
}

void Widget::on_system_exit_clicked()
{
    if (logWindow) {
        logWindow->close();
    }
    window()->close();
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    if(index==0)
    {
        final_axis=ACSC_AXIS_0;
    }else if(index==1)
    {
        final_axis=ACSC_AXIS_1;
    } else if(index==2)
    {
        final_axis=2;
    }
}

//开始界面急停
void Widget::on_start_stop_clicked()
{
    t_addlog(1,"急停");
    flag=false;
    m_recycleMoveNum=0;
    future.cancel();
    acsc_KillAll(hComm, NULL);
}

void Widget::stopstop()
{
    flag=false;
    m_recycleMoveNum=0;
    future.cancel();
    acsc_KillAll(hComm, NULL);
}

//测试界面设置
void Widget::on_test_set_clicked()
{
    t_addlog(1,"电机参数设置");
    //设置速度
    bool ok_vel;
    int vel=ui->line_test_vel->text().toInt(&ok_vel);
    if(!ok_vel)
    {
        QMessageBox::warning(this,"非法输入","请输入正确的速度");
        return;
    }
    int prot_vel=ui->line_set_prot_vel->text().toInt(&ok_vel);
    if(!ok_vel)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的速度阈值");
        return;
    }
    if(vel>prot_vel)
    {
        if(final_axis==2)
        {
            acsc_SetVelocity(hComm,ACSC_AXIS_0,prot_vel , NULL );
            acsc_SetVelocity(hComm,ACSC_AXIS_1,prot_vel , NULL );
        }else{
            acsc_SetVelocity(hComm,final_axis,prot_vel , NULL );
        }
        ui->line_test_vel->setText(QString::number(prot_vel));
    }else
    {
        if(final_axis==2)
        {
            acsc_SetVelocity(hComm,ACSC_AXIS_0,vel , NULL );
            acsc_SetVelocity(hComm,ACSC_AXIS_1,vel , NULL );
        }else{
            acsc_SetVelocity(hComm,final_axis,vel , NULL );
        }
        ui->line_test_vel->setText(QString::number(vel));
    }
    //设置加速度减速度
    bool ok_accdec;
    int accdec=ui->line_test_accdec->text().toInt(&ok_accdec);
    if(!ok_accdec)
    {
        QMessageBox::warning(this,"非法输入","请输入正确的加速度/减速度");
        return;
    }
    int prot_accdec=ui->line_set_prot_accdec->text().toInt(&ok_accdec);
    if(!ok_accdec)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的加减速度阈值");
        return;
    }
    if(accdec>prot_accdec)//150000
    {
        if(final_axis==2)
        {
            acsc_SetAcceleration(hComm,ACSC_AXIS_0,prot_accdec , NULL );
            acsc_SetAcceleration(hComm,ACSC_AXIS_1,prot_accdec , NULL );

            acsc_SetDeceleration(hComm,ACSC_AXIS_0,prot_accdec , NULL );
            acsc_SetDeceleration(hComm,ACSC_AXIS_1,prot_accdec , NULL );
        }else{
            acsc_SetAcceleration(hComm,final_axis,prot_accdec , NULL );

            acsc_SetDeceleration(hComm,final_axis,prot_accdec , NULL );
        }
        ui->line_test_accdec->setText(QString::number(prot_accdec));

    }
    else
    {
        if(final_axis==2)
        {
            acsc_SetAcceleration(hComm,ACSC_AXIS_0,accdec , NULL );
            acsc_SetAcceleration(hComm,ACSC_AXIS_1,accdec , NULL );
            acsc_SetDeceleration(hComm,ACSC_AXIS_0,accdec , NULL );
            acsc_SetDeceleration(hComm,ACSC_AXIS_1,accdec , NULL );
        }else{
            acsc_SetAcceleration(hComm,final_axis,accdec , NULL );
            acsc_SetDeceleration(hComm,final_axis,accdec , NULL );
        }
    }
}

//测试界面使能
void Widget::on_test_enable_clicked()
{
    t_addlog(1,"电机上使能");
    if(final_axis==2)
    {
        int Axes[] = { ACSC_AXIS_0,ACSC_AXIS_1, -1 };
        acsc_EnableM(hComm,Axes,NULL);
    }else{
        acsc_Enable(hComm,final_axis,NULL);
    }
}

//测试界面下使能
void Widget::on_test_disable_clicked()
{
    t_addlog(1,"电机下使能");
    if(final_axis==2)
    {
        acsc_DisableAll(hComm,NULL);
    }else{
        acsc_Disable(hComm,final_axis,NULL);
    }
}


void Widget::on_test_start_move_clicked()
{
    bool ok;
    if(ui->line_test_movenum->text().contains('/'))
    {
        QString a = ui->line_test_movenum->text();
        QString b = a.section('/', -1);
        bool ok9;
        int c=b.toInt(&ok9);
        if (!ok9) {
            QMessageBox::warning(this, "非法输入", "请正确输入循环次数");
            return;
        }
        cycle_num=c;
    }
    else
    {
        cycle_num = ui->line_test_movenum->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "非法输入", "请正确输入循环次数");
            return;
        }
    }
    cycle_pointA = ui->line_test_A->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "非法输入", "请正确输入行程点A");
        return;
    }
    cycle_pointB = ui->line_test_B->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "非法输入", "请正确输入行程点B");
        return;
    }
    cycle_waittime = ui->line_test_waittime->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "非法输入", "请正确输入等候时间");
        return;
    }
    int max=ui->line_set_prot_movemax->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的行程最大值");
        return;
    }
    int min=ui->line_set_prot_movemin->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的行程最小值");
        return;
    }
    if(cycle_pointA>max||cycle_pointA<min||cycle_pointB>max||cycle_pointB<min)
    {
        QString errorMsg = QString(
                    "行程超出运动范围阈值！\n\n"
                "当前值：\n"
                "  cycle_pointA = %1\n"
                "  cycle_pointB = %2\n"
                "  max = %3\n"
                "  min = %4\n\n"
                "超出情况：\n"
                "  %5%6%7%8"
            )
                .arg(cycle_pointA)
                .arg(cycle_pointB)
                .arg(max)
                .arg(min)
                .arg(cycle_pointA > max ? "  cycle_pointA > max (超出上限)\n" : "")
                .arg(cycle_pointA < min ? "  cycle_pointA < min (超出下限)\n" : "")
                .arg(cycle_pointB > max ? "  cycle_pointB > max (超出上限)\n" : "")
                .arg(cycle_pointB < min ? "  cycle_pointB < min (超出下限)\n" : "");
        QMessageBox::warning(this, "超范围", errorMsg);
        return ;
    }

    t_addlog(1,"开始循环运动");
    startcyclenum=cycle_num;

    int l_cycle_num = cycle_num;
    int l_pointA = cycle_pointA;
    int l_pointB = cycle_pointB;
    int l_wait = cycle_waittime;
    int l_axis = final_axis;
    double l_oldspeed = ui->line_test_vel->text().toDouble();
    double l_oldacc_dec = ui->line_test_accdec->text().toDouble();
    HANDLE localHcomm = hComm;


    ui->test_start_move->setEnabled(false);

    if (future.isRunning()) {
        flag = false;
        future.waitForFinished();
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1 };
        acsc_HaltM(localHcomm,Axes,nullptr);
    }

    m_recycleMoveNum=0;
    if(!xunhuantimer->isActive())
    {
        if(xunhuanpausetime.isNull()){
            xunhuanstarttime=QTime::currentTime();
        }
        else{
            QTime cut = QTime::currentTime();
            int t = xunhuanpausetime.msecsTo(cut);
            xunhuanstarttime=xunhuanstarttime.addMSecs(t);;
        }
        xunhuantimer->start(10);
    }
    flag=true;

    auto func=[=](){
        if(l_axis==2){
            acsc_SetVelocity(localHcomm,ACSC_AXIS_0,100 , nullptr );
            acsc_SetVelocity(localHcomm,ACSC_AXIS_1,100 , nullptr );
        }else{
            acsc_SetVelocity(localHcomm,l_axis,100 , nullptr );
        }
        if(l_axis==2){
            acsc_SetAcceleration(localHcomm,ACSC_AXIS_0,1000 , nullptr );
            acsc_SetAcceleration(localHcomm,ACSC_AXIS_1,1000 , nullptr );
            acsc_SetDeceleration(localHcomm,ACSC_AXIS_0,1000 , nullptr );
            acsc_SetDeceleration(localHcomm,ACSC_AXIS_1,1000 , nullptr );
        }else{
            acsc_SetAcceleration(localHcomm,l_axis,1000 , nullptr );
            acsc_SetDeceleration(localHcomm,l_axis,1000 , nullptr );
        }

        if(l_axis==2){
            acsc_ToPoint(localHcomm,0,0,-100,nullptr);
            acsc_ToPoint(localHcomm,0,1,-100,nullptr);
        }else{
            acsc_ToPoint(localHcomm,0,l_axis,-100,nullptr);
        }
        if(l_axis==2){
            acsc_WaitLogicalMotionEnd(localHcomm,0,30000);
            acsc_WaitLogicalMotionEnd(localHcomm,1,30000);
        }else{
            acsc_WaitLogicalMotionEnd(localHcomm,l_axis,30000);
        }

        if(l_axis==2){
            acsc_SetVelocity(localHcomm,ACSC_AXIS_0,l_oldspeed , nullptr );
            acsc_SetVelocity(localHcomm,ACSC_AXIS_1,l_oldspeed , nullptr );
        }else{
            acsc_SetVelocity(localHcomm,l_axis,l_oldspeed , nullptr );
        }
        if(l_axis==2){
            acsc_SetAcceleration(localHcomm,ACSC_AXIS_0,l_oldacc_dec , nullptr );
            acsc_SetAcceleration(localHcomm,ACSC_AXIS_1,l_oldacc_dec , nullptr );
            acsc_SetDeceleration(localHcomm,ACSC_AXIS_0,l_oldacc_dec , nullptr );
            acsc_SetDeceleration(localHcomm,ACSC_AXIS_1,l_oldacc_dec , nullptr );
        }else{
            acsc_SetAcceleration(localHcomm,l_axis,l_oldacc_dec , nullptr );
            acsc_SetDeceleration(localHcomm,l_axis,l_oldacc_dec , nullptr );
        }

        while (flag && m_recycleMoveNum < l_cycle_num)
        {
            if(l_axis==2)
            {
                int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1};
                double aa=l_pointA;
                double Points[] = {aa, aa};
                acsc_ToPointM(localHcomm,0,Axes,Points,nullptr );
            }else if(l_axis==0){
                acsc_ToPoint(localHcomm,0,l_axis,l_pointA,nullptr);
            }else if(l_axis==1){
                acsc_ToPoint(localHcomm,1,l_axis,l_pointA,nullptr);
            }
            if(l_axis==2)
            {
                acsc_WaitLogicalMotionEnd(localHcomm,0,30000);
                acsc_WaitLogicalMotionEnd(localHcomm,1,30000);
            }else{
                acsc_WaitLogicalMotionEnd(localHcomm,l_axis,30000);
            }

            int totalMs = l_wait;
            const int chunk = 20;
            while(totalMs>0 && flag)
            {
                int cur = qMin(chunk,totalMs);
                QThread::msleep(cur);
                totalMs -= cur;
            }
            if(!flag) break;

            if(l_axis==2)
            {
                int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1};
                double aa=l_pointB;
                double Points[] = {aa, aa};
                acsc_ToPointM(localHcomm,0,Axes,Points,nullptr );
            }else if(l_axis==0){
                acsc_ToPoint(localHcomm,0,l_axis,l_pointB,nullptr);
            }else if(l_axis==1){
                acsc_ToPoint(localHcomm,1,l_axis,l_pointB,nullptr);
            }
            if(l_axis==2)
            {
                acsc_WaitLogicalMotionEnd(localHcomm,0,30000);
                acsc_WaitLogicalMotionEnd(localHcomm,1,30000);
            }else{
                acsc_WaitLogicalMotionEnd(localHcomm,l_axis,30000);
            }

            totalMs = l_wait;
            while(totalMs>0 && flag)
            {
                int cur = qMin(chunk,totalMs);
                QThread::msleep(cur);
                totalMs -= cur;
            }
            if(!flag) break;

            emit updateWaveData();
            m_recycleMoveNum++;
            lastrecyclenum=m_recycleMoveNum;
            allrecyclenum++;

            QMetaObject::invokeMethod(this,[=](){
                ui->line_test_movenum->setText(QString::number(lastrecyclenum)+"/"+QString::number(startcyclenum));
            },Qt::QueuedConnection);
        }

        //循环结束，切回UI线程执行停止逻辑
        QMetaObject::invokeMethod(this,[=](){
            xunhuantimer->stop();
            xunhuanpausetime=QTime::currentTime();
            m_recycleMoveNum=0;
            on_test_stop2_clicked();
        },Qt::QueuedConnection);
    };

    future = QtConcurrent::run(func);
}
//测试界面停止1
void Widget::on_test_stop2_clicked()
{
    t_addlog(1,"停止运动");
    ui->test_start_move->setEnabled(true);
    xunhuantimer->stop();
    if(final_axis==2)
    {
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1 };
        acsc_HaltM(hComm,Axes,NULL);
    }else{
        acsc_Halt(hComm,final_axis,NULL);
    }

    flag = false;

    m_recycleMoveNum=0;
    xunhuanpausetime=QTime::currentTime();

}

//测试界面正向Jog
void Widget::on_test_jog_pressed()
{
    t_addlog(1,"正向点动");
    if(final_axis==2)
    {
        double Velocity;
        acsc_GetVelocity(hComm,0,&Velocity, NULL );
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1, -1 };
        int Directions[] = { ACSC_POSITIVE_DIRECTION, ACSC_NEGATIVE_DIRECTION};
        acsc_JogM(hComm,0,Axes,Directions,Velocity,NULL);
    }else if(final_axis==0){
        double Velocity;
        acsc_GetVelocity(hComm,final_axis,&Velocity, NULL );
        acsc_Jog(hComm,0,final_axis,Velocity,NULL);
    }else if(final_axis==1){
        double Velocity;
        acsc_GetVelocity(hComm,final_axis,&Velocity, NULL );
        acsc_Jog(hComm,0,final_axis,-Velocity,NULL);
    }
}

//测试界面正向Jog
void Widget::on_test_jog_released()
{
    if(final_axis==2)
    {
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1 };
        acsc_HaltM(hComm,Axes,NULL);
    }else{acsc_Halt(hComm,final_axis,NULL);}
}

//测试界面反向Jog
void Widget::on_test_jog__pressed()
{
    t_addlog(1,"负向点动");
    if(final_axis==2)
    {
        double Velocity;
        acsc_GetVelocity(hComm,0,&Velocity, NULL );
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1, -1 };
        int Directions[] = { ACSC_NEGATIVE_DIRECTION, ACSC_POSITIVE_DIRECTION};
        acsc_JogM(hComm,0,Axes,Directions,Velocity,NULL);
    }else if(final_axis==0){
        double Velocity;
        acsc_GetVelocity(hComm,final_axis,&Velocity, NULL );
        acsc_Jog(hComm,0,final_axis,-Velocity,NULL);
    }else if(final_axis==1){
        double Velocity;
        acsc_GetVelocity(hComm,final_axis,&Velocity, NULL );
        acsc_Jog(hComm,0,final_axis,Velocity,NULL);
    }
}

//测试界面反向Jog
void Widget::on_test_jog__released()
{
    if(final_axis==2)
    {
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1 };
        acsc_HaltM(hComm,Axes,NULL);
    }else{acsc_Halt(hComm,final_axis,NULL);}
}

//测试界面绝对运动
void Widget::on_test_ptp_clicked()
{
    t_addlog(1,"绝对运动");
    bool ok;
    double location=ui->line_test_ptp->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请输入正确的绝对运行目标位置");
        return;
    }
    location=-(location+100);
    int max=ui->line_set_prot_movemax->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的行程最大值");
        return;
    }
    int min=ui->line_set_prot_movemin->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的行程最小值");
        return;
    }
    if(location>max||location<min)
    {
        QMessageBox::warning(this,"超范围","目标位置超出运动范围阈值");
        return ;
    }
    if(final_axis==2)
    {
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1};
        double Points[] = {location, location};
        acsc_ToPointM(hComm,0,Axes,Points,NULL );

    }else
    {
        acsc_ToPoint(hComm,0,final_axis,location,NULL);
    }

}

//测试界面相对运动
void Widget::on_test_ptpr_clicked()
{
    t_addlog(1,"相对运动");
    bool ok;
    double location=ui->line_test_ptpr->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请输入正确的相对运行距离");
        return;
    }
    int max=ui->line_set_prot_movemax->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的行程最大值");
        return;
    }
    int min=ui->line_set_prot_movemin->text().toInt(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,"非法输入","请在设置界面输入正确的行程最小值");
        return;
    }
    if(location>max||location<min)
    {
        QMessageBox::warning(this,"超范围","目标位置超出运动范围阈值");
        return ;
    }

    if (final_axis == 2)
    {
        acsc_ToPoint(hComm, ACSC_AMF_RELATIVE, 0, location, NULL);
        acsc_ToPoint(hComm, ACSC_AMF_RELATIVE, 1, location, NULL);
    }
    else
    {
        acsc_ToPoint(hComm, ACSC_AMF_RELATIVE, final_axis, location, NULL);
    }
}

//测试界面清除报错
void Widget::on_test_clear_error_clicked()
{
    t_addlog(1,"清除报错");
    int Axes[]={ACSC_AXIS_0,ACSC_AXIS_1,-1};
    acsc_FaultClearM( hComm,Axes,NULL );
}

//测试界面停止2
void Widget::on_test_stop_clicked()
{

    t_addlog(1,"停止运动");
    ui->test_start_move->setEnabled(true);
    xunhuantimer->stop();
    if(final_axis==2)
    {
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1 };
        acsc_HaltM(hComm,Axes,NULL); // 立刻急停电机，打断当前运动！
    }else{
        acsc_Halt(hComm,final_axis,NULL);
    }

    flag = false;   //通知循环线程退出
    // ===== 删除 future.waitForFinished(); 这一行！！不要UI线程阻塞等待！！=====

    m_recycleMoveNum=0;
    xunhuanpausetime=QTime::currentTime();
}

void Widget::on_btn_set_load_set_clicked()
{
    t_addlog(1,"软件参数加载");
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open XML File"), QDir::homePath(), tr("XML Files (*.xml)"));

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Operation Cancelled", "No file selected.");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", QString("Failed to open file for reading: %1").arg(filePath));
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        QMessageBox::critical(this, "Error", "Failed to parse XML file.");
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();

    if (root.tagName() != "Protect_Parameters") {
        QMessageBox::warning(this, "Invalid File", "请选择保护参数文件，而不是运动参数文件");
        return;
    }

    ui->spin_set_warn_tem1->setText(root.firstChildElement("warn_tem1").text());
    ui->spin_set_warn_tem2->setText(root.firstChildElement("warn_tem2").text());
    ui->spin_set_wrong_tem1->setText(root.firstChildElement("wrong_tem1").text());
    ui->spin_set_wrong_tem2->setText(root.firstChildElement("wrong_tem2").text());
    ui->spin_set_warn_current1->setText(root.firstChildElement("warn_current1").text());
    ui->spin_set_warn_current2->setText(root.firstChildElement("warn_current2").text());
    ui->spin_set_wrong_current1->setText(root.firstChildElement("wrong_current1").text());
    ui->spin_set_wrong_current2->setText(root.firstChildElement("wrong_current2").text());
    ui->spin_set_flow1->setText(root.firstChildElement("flow1").text());
    ui->spin_set_flow2->setText(root.firstChildElement("flow2").text());
    ui->spin_set_press1->setText(root.firstChildElement("press1").text());
    ui->spin_set_press2->setText(root.firstChildElement("press2").text());



    ui->line_set_prot_vel->setText(QString::number(root.firstChildElement("prot_vel").text().toDouble()));
    ui->line_set_prot_accdec->setText(QString::number(root.firstChildElement("prot_accdec").text().toDouble()));
    ui->line_set_prot_movemax->setText(QString::number(root.firstChildElement("prot_movemax").text().toDouble()));
    ui->line_set_prot_movemin->setText(QString::number(root.firstChildElement("prot_movemin").text().toDouble()));
    ui->line_set_csv_size->setText(QString::number(root.firstChildElement("csv_size").text().toDouble()));
    ui->line_set_log_size->setText(QString::number(root.firstChildElement("log_size").text().toDouble()));
    ui->line_set_save_s->setText(QString::number(root.firstChildElement("save_s").text().toDouble()));
    ui->combox_set_save_hz->setCurrentIndex(root.firstChildElement("save_hz").text().toDouble());
    ui->line_set_save_time->setText(QString::number(root.firstChildElement("save_time").text().toDouble()));
}

void Widget::on_btn_set_save_set_clicked()
{
    t_addlog(1,"软件参数保存");
    double warn_tem1=ui->spin_set_warn_tem1->text().toDouble();                       //告警温度
    double warn_tem2=ui->spin_set_warn_tem2->text().toDouble();                     //告警温度2
    double wrong_tem1=ui->spin_set_wrong_tem1->text().toDouble();                   //异常温度
    double wrong_tem2=ui->spin_set_wrong_tem2->text().toDouble();                  //异常温度2
    double warn_current1=ui->spin_set_warn_current1->text().toDouble();               //告警电流
    double warn_current2=ui->spin_set_warn_current2->text().toDouble();              //告警电流2
    double wrong_current1=ui->spin_set_wrong_current1->text().toDouble();            //异常电流1
    double wrong_current2=ui->spin_set_wrong_current2->text().toDouble();          //异常电流2
    double flow1=ui->spin_set_flow1->text().toDouble();             //流量阈值1
    double flow2=ui->spin_set_flow2->text().toDouble();              //流量阈值2
    double press1=ui->spin_set_press1->text().toDouble();                    //气压阈值1
    double press2=ui->spin_set_press2->text().toDouble();                   //气压阈值2


    double prot_vel=ui->line_set_prot_vel->text().toDouble();
    double prot_accdec=ui->line_set_prot_accdec->text().toDouble();
    double prot_movemax=ui->line_set_prot_movemax->text().toDouble();
    double prot_movemin=ui->line_set_prot_movemin->text().toDouble();
    double csv_size=ui->line_set_csv_size->text().toDouble();
    double log_size=ui->line_set_log_size->text().toDouble();
    double save_s=ui->line_set_save_s->text().toDouble();
    double save_hz=ui->combox_set_save_hz->currentIndex();

    double save_time=ui->line_set_save_time->text().toDouble();
    qDebug()<<"===================================";


    QDomDocument doc;
    QDomElement root = doc.createElement("Protect_Parameters");
    doc.appendChild(root);

    QDomElement element1 = doc.createElement("warn_tem1");
    element1.appendChild(doc.createTextNode(QString::number(warn_tem1)));
    root.appendChild(element1);

    QDomElement element2 = doc.createElement("warn_tem2");
    element2.appendChild(doc.createTextNode(QString::number(warn_tem2)));
    root.appendChild(element2);

    QDomElement element3 = doc.createElement("wrong_tem1");
    element3.appendChild(doc.createTextNode(QString::number(wrong_tem1)));
    root.appendChild(element3);

    QDomElement element4 = doc.createElement("wrong_tem2");
    element4.appendChild(doc.createTextNode(QString::number(wrong_tem2)));
    root.appendChild(element4);

    QDomElement element5 = doc.createElement("warn_current1");
    element5.appendChild(doc.createTextNode(QString::number(warn_current1)));
    root.appendChild(element5);

    QDomElement element6 = doc.createElement("warn_current2");
    element6.appendChild(doc.createTextNode(QString::number(warn_current2)));
    root.appendChild(element6);

    QDomElement element7 = doc.createElement("wrong_current1");
    element7.appendChild(doc.createTextNode(QString::number(wrong_current1)));
    root.appendChild(element7);

    QDomElement element8 = doc.createElement("wrong_current2");
    element8.appendChild(doc.createTextNode(QString::number(wrong_current2)));
    root.appendChild(element8);

    QDomElement element9 = doc.createElement("flow1");
    element9.appendChild(doc.createTextNode(QString::number(flow1)));
    root.appendChild(element9);

    QDomElement element10 = doc.createElement("flow2");
    element10.appendChild(doc.createTextNode(QString::number(flow2)));
    root.appendChild(element10);

    QDomElement element11 = doc.createElement("press1");
    element11.appendChild(doc.createTextNode(QString::number(press1)));
    root.appendChild(element11);

    QDomElement element12 = doc.createElement("press2");
    element12.appendChild(doc.createTextNode(QString::number(press2)));
    root.appendChild(element12);



    QDomElement element13 = doc.createElement("prot_vel");
    element13.appendChild(doc.createTextNode(QString::number(prot_vel)));
    root.appendChild(element13);
    QDomElement element14 = doc.createElement("prot_accdec");
    element14.appendChild(doc.createTextNode(QString::number(prot_accdec)));
    root.appendChild(element14);
    QDomElement element15 = doc.createElement("prot_movemax");
    element15.appendChild(doc.createTextNode(QString::number(prot_movemax)));
    root.appendChild(element15);
    QDomElement element16 = doc.createElement("prot_movemin");
    element16.appendChild(doc.createTextNode(QString::number(prot_movemin)));
    root.appendChild(element16);
    QDomElement element17 = doc.createElement("csv_size");
    element17.appendChild(doc.createTextNode(QString::number(csv_size)));
    root.appendChild(element17);
    QDomElement element18 = doc.createElement("log_size");
    element18.appendChild(doc.createTextNode(QString::number(log_size)));
    root.appendChild(element18);
    QDomElement element19 = doc.createElement("save_s");
    element19.appendChild(doc.createTextNode(QString::number(save_s)));
    root.appendChild(element19);
    QDomElement element20 = doc.createElement("save_hz");
    element20.appendChild(doc.createTextNode(QString::number(save_hz)));
    root.appendChild(element20);
    QDomElement element21 = doc.createElement("save_time");
    element21.appendChild(doc.createTextNode(QString::number(save_time)));
    root.appendChild(element21);




    QString filePath = QFileDialog::getSaveFileName(this, tr("Save XML File"), QDir::homePath(), tr("XML Files (*.xml)"));

    if (filePath.isEmpty()) {
        //QMessageBox::warning(this, "Operation Cancelled", "No file selected.");
        return;
    }

    // 保存到文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << doc.toString();
        file.close();
        QMessageBox::information(this, "Success", QString("成功保存到 %1").arg(filePath));
    } else {
        QMessageBox::critical(this, "Error", QString("Failed to open file for writing: %1").arg(filePath));
    }

    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath2 = currentDir + "/initprotectdata.xml";
    QFile file2(filePath2);
    if (file2.open(QIODevice::WriteOnly)) {
        QTextStream out(&file2);
        out << doc.toString();
        file.close();

    }
}

void Widget::init_protcet_xml()
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath2 = currentDir + "/initprotectdata.xml";QFile file2(filePath2);
    if (!file2.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", QString("Failed to open file for reading: %1").arg(filePath2));
        return;
    }
    QDomDocument doc2;
    if (!doc2.setContent(&file2)) {
        QMessageBox::critical(this, "Error", "Failed to parse XML file.");
        file2.close();
        return;
    }
    file2.close();
    QDomElement root = doc2.documentElement();

    if (root.tagName() != "Protect_Parameters") {
        QMessageBox::warning(this, "Invalid File", "请选择正确文件");
        return;
    }


    ui->spin_set_warn_tem1->setText(root.firstChildElement("warn_tem1").text());
    ui->spin_set_warn_tem2->setText(root.firstChildElement("warn_tem2").text());
    ui->spin_set_wrong_tem1->setText(root.firstChildElement("wrong_tem1").text());
    ui->spin_set_wrong_tem2->setText(root.firstChildElement("wrong_tem2").text());
    ui->spin_set_warn_current1->setText(root.firstChildElement("warn_current1").text());
    ui->spin_set_warn_current2->setText(root.firstChildElement("warn_current2").text());
    ui->spin_set_wrong_current1->setText(root.firstChildElement("wrong_current1").text());
    ui->spin_set_wrong_current2->setText(root.firstChildElement("wrong_current2").text());
    ui->spin_set_flow1->setText(root.firstChildElement("flow1").text());
    ui->spin_set_flow2->setText(root.firstChildElement("flow2").text());
    ui->spin_set_press1->setText(root.firstChildElement("press1").text());
    ui->spin_set_press2->setText(root.firstChildElement("press2").text());


    ui->line_set_prot_vel->setText(QString::number(root.firstChildElement("prot_vel").text().toDouble()));
    ui->line_set_prot_accdec->setText(QString::number(root.firstChildElement("prot_accdec").text().toDouble()));
    ui->line_set_prot_movemax->setText(QString::number(root.firstChildElement("prot_movemax").text().toDouble()));
    ui->line_set_prot_movemin->setText(QString::number(root.firstChildElement("prot_movemin").text().toDouble()));
    ui->line_set_csv_size->setText(QString::number(root.firstChildElement("csv_size").text().toDouble()));
    ui->line_set_log_size->setText(QString::number(root.firstChildElement("log_size").text().toDouble()));
    ui->line_set_save_s->setText(QString::number(root.firstChildElement("save_s").text().toDouble()));

    QTimer::singleShot(1000, this, [=](){
        ui->combox_set_save_hz->setCurrentIndex(root.firstChildElement("save_hz").text().toDouble());
    });
    ui->line_set_save_time->setText(QString::number(root.firstChildElement("save_time").text().toDouble()));

}

void Widget::on_btn_set_change_csv_path_clicked()
{
    SaveLog::Instance()->stop();
    t_addlog(1,"更新数据保存位置");
    //timer_save->stop();
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/savepath.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for reading:" << filePath;
        return;
    }
    QTextStream in(&file);
    QString imagepathLine;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("imagepath=")) {
            imagepathLine = line;
        }
    }
    file.close();
    QString newPath = QFileDialog::getExistingDirectory(
                this, tr("选择目录"),
                QDir::homePath()
                );

    if (newPath.isEmpty()) return;
    QString newContent;
    newContent += "datapath=" + newPath + "\n";
    if (!imagepathLine.isEmpty()) {
        newContent += imagepathLine + "\n";
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for writing:" << filePath;
        return;
    }
    QTextStream out(&file);
    out << newContent;
    file.close();
    ui->label_set_csv_path->setText(newPath);
    QMutexLocker locker(&m_filenameMutex);
    filename=createcsv();
    xunhuanfilename=createxunhuancsv();
    logname=createLog();
    //timer_save->start(save_timer);
    SaveLog::Instance()->start(getlogfile());
}

void Widget::on_btn_set_change_image_path_clicked()
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/savepath.txt";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for reading:" << filePath;
        return;
    }
    QTextStream in(&file);
    QString imagepathLine;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("datapath=")) {
            imagepathLine = line;
        }
    }
    file.close();

    QString newPath = QFileDialog::getExistingDirectory(
                this, tr("选择目录"),
                QDir::homePath()
                );

    if (newPath.isEmpty()) return;

    QString newContent;
    newContent += "imagepath=" + newPath + "\n";
    if (!imagepathLine.isEmpty()) {
        newContent += imagepathLine + "\n";
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for writing:" << filePath;
        return;
    }
    QTextStream out(&file);
    out << newContent;
    file.close();
    ui->label_set_image_path->setText(newPath);
}

void Widget::on_btn_set_open_csv_path_clicked()
{
    t_addlog(1,"打开数据保存路径");
    QString targetFolderPath=ui->label_set_csv_path->text();
    QUrl folderUrl = QUrl::fromLocalFile(targetFolderPath);
    QDesktopServices::openUrl(folderUrl);
}

void Widget::on_btn_set_opene_image_path_clicked()
{
    QString targetFolderPath=ui->label_set_image_path->text();
    QUrl folderUrl = QUrl::fromLocalFile(targetFolderPath);
    QDesktopServices::openUrl(folderUrl);
}

void Widget::on_pushButton_clicked()
{

    QString passwordAdmin;
    QString passwordTester;
    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/password.txt";
    // 检查文件是否存在
    QFile file(filePath);
    if (!file.exists())
    {
        QMessageBox::warning(this,"警告","密码文件丢失!!!");
    }
    else
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open file:" << filePath;
            return;
        }

        QTextStream in(&file);
        QString line;
        while ((line = in.readLine()).isNull() == false)
        {
            if (line.startsWith("password_Administrator"))
            {
                passwordAdmin = line.section('=', 1, 1).trimmed();
            }
            else if (line.startsWith("password_Tester"))
            {
                passwordTester = line.section('=', 1, 1).trimmed();
            }
        }
        file.close();
    }
    if (*loginstate == 1)
    {
        qDebug()<<"*loginstate == 1";
        user_disableall();
        *loginstate = 0;
        ui->label_user->setText("操作员");
        t_addlog(1,"软件切换权限-->操作员");
        return;
    }
    else if (*loginstate == 0)
    {
        QDialog dialog(this);
        QFormLayout form(&dialog);
        form.addRow(new QLabel("输入密码:"));

        QLineEdit *lineEdit = new QLineEdit(&dialog);
        lineEdit->setEchoMode(QLineEdit::Password);
        form.addRow(lineEdit);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);

        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
            QString inputPassword = lineEdit->text();

            qDebug() << "Input Password:" << inputPassword;
            if (inputPassword == passwordAdmin)
            { qDebug()<<"*loginstate == 0";
                user_enableall();
                *loginstate = 1;
                ui->label_user->setText("管理员");
                t_addlog(1,"软件切换权限-->管理员");
            }
            else
            {
                QMessageBox::warning(this, " ", "密码错误");
            }
            dialog.accept();
        });

        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, [&]() {
            dialog.reject();
        });
        dialog.exec();

    }
}

void Widget::user_enableall()
{
    ui->line_test_vel->setEnabled(true);
    ui->line_test_accdec->setEnabled(true);
    ui->line_test_waittime->setEnabled(true);
    ui->line_test_movenum->setEnabled(true);
    ui->line_test_A->setEnabled(true);
    ui->line_test_B->setEnabled(true);
    ui->spin_set_warn_tem1->setEnabled(true);
    ui->spin_set_wrong_tem1->setEnabled(true);
    ui->spin_set_warn_current1->setEnabled(true);
    ui->spin_set_wrong_current1->setEnabled(true);
    ui->spin_set_flow1->setEnabled(true);
    ui->spin_set_press1->setEnabled(true);
    ui->spin_set_warn_tem2->setEnabled(true);
    ui->spin_set_wrong_tem2->setEnabled(true);
    ui->spin_set_warn_current2->setEnabled(true);
    ui->spin_set_wrong_current2->setEnabled(true);
    ui->spin_set_flow2->setEnabled(true);
    ui->spin_set_press2->setEnabled(true);
    ui->line_set_prot_vel->setEnabled(true);
    ui->line_set_prot_accdec->setEnabled(true);
    ui->line_set_prot_movemax->setEnabled(true);
    ui->line_set_prot_movemin->setEnabled(true);
    ui->line_set_csv_size->setEnabled(true);
    ui->line_set_log_size->setEnabled(true);
    ui->line_set_save_s->setEnabled(true);
    ui->combox_set_save_hz->setEnabled(true);
    ui->btn_set_change_csv_path->setEnabled(true);
    ui->btn_set_change_image_path->setEnabled(true);
    ui->btn_set_load_set->setEnabled(true);
    ui->btn_set_save_set->setEnabled(true);
    ui->set_save_key->setEnabled(true);
    ui->set_protect_key->setEnabled(true);
    ui->btn_set_change_csv_path->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }");
    ui->btn_set_change_image_path->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }");
    ui->btn_set_load_set->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }");
    ui->btn_set_save_set->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }");
}

void Widget::user_disableall()
{
    ui->line_test_vel->setEnabled(false);
    ui->line_test_accdec->setEnabled(false);
    ui->line_test_waittime->setEnabled(false);
    ui->line_test_movenum->setEnabled(false);
    ui->line_test_A->setEnabled(false);
    ui->line_test_B->setEnabled(false);
    ui->spin_set_warn_tem1->setEnabled(false);
    ui->spin_set_wrong_tem1->setEnabled(false);
    ui->spin_set_warn_current1->setEnabled(false);
    ui->spin_set_wrong_current1->setEnabled(false);
    ui->spin_set_flow1->setEnabled(false);
    ui->spin_set_press1->setEnabled(false);
    ui->spin_set_warn_tem2->setEnabled(false);
    ui->spin_set_wrong_tem2->setEnabled(false);
    ui->spin_set_warn_current2->setEnabled(false);
    ui->spin_set_wrong_current2->setEnabled(false);
    ui->spin_set_flow2->setEnabled(false);
    ui->spin_set_press2->setEnabled(false);
    ui->line_set_prot_vel->setEnabled(false);
    ui->line_set_prot_accdec->setEnabled(false);
    ui->line_set_prot_movemax->setEnabled(false);
    ui->line_set_prot_movemin->setEnabled(false);
    ui->line_set_csv_size->setEnabled(false);
    ui->line_set_log_size->setEnabled(false);
    ui->line_set_save_s->setEnabled(false);
    ui->combox_set_save_hz->setEnabled(false);
    ui->btn_set_change_csv_path->setEnabled(false);
    ui->btn_set_change_image_path->setEnabled(false);
    ui->btn_set_load_set->setEnabled(false);
    ui->btn_set_save_set->setEnabled(false);
    ui->set_save_key->setEnabled(false);
    ui->set_protect_key->setEnabled(false);
    ui->btn_set_change_csv_path->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }""QPushButton:disabled {"
            "    background-color: #bdc3c7;""    color: #7f8c8d;""}");
    ui->btn_set_change_image_path->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }""QPushButton:disabled {"
            "    background-color: #bdc3c7;""    color: #7f8c8d;""}");
    ui->btn_set_load_set->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }""QPushButton:disabled {"
            "    background-color: #bdc3c7;""    color: #7f8c8d;""}");
    ui->btn_set_save_set->setStyleSheet(
                "QPushButton {""    background-color: #487eb0;""    color: white;""    border: none;"
                "    border-radius: 8px;""}""QPushButton:hover { background-color: #40739e; }"
                "QPushButton:pressed { background-color: #2c3e50; }""QPushButton:disabled {"
            "    background-color: #bdc3c7;""    color: #7f8c8d;""}");
}

void Widget::on_change_test_num_clicked()
{
    t_addlog(1, "修改测试编号");
    bool ok = false;
    QString styleSheet =
            "QDialog, QDialog > QWidget {"
        "   min-width: 400px;"
        "   min-height: 40px;"
        "}"
        "QLabel {"
        "   font-size: 18px;"
        "   padding: 10px;"
        "}"
        "QLineEdit {"
        "   font-size: 18px;"
        "   min-height: 40px;"
        "   padding: 5px;"
        "}"
        "QPushButton {"
        "   font-size: 18px;"
        "   min-width: 100px;"
        "   min-height: 40px;"
        "   padding: 5px;"
        "}";

    QInputDialog dialog;
    dialog.setStyleSheet(styleSheet);
    //dialog.setMinimumSize(500, 80);



    dialog.setWindowTitle("输入测试编号名");
    dialog.setLabelText("请输入测试编号名:");
    dialog.setInputMode(QInputDialog::TextInput);

    QString folderName;
    if (dialog.exec() == QDialog::Accepted) {
        folderName = dialog.textValue();
        ok = true;

        if (!folderName.isEmpty()) {
            QString invalidChars = "<>:\"/\\|?*";
            for (QChar ch : invalidChars) {
                if (folderName.contains(ch)) {
                    QMessageBox::warning(
                                nullptr,
                                "错误",
                                "编号名不能包含以下字符: < > : \" / \\ | ? *"
                    );
                    return;
                }
            }

            if (folderName.startsWith(" ") || folderName.startsWith(".") ||
                    folderName.endsWith(" ") || folderName.endsWith(".")) {
                QMessageBox::warning(
                            nullptr,
                            "错误",
                            "编号名不能以空格或点开头/结尾！"
                );
                return;
            }

            if (folderName.trimmed().isEmpty()) {
                QMessageBox::warning(nullptr, "错误", "编号名不能为空！");
                return;
            }

            QMessageBox::information(nullptr, "成功", "输入有效: " + folderName);
        } else {
            QMessageBox::warning(nullptr, "错误", "编号名不能为空！");
            return;
        }
    }

    if (!ok || folderName.isEmpty()) {
        return;
    }

    QString value=folderName;

    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/testnumber.txt";

    // 读取文件内容
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for reading:" << filePath;
        return;
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testnumber=")) {
            line = "testnumber=" + value;
        }
        lines << line;
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for writing:" << filePath;
        return;
    }

    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    file.close();

    ui->label_19->setText(value);
    QMutexLocker locker(&m_filenameMutex);
    filename = createcsv();
    xunhuanfilename = createxunhuancsv();
    logname = createLog();



    QString a6=readTestNum();
    allrecyclenum=(a6).toInt();
    qDebug()<<"a6"<<a6;
    ui->above_all_num->setText(a6);
    oldtime=readTestTime();
    ui->above_time->setText(oldtime);
    xunhuanpausetime = QTime();
}

QString Widget::getTestDataFolderPath() {
    QString currentDir = QCoreApplication::applicationDirPath();
    QString configPath = currentDir + "/savepath.txt";

    QString dataPath;
    QFile configFile(configPath);
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&configFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line.mid(9);
                break;
            }
        }
        configFile.close();
    }

    if (dataPath.isEmpty()) {
        qWarning() << "无法获取datapath配置";
        return "";
    }

    QDir dataDir(dataPath);
    QString dataFolderPath = dataDir.absoluteFilePath("data");
    QString testNumber = gettestnumber();
    return dataFolderPath + "/" + testNumber;
}

// 写入testtime
void Widget::writeTestTime(const QString &value) {
    QString filePath = getTestDataFolderPath() + "/test_num.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        return;
    }

    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testtime=")) {
            line = "testtime=" + value;
        }
        lines << line;
    }

    if (lines.filter("testtime=").isEmpty()) {
        lines << "testtime=" + value;
    }

    file.resize(0);
    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    file.close();
}

// 读取testtime
QString Widget::readTestTime() {
    QString filePath = getTestDataFolderPath() + "/test_num.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        return "0";
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testtime=")) {
            return line.mid(9);
        }
    }
    return "0";
}

// 写入testnum
void Widget::writeTestNum(const QString &value) {
    QString filePath = getTestDataFolderPath() + "/test_num.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        return;
    }

    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testnum=")) {
            line = "testnum=" + value;
        }
        lines << line;
    }


    if (lines.filter("testnum=").isEmpty()) {
        lines << "testnum=" + value;
    }


    file.resize(0);
    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    file.close();
}

QString Widget::readTestNum() {
    QString filePath = getTestDataFolderPath() + "/test_num.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath;
        return "0";
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testnum=")) {
            file.close();
            return line.mid(8);
        }
    }
    file.close();
    return "0";
}

QTime Widget::parseSavedTime(const QString &timeStr) {
    QStringList parts = timeStr.split('.');
    if (parts.size() != 2) return QTime::currentTime();

    int days = parts[0].toInt();
    QString timePart = parts[1];

    QStringList timeComponents = timePart.split(':');
    if (timeComponents.size() != 4) return QTime::currentTime();

    int hours = timeComponents[0].toInt() + days * 24;
    int minutes = timeComponents[1].toInt();
    int seconds = timeComponents[2].toInt();
    int msecs = timeComponents[3].toInt();

    return QTime(hours, minutes, seconds, msecs);
}

void Widget::saveFrameimage(int frame_num, QFrame* frame)
{
    QPixmap screenshot = frame->grab();

    QString currentDir = QCoreApplication::applicationDirPath();
    QString configPath = currentDir + "/savepath.txt";

    QString dataPath;
    QFile configFile(configPath);
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&configFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("imagepath=")) {
                dataPath = line.mid(9);
                break;
            }
        }
        configFile.close();
    }

    if (dataPath.isEmpty()) {
        qWarning() << "未找到有效的数据路径配置";
        return;
    }
    QString datapath2= dataPath.mid(dataPath.indexOf('=') + 1);
    QDir dataDir(datapath2);
    QString imageFolderPath = dataDir.absoluteFilePath("image");

    if (!QDir(imageFolderPath).exists()) {
        if (!dataDir.mkdir("image")) {
            qWarning() << "创建图片目录失败:" << imageFolderPath;
            return;
        }
    }

    QString testNumber = gettestnumber();
    QString targetFolder = imageFolderPath + "/" + testNumber;
    if (!QDir(targetFolder).exists()) {
        QDir().mkpath(targetFolder);
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyyMMdd_HHmmss_zzz");
    QString fileName;
    switch(frame_num)
    {
    case 1:
        fileName = QString("位置_%1.png").arg(timestamp);break;
    case 2:
        fileName = QString("速度_加速度_%1.png").arg(timestamp);break;
    case 3:
        fileName = QString("温度_%1.png").arg(timestamp);break;
    case 4:
        fileName = QString("电流_%1.png").arg(timestamp);break;
    case 5:
        fileName = QString("开始界面_%1.png").arg(timestamp);break;
    }


    QString fullPath = targetFolder + "/" + fileName;
    if (!screenshot.save(fullPath)) {
        qWarning() << "截图保存失败:" << fullPath;
    } else {
        qDebug() << "截图已保存至:" << fullPath;
    }
}

void Widget::on_save_image5_clicked()
{
    t_addlog(1,"保存开始界面截图");
    saveFrameimage(5,ui->frame_start);
}

void Widget::on_save_image1_clicked()
{
    t_addlog(1,"保存位置图表截图");
    saveFrameimage(1,ui->frame_test1);
}

void Widget::on_save_image2_clicked()
{
    t_addlog(1,"保存速度_加速度图表截图");
    saveFrameimage(2,ui->frame_test2);
}

void Widget::on_save_image3_clicked()
{
    t_addlog(1,"保存位置图表截图");
    saveFrameimage(3,ui->frame_test3);
}

void Widget::on_save_image4_clicked()
{
    t_addlog(1,"保存电流图表截图");
    saveFrameimage(4,ui->frame_test4);
}

void Widget::refresh_olddata()
{
    result.clear();
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
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyy.MM.dd.HH.mm.ss");

    QString datapath2 = dataPath.mid(dataPath.indexOf('=') + 1);
    QDir dataDir(datapath2);
    QString dataFolderPath = dataDir.absoluteFilePath("data");

    if (!QDir(dataFolderPath).exists()) {
        if (!dataDir.mkdir("data")) {
            qWarning() << "Failed to create directory:" << dataFolderPath;
            return;
        }
    }

    QString folderPath = dataFolderPath + "/" + gettestnumber();
    QDir dir(folderPath);
    QFileInfoList files = dir.entryInfoList(QStringList() << "*Cyclic_Motion*.csv", QDir::Files);
    int n = files.size();
    qDebug() << "找到的CSV文件数量:" << n;


    QList<int> validLineCounts;
    for (const QFileInfo &fileInfo : files) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "无法打开文件:" << file.fileName();
            validLineCounts.append(0);
            continue;
        }

        QTextStream in(&file);
        in.readLine();
        in.readLine();

        int count = 0;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList cols = line.split(',');
            if (cols.size() >= 5) count++;
        }
        file.close();
        validLineCounts.append(count);
    }

    int totalValidLines = 0;
    for (int count : validLineCounts) {
        totalValidLines += count;
    }

    const int TOTAL_NEEDED = 2000;
    QList<int> rowsPerFile;
    if (totalValidLines <= TOTAL_NEEDED) {
        for (int i = 0; i < n; ++i) {
            rowsPerFile.append(validLineCounts[i]);
        }
    } else {
        for (int i = 0; i < n; ++i) {
            rowsPerFile.append(static_cast<int>(static_cast<double>(validLineCounts[i]) / totalValidLines * TOTAL_NEEDED));
        }

        int currentSum = 0;
        for (int count : rowsPerFile) {
            currentSum += count;
        }
        if (currentSum < TOTAL_NEEDED) {
            for (int i = 0; i < TOTAL_NEEDED - currentSum; ++i) {
                if (i < rowsPerFile.size()) {
                    rowsPerFile[i]++;
                }
            }
        } else if (currentSum > TOTAL_NEEDED) {
            for (int i = 0; i < currentSum - TOTAL_NEEDED; ++i) {
                if (i < rowsPerFile.size()) {
                    rowsPerFile[i]--;
                }
            }
        }
    }

    for (int i = 0; i < files.size(); ++i) {
        if (rowsPerFile[i] <= 0) continue;

        QFile file(files[i].absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "无法打开文件:" << file.fileName();
            continue;
        }

        QTextStream in(&file);
        in.readLine();
        in.readLine();

        QList<QStringList> validLines;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            QStringList cols = line.split(',');
            if (cols.size() >= 5) {
                validLines.append(cols);
            }
        }
        file.close();

        if (validLines.isEmpty()) continue;

        QList<QList<double>> fileData;
        int needed = rowsPerFile[i];
        for (int j = 0; j < needed; ++j) {
            int idx = static_cast<int>(static_cast<double>(j) / needed * validLines.size());
            if (idx >= validLines.size()) idx = validLines.size() - 1;

            QStringList cols = validLines[idx];
            bool ok1, ok2, ok3, ok4;
            double v2 = cols[1].toDouble(&ok1);
            double v3 = cols[2].toDouble(&ok2);
            double v4 = cols[3].toDouble(&ok3);
            double v5 = cols[4].toDouble(&ok4);

            if (!ok1 || !ok2 || !ok3 || !ok4) {
                qDebug() << "数据转换失败，行内容:" << cols;
                continue;
            }
            fileData.append({v2, v3, v4, v5});
        }

        if (!fileData.isEmpty()) {
            result.append(fileData);
        }
    }

    int finalSize = 0;
    for (const QList<QList<double>> &data : result) {
        finalSize += data.size();
    }

    if (finalSize < TOTAL_NEEDED) {
        qDebug() << "最终数据不足2000行，实际获取:" << finalSize << "行";
    } else {
        qDebug() << "成功获取2000行数据";
    }
}

void Widget::on_t_log_clicked()
{
    if (logWindow->isVisible()) {
        logWindow->hide();
    } else {
        logWindow->show();
    }

}

void Widget::on_combox_set_save_hz_currentIndexChanged(int index)
{
    qDebug()<<1000/(ui->combox_set_save_hz->currentText().toInt());
    getinfotimer1->stop();
    getinfotimer1->start(1000/(ui->combox_set_save_hz->currentText().toInt()));
}

void Widget::on_set_protect_key_stateChanged(int arg1)
{
    static int previousState = Qt::Unchecked; // 保存前一个状态

    if (arg1 == Qt::Unchecked && previousState == Qt::Checked) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("警告"));
        msgBox.setText(tr("保护系统将关闭！"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);

        QString styleSheet =
                "QMessageBox {"
            "   background-color: white;"
            "   min-width: 200px;"      // 最小宽度
                "   min-height: 150px;"     // 最小高度
                "   font-size: 18px;"      // 基本字体大小
                "}"
            "QMessageBox QLabel {"
            "   font-size: 30px;"      // 文本字体大小
                "   min-width: 300px;"     // 文本区域最小宽度
                "   min-height: 100px;"    // 文本区域最小高度
                "}"
            "QMessageBox QPushButton {"
            "   font-size: 18px;"      // 按钮字体大小
                "   min-width: 100px;"    // 按钮最小宽度
                "   min-height: 40px;"    // 按钮最小高度
                "   padding: 10px;"      // 按钮内边距
                "}";

        msgBox.setStyleSheet(styleSheet);

        msgBox.setMinimumSize(400, 200);

        msgBox.exec();
    }

    previousState = arg1;
}
