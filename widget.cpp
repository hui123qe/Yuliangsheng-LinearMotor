#include "widget.h"
#include "ui_widget.h"
#include "tt_tem.hpp"

//是否循环运动的标签
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
    connect_ACS(); //连接设备
}

Widget::~Widget()
{
    if (logWindow) {
        logWindow->close(); // 关闭日志窗口
        delete logWindow;   // 删除日志窗口
    }
    getinfotimer1->stop();
    getinfotimer2->stop();
        m_workThread->quit();
        m_workThread->wait();
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
        msgBox.setText("设备连接失败，请检查通讯");
        msgBox.setIcon(QMessageBox::Warning);

        // 设置整体样式和字体
        msgBox.setStyleSheet(
                    "QMessageBox {"
            "   min-width: 1600px;"
            "   min-height:1300px;"
            "}"
            "QMessageBox QLabel {"
            "   font-size: 25px;"  // 设置文本字体大小
                    "}"
            "QMessageBox QPushButton {"
            "   font-size: 25px;"  // 设置按钮字体大小
                    "   min-width: 100px;"  // 设置按钮最小宽度
                    "   min-height: 50px;" // 设置按钮最小高度
                    "}"
        );

        msgBox.exec();
        t_addlog(1,"设备连接失败，请检查通讯");
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
        t_addlog(1,"设备连接成功");
        getinfotimer1->start(100);

        // 1. 创建子线程
            m_workThread = new QThread(this);

            // 2. 创建定时器（关键：不能指定父对象！）
            getinfotimer2 = new QTimer;
            getinfotimer2->setInterval(100); // 100ms

            // 3. 定时器 移动到子线程
            getinfotimer2->moveToThread(m_workThread);

            // 4. 定时器触发 → 执行你的任务（子线程）
            connect(getinfotimer2, &QTimer::timeout, this, &Widget::timerTask);

            // 5. 线程启动 → 自动启动定时器
            connect(m_workThread, &QThread::started, getinfotimer2, QOverload<>::of(&QTimer::start));

            // 6. 安全回收
            connect(m_workThread, &QThread::finished, getinfotimer2, &QTimer::deleteLater);

            // 7. 启动子线程（定时器开始跑）
            m_workThread->start();
    }

    //连接温度模块
    if (!tempmodbus->Connect("192.168.1.150", 5000)) {
        t_addlog(1,"通讯模块连接失败");
        return;
    }
    t_addlog(1,"通讯模块连接成功");

}


void Widget::log_init()
{
    logWindow = new LogWindow(nullptr); // 设置父窗口为 nullptr，使其独立
    logWindow->hide(); // 初始时隐藏日志窗口




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
//图表初始化
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
    mGraph5_1 = mPlot5->addGraph();            //开始界面图像添加第一根线
    mGraph5_2 = mPlot5->addGraph();            //开始界面图像添加第二根线
    mGraph5_1->setPen(QPen(Qt::red, 2));       //开始界面图像第一根线设置红色
    mGraph5_2->setPen(QPen(Qt::blue, 2));      //开始界面图像第二根线设置蓝色
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
    mPlot5->yAxis2->setVisible(true);          //显示右侧的第二个Y轴（Y2轴）
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
    mGraph1_1 = mPlot1->addGraph();            //测试界面1图像添加第一根线
    mGraph1_2 = mPlot1->addGraph();            //测试界面1图像添加第二根线
    mGraph1_1->setPen(QPen(Qt::red, 2));       //测试界面1图像第一根线设置红色
    mGraph1_2->setPen(QPen(Qt::blue, 2));      //测试界面1图像第二根线设置蓝色
    // 坐标轴设置
    mPlot1->xAxis->setLabel("          时间 (s)");
    mPlot1->yAxis->setLabel("                           位置 (mm)");
    mPlot1->xAxis->setLabelFont(axisFont);
    mPlot1->yAxis->setLabelFont(axisFont);
    mPlot1->xAxis->setRange(0, 20);
    mPlot1->yAxis->setRange(-200, 700);


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

    // 初始化位置曲线
    mGraph4_1 = mPlot4->addGraph();
    mGraph4_2 = mPlot4->addGraph();
    mGraph4_1->setPen(QPen(Qt::red, 2));
    mGraph4_2->setPen(QPen(Qt::blue, 2));

    // 坐标轴设置
    mPlot4->xAxis->setLabel("次数");
    mPlot4->yAxis->setLabel("                            电流 (A)");
    mPlot4->xAxis->setLabelFont(axisFont);
    mPlot4->yAxis->setLabelFont(axisFont);
    mPlot4->xAxis->setRange(0, 20);
    mPlot4->yAxis->setRange(-10, 200);


    //-------------------通用设置--------------------//
    //    mPlot5->yAxis->setLabelRotation(90);  // 0=水平，90=竖直向上，-90=竖直向下
    //    mPlot5->yAxis->setLabelPadding(-30);  // 调整数值使标签靠近顶部
    // 交互设置  启用视图拖拽功能
    mPlot1->setInteractions(QCP::iRangeDrag);
    mPlot2->setInteractions(QCP::iRangeDrag);
    mPlot3->setInteractions(QCP::iRangeDrag);
    mPlot4->setInteractions(QCP::iRangeDrag);
    mPlot5->setInteractions(QCP::iRangeDrag);

    // 事件过滤  设置事件过滤器
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

        // ------------------------------------------------------------特殊处理双Y轴图表------------------------------------------------------------
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
        const double initialXRange = 20.0; // 固定初始X轴长度10秒
        double currentEnd = mTimeData.last();

        // 设置新的X轴范围（保证总长度10秒）
        double newXMin = qMax(currentEnd - initialXRange, 0.0); // 确保最小值不小于0
        double newXMax = qMax(currentEnd, initialXRange);       // 确保有数据时至少显示10秒

        //currentPlot->xAxis->setRange(newXMin, newXMax);

        // 同步处理特殊轴（如双Y轴）
        if (currentPlot == mPlot5) {
            double lastX;
            if (mGraph5_1) {
                QSharedPointer<QCPGraphDataContainer> dataContainer = mGraph5_1->data();
                if (!dataContainer->isEmpty()) {
                    // 获取最后一个数据点的迭代器
                    QCPGraphDataContainer::const_iterator lastPointIterator = dataContainer->constEnd();
                    --lastPointIterator; // 移动到最后一个数据点

                    // 获取最后一个点的坐标
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
            currentPlot->yAxis->setRange(-200, 700);
        }else if (currentPlot == mPlot2) {
            currentPlot->xAxis->setRange(newXMin, newXMax);
            currentPlot->yAxis->setRange(-10000, 10000);
        }else if (currentPlot == mPlot3) {
            double lastX;
            if (mGraph3_1) {
                QSharedPointer<QCPGraphDataContainer> dataContainer = mGraph3_1->data();
                if (!dataContainer->isEmpty()) {
                    // 获取最后一个数据点的迭代器
                    QCPGraphDataContainer::const_iterator lastPointIterator = dataContainer->constEnd();
                    --lastPointIterator; // 移动到最后一个数据点

                    // 获取最后一个点的坐标
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
                    // 获取最后一个数据点的迭代器
                    QCPGraphDataContainer::const_iterator lastPointIterator = dataContainer->constEnd();
                    --lastPointIterator; // 移动到最后一个数据点

                    // 获取最后一个点的坐标
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
            QString number = line.mid(11); // 跳过"testnumber="共11个字符
            file.close();
            ui->label_19->setText(number);
        }
    }


    //设置界面参数初始化
    init_protcet_xml();

    //生成初始csv文件
    filename=createcsv();
    xunhuanfilename=createxunhuancsv();
    gettestnumber();
    //初始化计数时间次数
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
    lookupDict=createLookupDict(R,T,sizeof(R) / sizeof(R[0]));

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
    setWindowFlags(Qt::FramelessWindowHint); //去除窗口默认边框
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
        //        ui->checkBox_test1_pos2->setStyleSheet(
        //                    "QCheckBox {"
        //                    "    spacing: 5px;"  // 整体间距
        //                    "}"
        //                    "QCheckBox::indicator {"
        //                    "    width: 20px;"    // 复选框宽度
        //                    "    height: 20px;"   // 复选框高度
        //                    "    subcontrol-position: top left;"  // 固定复选框在左上角
        //                    "}"
        //                    "QCheckBox::text {"
        //                    "    margin-top: 25px;"  // 文字与复选框的垂直间距
        //                    "}"
        //                );
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
    ///----------------------------------------------------开始界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------开始界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------开始界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------开始界面各状态组件设置----------------------------------------------------///
    //frame边框设置黑色，各frame间距
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
    //    ui->start_tips_not_connected->setFixedSize(50, 50);
    //    ui->start_tips_not_connected->setText(" ");
    //    ui->start_tips_not_connected->setStyleSheet(QString("QLabel { ""   background-color: black; ""   border-radius: %1px; "
    //     """   border: 2px solid black; ""}").arg(ui->start_tips_not_connected->width() / 2));

    //实时获取数据状态灯初始化
    ui->zsd_enable1->setFixedSize(50, 50);
    ui->zsd_enable1->setText(" ");
    ui->zsd_enable1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_enable1->width() / 2));
    ui->zsd_enable2->setFixedSize(50, 50);
    ui->zsd_enable2->setText(" ");
    ui->zsd_enable2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_enable2->width() / 2));
    ui->zsd_move1->setFixedSize(50, 50);
    ui->zsd_move1->setText(" ");
    ui->zsd_move1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_move1->width() / 2));
    ui->zsd_move2->setFixedSize(50, 50);
    ui->zsd_move2->setText(" ");
    ui->zsd_move2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_move2->width() / 2));
    ui->zsd_tem1->setFixedSize(50, 50);
    ui->zsd_tem1->setText(" ");
    ui->zsd_tem1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_tem1->width() / 2));
    ui->zsd_tem2->setFixedSize(50, 50);
    ui->zsd_tem2->setText(" ");
    ui->zsd_tem2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_tem2->width() / 2));
    ui->zsd_current1->setFixedSize(50, 50);
    ui->zsd_current1->setText(" ");
    ui->zsd_current1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_current1->width() / 2));
    ui->zsd_current2->setFixedSize(50, 50);
    ui->zsd_current2->setText(" ");
    ui->zsd_current2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_current2->width() / 2));
    ui->zsd_flow1->setFixedSize(50, 50);
    ui->zsd_flow1->setText(" ");
    ui->zsd_flow1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_flow1->width() / 2));
    ui->zsd_flow2->setFixedSize(50, 50);
    ui->zsd_flow2->setText(" ");
    ui->zsd_flow2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_flow2->width() / 2));
    ui->zsd_air1->setFixedSize(50, 50);
    ui->zsd_air1->setText(" ");
    ui->zsd_air1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_air1->width() / 2));
    ui->zsd_air2->setFixedSize(50, 50);
    ui->zsd_air2->setText(" ");
    ui->zsd_air2->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_air2->width() / 2));
    ui->zsd_gm1->setFixedSize(50, 50);
    ui->zsd_gm1->setText(" ");
    ui->zsd_gm1->setStyleSheet(QString("QLabel { ""   background-color: gray; ""   border-radius: %1px; "
     """   border: 2px solid black; ""}").arg(ui->zsd_gm1->width() / 2));
    ui->zsd_gm2->setFixedSize(50, 50);
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
    ///----------------------------------------------------测试界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------测试界面各状态组件设置----------------------------------------------------///
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
    ///----------------------------------------------------设置界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------设置界面各状态组件设置----------------------------------------------------///
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
    ///----------------------------------------------------帮助界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------帮助界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------帮助界面各状态组件设置----------------------------------------------------///
    ///----------------------------------------------------帮助界面各状态组件设置----------------------------------------------------///


    ui->searchEdit->addAction(QIcon(":/image/R-C.png"),QLineEdit::LeadingPosition);

    //设置文本边距（根据图标尺寸调整，示例：左侧留30像素）
    //ui->searchEdit->setTextMargins(30, 0, 0, 0);

    // 设置占位符文本
    ui->searchEdit->setPlaceholderText("输入关键字");

    // 通过样式表设置文本颜色和占位符颜色
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


    // 主线程监听 UI 勾选状态，同步到原子变量
    connect(ui->set_save_key, &QCheckBox::clicked, this, [this](bool checked) {
        m_shouldSaveCsv = checked; // 原子变量赋值
    });
}

void Widget::timerTask()
{
    QList<QString> datalist;
    //使能状态
    int State;
    acsc_GetMotorState(hComm,ACSC_AXIS_0,&State, NULL);
    int State1;
    acsc_GetMotorState(hComm,ACSC_AXIS_1,&State1, NULL);
    t_State=State;t_State1=State1;
    //运动状态
    int movestate0,movestate1;
    acsc_GetMotorState(hComm,ACSC_AXIS_0,&movestate0,NULL);
    acsc_GetMotorState(hComm,ACSC_AXIS_1,&movestate1,NULL);
    t_movestate0=movestate0;t_movestate1=movestate1;
    //测试时间
    QDateTime currentTime1 = QDateTime::currentDateTime(); //当前时间
    qint64 testtime = currentTime1.toMSecsSinceEpoch()-millisecondsSinceEpoch;
    double doubleValue = static_cast<double>(testtime) / 1000.0;
    QString strValue = QString::number(doubleValue, 'f', 3);
    datalist.append(strValue);
    //运行时间
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
    //位置
    double pos1;
    acsc_GetFPosition(hComm,ACSC_AXIS_0,&pos1,NULL);
    datalist.append(QString::number(pos1));
    pos1=-pos1-100;
    double pos2;
    acsc_GetFPosition(hComm,ACSC_AXIS_1,&pos2,NULL);
    datalist.append(QString::number(pos2));
    pos2=-pos2-100;
    t_pos1=pos1;t_pos2=pos2;
    //速度
    double vel1;
    acsc_GetFVelocity(hComm,ACSC_AXIS_0,&vel1, NULL );
    datalist.append(QString::number(vel1));
    double vel2;
    acsc_GetFVelocity(hComm,ACSC_AXIS_1,&vel2, NULL );
    datalist.append(QString::number(vel2));
    t_vel1=vel1;t_vel2=vel2;
    //加速度
    double acc1=getAInvalue(const_cast<char*>("?FACC0\r"));
    datalist.append(QString::number(acc1));
    double acc2=getAInvalue(const_cast<char*>("?FACC1\r"));
    datalist.append(QString::number(acc2));
    t_acc1=acc1;t_acc2=acc2;
    //电流
    double current1=getAInvalue(const_cast<char*>("?B\r"));
    datalist.append(QString::number(current1));
    double current2=getAInvalue(const_cast<char*>("?B2\r"));
    datalist.append(QString::number(current2));
    t_current1=current1;t_current2=current2;
    //电压
    double vol1=getAInvalue(const_cast<char*>("?A\r"));
    datalist.append(QString::number(vol1));
    double vol2=getAInvalue(const_cast<char*>("?A2\r"));
    datalist.append(QString::number(vol2));
    t_vol1=vol1;t_vol2=vol2;
    //温度
    float tem1=0;
    //tempmodbus->motionTem1(tem1);
    datalist.append(QString::number(tem1));
    float tem2=0;
    //tempmodbus->motionTem1(tem2);
    datalist.append(QString::number(tem2));
    t_tem1=tem1;t_tem2=tem2;
    //ptc温度阻值
    float temptc1=0;
    //tempmodbus->motionResptc1(temptc1);
    float temptc2=0;
    //tempmodbus->motionResptc2(temptc2);
    t_temptc1=temptc1;t_temptc2=temptc2;
    //流量
    double Flowrate1=getAInvalue(const_cast<char*>("?AInvalue3\r"));
    Flowrate1=((Flowrate1-3276)/(13104.0))*35+5;
    datalist.append(QString::number(Flowrate1));
    double Flowrate2=getAInvalue(const_cast<char*>("?AInvalue7\r"));
    Flowrate2=((Flowrate2-3276)/(13104.0))*35+5;
    datalist.append(QString::number(Flowrate2));
    t_Flowrate1=Flowrate1;t_Flowrate2=Flowrate2;
    //进口水压
    double inwater1=getAInvalue(const_cast<char*>("?AInvalue1\r)"));
    inwater1=(inwater1-3276)/(13104.0);
    datalist.append(QString::number(inwater1));
    double inwater2=getAInvalue(const_cast<char*>("?AInvalue5\r"));
    inwater2=(inwater2-3276)/(13104.0);
    datalist.append(QString::number(inwater2));
    t_inwater1=inwater1;t_inwater2=inwater2;
    //出口水压1
    double outwater1=getAInvalue(const_cast<char*>("?AInvalue2\r"));
    outwater1=(outwater1-3276)/(13104.0);
    ui->label_68->setText(QString::number(outwater1,'f',4));
    datalist.append(QString::number(outwater1));
    double outwater2=getAInvalue(const_cast<char*>("?AInvalue6\r"));
    outwater2=(outwater2-3276)/(13104.0);
    ui->label_71->setText(QString::number(outwater2,'f',4));
    datalist.append(QString::number(outwater2));
    t_outwater1=outwater1;t_outwater2=outwater2;
    //压降
    t_yajiang1=outwater1-inwater1;
    t_yajiang2=outwater2-inwater2;
    //气压1
    double qiya_1=getAInvalue(const_cast<char*>("?AInvalue4\r"));
    qiya_1=(qiya_1-3276)/(13104.0);
    ui->label_63->setText(QString::number(qiya_1,'f',4));
    datalist.append(QString::number(qiya_1));
    //气压2
    double qiya_2=getAInvalue(const_cast<char*>("?AInvalue8\r"));
    qiya_2=(qiya_2-3276)/(13104.0);
    ui->label_65->setText(QString::number(qiya_2,'f',4));
    datalist.append(QString::number(qiya_2));
    if (issaveincsv&&ui->set_save_key->isChecked())
    {
        QMutexLocker locker(&m_filenameMutex);
        appendRowsToCsv(filename,datalist);
    }

   // emit updateUI();
}


void Widget::updateUI()
{

}
void Widget::getinfo()
{
    //保存日志测试
    // qInfo()<<"info日志";
    //qWarning()<<"warning日志";
    //界面日志测试
    //    QString level = (QRandomGenerator::global()->bounded(2) == 0) ? "Info" : "Warning";
    //    QString message = "这是一个测试日志信息";
    //    logWindow->addLog(level, message);


    //判断各种状态
    //使能状态

    if(t_State!=16)
    {
        ui->zsd_enable1->setStyleSheet("QLabel { background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->test_enable_label_0->setStyleSheet("background-color: green;");
    }
    else if(t_State==16)
    {
        ui->zsd_enable1->setStyleSheet("QLabel { background-color: gray; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->test_enable_label_0->setStyleSheet("background-color: gray;");
    }

    if(t_State1!=16)
    {
        ui->zsd_enable2->setStyleSheet("QLabel { background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->test_enable_label_1->setStyleSheet("background-color: green;");
    }
    else if(t_State1==16)
    {
        ui->zsd_enable2->setStyleSheet("QLabel { background-color: gray; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->test_enable_label_1->setStyleSheet("background-color: gray;");
    }


    //运动状态
    if(t_movestate0 == 33)
    {
        ui->zsd_move1->setStyleSheet("QLabel { background-color: red; ""border-radius: 25px; " "border: 2px solid black; ""}");
    }else
    {
        ui->zsd_move1->setStyleSheet("QLabel { background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
    }
    if(t_movestate1 == 33)
    {
        ui->zsd_move2->setStyleSheet("QLabel { background-color: red; ""border-radius: 25px; " "border: 2px solid black; ""}");
    }else
    {
        ui->zsd_move2->setStyleSheet("QLabel { background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
    }



    //电压1
    ui->label_36->setText(QString::number(t_vol1,'f',4));
    //电压2
    ui->label_37->setText(QString::number(t_vol2,'f',4));

    //流量1
    ui->label_50->setText(QString::number(t_Flowrate1,'f',4));
    //流量2
    ui->label_55->setText(QString::number(t_Flowrate2,'f',4));
    //进口水压1
    ui->label_48->setText(QString::number(t_inwater1,'f',4));
    //进口水压2
    ui->label_49->setText(QString::number(t_inwater2,'f',4));
    //出口水压1
    ui->label_68->setText(QString::number(t_outwater1,'f',4));
    //出口水压2
    ui->label_71->setText(QString::number(t_outwater2,'f',4));
    //压降
    ui->label_80->setText(QString::number(t_yajiang1,'f',4));
    ui->label_83->setText(QString::number(t_yajiang2,'f',4));
    //气压1
    ui->label_63->setText(QString::number(t_qiya_1,'f',4));
    //气压2
    ui->label_65->setText(QString::number(t_qiya_2,'f',4));
    //图表添加数据
//    static double timeCount = 0;
//    const double timeStep = 0.02;
//    double currentTime = timeCount * timeStep;
    // 真实时间戳
    static qint64 startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double currentTime = (now - startTime) / 1000.0;  // 秒，绝对真实
    mTimeData.append(currentTime);
    mData1_1.append(t_pos1);
    mData1_2.append(t_pos2);
    mData2_1.append(t_vel1);
    mData2_2.append(t_vel2);
    mData2_3.append(t_acc1);
    mData2_4.append(t_acc2);
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








    if(max_current1 ==-0.111){max_current1=t_current1;}if(max_current1<t_current1){max_current1=t_current1;}
    if(max_current2 ==-0.111){max_current2=t_current2;}if(max_current2<t_current2){max_current2=t_current2;}
    if(max_tem1 ==-0.111){max_tem1=t_tem1;}if(max_tem1<t_tem1){max_tem1=t_tem1;}
    if(max_tem2 ==-0.111){max_tem2=t_tem2;}if(max_tem2<t_tem2){max_tem2=t_tem2;}
    if(final_max_current1 ==-0.111){final_max_current1=t_current1;}if(final_max_current1<t_current1){final_max_current1=t_current1;}
    if(final_max_current2 ==-0.111){final_max_current2=t_current2;}if(final_max_current2<t_current2){final_max_current2=t_current2;}
    if(final_max_tem1 ==-0.111){final_max_tem1=t_tem1;}if(final_max_tem1<t_tem1){final_max_tem1=t_tem1;}
    if(final_max_tem2 ==-0.111){final_max_tem2=t_tem2;}if(final_max_tem2<t_tem2){final_max_tem2=t_tem2;}


    ui->above_current1->setText(QString::number(final_max_current1,'f',4));
    ui->above_current2->setText(QString::number(final_max_current2,'f',4));
    ui->above_tem1->setText(QString::number(final_max_tem1,'f',4));
    ui->above_tem2->setText(QString::number(final_max_tem2,'f',4));
    ui->above_pos1->setText(QString::number(t_pos1, 'f', 3));
    ui->above_pos2->setText(QString::number(t_pos2, 'f', 3));




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
        if ((m_warnCurrent < std::abs(t_current1)) || (m_warnCurrent2 < std::abs(t_current2))) {
            bool isCritical = false;  // 标记是否触发断电

            // 处理电机1断电逻辑
            if (m_abnorCurrent < std::abs(t_current1)) {

                isCritical = true;
                ui->zsd_current1->setStyleSheet("QLabel { background-color: red; border-radius: 25px; border: 2px solid black; }");
                if(currentlogsign1)
                {
                    qWarning()<<"电机1当前电流为"+QString::number(t_current1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前电流为"+QString::number(t_current1)+",设备停机下使能");
                    currentlogsign1=false;
                }
            }

            // 处理电机2断电逻辑
            if (m_abnorCurrent2 < std::abs(t_current2)) {

                isCritical = true;
                ui->zsd_current2->setStyleSheet("QLabel { background-color: red; border-radius: 25px; border: 2px solid black; }");
                if(currentlogsign2)
                {
                    qWarning()<<"电机2当前电流为"+QString::number(t_current2)+",设备停机下使能";
                    logWindow->addLog("Warning","电机2当前电流为"+QString::number(t_current2)+",设备停机下使能");
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
            ui->zsd_current1->setStyleSheet((m_warnCurrent < std::abs(t_current1)) ?
                                                "QLabel { background-color: yellow; border-radius: 25px; border: 2px solid black; }" :
                                                "QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
            ui->zsd_current2->setStyleSheet((m_warnCurrent2 < std::abs(t_current2)) ?
                                                "QLabel { background-color: yellow; border-radius: 25px; border: 2px solid black; }" :
                                                "QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
            if(currentlogsign3)
            {
                qWarning()<<"电机1当前电流为"+QString::number(t_current1);
                qWarning()<<"电机2当前电流为"+QString::number(t_current2);
                qWarning()<<"电流过高";
                logWindow->addLog("Warning","电机1当前电流为"+QString::number(t_current1));
                logWindow->addLog("Warning","电机2当前电流为"+QString::number(t_current2));
                logWindow->addLog("Warning","电流过高");
                currentlogsign3=false;
            }
        }
        else {
            // 完全正常状态
            ui->test_status4->setStyleSheet("color: blue;");
            ui->test_status4->setText("正常");
            ui->zsd_current1->setStyleSheet("QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
            ui->zsd_current2->setStyleSheet("QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
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
        if((m_warnTemperature<t_tem1)||(m_warnTemperature2<t_tem2)||(t_temptc1>3420)||(t_temptc2>3420)){
            bool isCritical = false;  // 标记是否触发断电

            // 处理电机1断电逻辑
            if (m_abnorTemperature < std::abs(t_tem1)) {

                isCritical = true;
                ui->zsd_tem1->setStyleSheet("QLabel { background-color: red; border-radius: 25px; border: 2px solid black; }");
                if(temlogsign1)
                {
                    qWarning()<<"电机1当前温度为"+QString::number(t_tem1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前温度为"+QString::number(t_tem1)+",设备停机下使能");
                    temlogsign1=false;
                }

            }
            // 处理电机2断电逻辑
            if (m_abnorTemperature2 < std::abs(t_tem2)) {
                isCritical = true;
                ui->zsd_tem2->setStyleSheet("QLabel { background-color: red; border-radius: 25px; border: 2px solid black; }");
                if(temlogsign2)
                {
                    qWarning()<<"电机2当前温度为"+QString::number(t_tem2)+",设备停机下使能";
                    logWindow->addLog("Warning","电机2当前温度为"+QString::number(t_tem2)+",设备停机下使能");
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
            ui->zsd_tem1->setStyleSheet((m_warnTemperature < std::abs(t_tem1)) ?
                                            "QLabel { background-color: yellow; border-radius: 25px; border: 2px solid black; }" :
                                            "QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
            ui->zsd_tem2->setStyleSheet((m_warnTemperature2 < std::abs(t_tem2)) ?
                                            "QLabel { background-color: yellow; border-radius: 25px; border: 2px solid black; }" :
                                            "QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
            if(temlogsign3)
            {
                qWarning()<<"电机1当前温度为"+QString::number(t_tem1);
                qWarning()<<"电机2当前温度为"+QString::number(t_tem2);
                qWarning()<<"温度过高";
                logWindow->addLog("Warning","电机1当前温度为"+QString::number(t_tem1));
                logWindow->addLog("Warning","电机2当前温度为"+QString::number(t_tem2));
                logWindow->addLog("Warning","温度过高");
                temlogsign3=false;
            }
        }
        else {
            // 完全正常状态
            ui->test_status3->setStyleSheet("color: blue;");
            ui->test_status3->setText("正常");
            ui->zsd_tem1->setStyleSheet("QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
            ui->zsd_tem2->setStyleSheet("QLabel { background-color: green; border-radius: 25px; border: 2px solid black; }");
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
        if((m_shuiliuliang1>t_Flowrate1)||(m_shuiliuliang2>t_Flowrate2)){
            if(m_shuiliuliang1>t_Flowrate1){
                //disableall2();
                stopstop();
                acsc_DisableAll(hComm,NULL);
                acsc_KillAll(hComm, NULL);
                ui->label_50->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");

                ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: red; ""border-radius: 25px; " "border: 2px solid black; ""}");
                if(flowlogsign1)
                {
                    qWarning()<<"电机1当前水流量为"+QString::number(t_Flowrate1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前水流量为"+QString::number(t_Flowrate1)+"设备停机下使能");
                    flowlogsign1=false;
                }
            }else
            {
                ui->label_50->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
                ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
                if(!flowlogsign1)
                {
                    qInfo()<<"电机1当前水流量恢复正常";
                    logWindow->addLog("Info","电机1当前水流量恢复正常");
                    flowlogsign1=true;
                }
            }

            if(m_shuiliuliang2>t_Flowrate2){
                //disableall2();
                stopstop();
                acsc_DisableAll(hComm,NULL);
                acsc_KillAll(hComm, NULL);
                ui->label_55->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");
                ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: red; ""border-radius: 25px; " "border: 2px solid black; ""}");
                if(flowlogsign2)
                {
                    qWarning()<<"电机2当前水流量为"+QString::number(t_Flowrate2)+"设备停机下使能";
                    logWindow->addLog("Warning","电机2当前水流量为"+QString::number(t_Flowrate2)+"设备停机下使能");
                    flowlogsign2=false;
                }
            }
            else
            {
                ui->label_55->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
                ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
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
            ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");

            ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
        }





        //这里判断气压
        if((m_atm1>t_qiya_1)||(m_atm2>t_qiya_2)){
            if(m_atm1>t_qiya_1){
                //disableall2();
                stopstop();
                acsc_DisableAll(hComm,NULL);
                acsc_KillAll(hComm, NULL);
                ui->label_63->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");
                ui->zsd_air1->setStyleSheet("QLabel { ""background-color: red; ""border-radius: 25px; " "border: 2px solid black; ""}");
                if(atmlogsign1)
                {
                    qWarning()<<"电机1当前气压为"+QString::number(t_qiya_1)+",设备停机下使能";
                    logWindow->addLog("Warning","电机1当前气压为"+QString::number(t_qiya_1)+",设备停机下使能");
                    atmlogsign1=false;
                }
            }else
            {
                ui->label_63->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
                if(!atmlogsign1)
                {
                    qInfo()<<"电机1当前气压恢复正常";
                    logWindow->addLog("Info","电机1当前气压恢复正常");
                    atmlogsign1=true;
                }
            }

            if(m_atm2>t_qiya_2){
                //disableall2();
                stopstop();
                acsc_DisableAll(hComm,NULL);
                acsc_KillAll(hComm, NULL);
                ui->label_65->setStyleSheet("QLabel {"" color: yellow; "" background-color: red;""}");
                ui->zsd_air2->setStyleSheet("QLabel {"" background-color: red;""border-radius: 25px; " "border: 2px solid black; ""}");
                if(atmlogsign2)
                {
                    qWarning()<<"电机2当前气压为"+QString::number(t_qiya_2)+",设备停机下使能";
                    logWindow->addLog("Warning","电机2当前气压为"+QString::number(t_qiya_2)+",设备停机下使能");
                    atmlogsign2=false;
                }
            }
            else
            {
                ui->label_65->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
                if(!atmlogsign2)
                {
                    qInfo()<<"电机2当前气压恢复正常";
                    logWindow->addLog("Info","电机2当前气压恢复正常");
                    atmlogsign2=true;
                }
            }
        }else
        {
            ui->label_63->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
            ui->zsd_air1->setStyleSheet("QLabel {"" background-color: green;""border-radius: 25px; " "border: 2px solid black; ""}");
            ui->label_65->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
            ui->zsd_air2->setStyleSheet("QLabel {"" background-color: green;""border-radius: 25px; " "border: 2px solid black; ""}");
        }
    }
    else
    {
        ui->test_status4->setStyleSheet("color:blue;");
        ui->test_status4->setText("正常");
        ui->zsd_current1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->zsd_current2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");        //恢复正常


        ui->test_status3->setStyleSheet("color:blue;");
        ui->test_status3->setText("正常");
        ui->zsd_tem1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->zsd_tem2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");
        ui->label_50->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        ui->label_55->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");
        ui->zsd_flow1->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");

        ui->zsd_flow2->setStyleSheet("QLabel { ""background-color: green; ""border-radius: 25px; " "border: 2px solid black; ""}");

        ui->label_63->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");

        ui->zsd_air1->setStyleSheet("QLabel {"" background-color: green;""border-radius: 25px; " "border: 2px solid black; ""}");

        ui->label_65->setStyleSheet("QLabel {"" color: black; "" background-color: white;""}");

        ui->zsd_air2->setStyleSheet("QLabel {"" background-color: green;""border-radius: 25px; " "border: 2px solid black; ""}");
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

//void Widget::on_dingwei5_clicked()
//{
//    double yMax;
//    double yMin;
//    double xMax;
//    double xMin;
//    QDialog dialog(this);
//    //dialog.setWindowTitle("设置坐标范围");

//    // 创建布局
//    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

//    // 创建4个输入行
//    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
//    QLineEdit *yMinEdit = new QLineEdit(&dialog);
//    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
//    QLineEdit *xMinEdit = new QLineEdit(&dialog);

//    // 添加标签和输入框
//    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
//    mainLayout->addWidget(yMaxEdit);
//    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
//    mainLayout->addWidget(yMinEdit);
//    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
//    mainLayout->addWidget(xMaxEdit);
//    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
//    mainLayout->addWidget(xMinEdit);

//    // 添加确定按钮
//    QPushButton *okButton = new QPushButton("确定", &dialog);
//    mainLayout->addWidget(okButton);

//    // 连接按钮信号
//    QObject::connect(okButton, &QPushButton::clicked, [&]() {
//        // 获取输入值
//        yMax = yMaxEdit->text().toDouble();
//        yMin = yMinEdit->text().toDouble();
//        xMax = xMaxEdit->text().toDouble();
//        xMin = xMinEdit->text().toDouble();

//        // 打印结果
//        qDebug() << "输入的坐标范围:";
//        qDebug() << "Y最大值:" << yMax;
//        qDebug() << "Y最小值:" << yMin;
//        qDebug() << "X最大值:" << xMax;
//        qDebug() << "X最小值:" << xMin;

//        dialog.accept(); // 关闭对话框
//    });

//    // 显示对话框
//    dialog.exec();
//    mPlot5->xAxis->setRange(xMin, xMax);
//    mPlot5->yAxis->setRange(yMin, yMax);
//    mPlot5->yAxis2->setRange(yMin, yMax);
//    mPlot5->replot();
//}
void Widget::on_dingwei5_clicked()
{
    double yMax;
    double yMin;
    double xMax;
    double xMin;
    QDialog dialog(this);
    dialog.setWindowTitle("设置坐标范围");  // 添加对话框标题

    // 使用网格布局（3行4列）
    QGridLayout *mainLayout = new QGridLayout(&dialog);

    // 第一行：X轴范围
    mainLayout->addWidget(new QLabel("X轴范围:", &dialog), 0, 0);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    mainLayout->addWidget(xMinEdit, 0, 1);
    mainLayout->addWidget(new QLabel("~", &dialog), 0, 2);
    mainLayout->addWidget(xMaxEdit, 0, 3);

    // 第二行：Y轴范围
    mainLayout->addWidget(new QLabel("Y轴范围:", &dialog), 1, 0);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    mainLayout->addWidget(yMinEdit, 1, 1);
    mainLayout->addWidget(new QLabel("~", &dialog), 1, 2);
    mainLayout->addWidget(yMaxEdit, 1, 3);

    // 第三行：确定按钮（跨列+加大尺寸）
    QPushButton *okButton = new QPushButton("确定", &dialog);
    okButton->setFixedHeight(40);  // 设置固定高度
    okButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // 水平扩展
    QFont font = okButton->font();
    font.setPointSize(12);  // 加大字体
    okButton->setFont(font);
    okButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; }"
                           "QPushButton:hover { background-color: #45a049; }");  // 添加悬停效果
    mainLayout->addWidget(okButton, 2, 0, 1, 4);  // 跨4列

    // 连接按钮信号
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        qDebug() << "X轴范围:" << xMin << "~" << xMax;
        qDebug() << "Y轴范围:" << yMin << "~" << yMax;

        dialog.accept();
    });

    // 显示对话框并更新图表
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
    //dialog.setWindowTitle("设置坐标范围");

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // 创建4个输入行
    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    // 添加标签和输入框
    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    // 连接按钮信号
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        // 获取输入值
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        // 打印结果
        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept(); // 关闭对话框
    });

    // 显示对话框
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
    //dialog.setWindowTitle("设置坐标范围");

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // 创建4个输入行
    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    // 添加标签和输入框
    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    // 连接按钮信号
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        // 获取输入值
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        // 打印结果
        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept(); // 关闭对话框
    });

    // 显示对话框
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
    //dialog.setWindowTitle("设置坐标范围");

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // 创建4个输入行
    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    // 添加标签和输入框
    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    // 连接按钮信号
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        // 获取输入值
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        // 打印结果
        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept(); // 关闭对话框
    });

    // 显示对话框
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
    //dialog.setWindowTitle("设置坐标范围");

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // 创建4个输入行
    QLineEdit *yMaxEdit = new QLineEdit(&dialog);
    QLineEdit *yMinEdit = new QLineEdit(&dialog);
    QLineEdit *xMaxEdit = new QLineEdit(&dialog);
    QLineEdit *xMinEdit = new QLineEdit(&dialog);

    // 添加标签和输入框
    mainLayout->addWidget(new QLabel("Y最大值:", &dialog));
    mainLayout->addWidget(yMaxEdit);
    mainLayout->addWidget(new QLabel("Y最小值:", &dialog));
    mainLayout->addWidget(yMinEdit);
    mainLayout->addWidget(new QLabel("X最大值:", &dialog));
    mainLayout->addWidget(xMaxEdit);
    mainLayout->addWidget(new QLabel("X最小值:", &dialog));
    mainLayout->addWidget(xMinEdit);

    // 添加确定按钮
    QPushButton *okButton = new QPushButton("确定", &dialog);
    mainLayout->addWidget(okButton);

    // 连接按钮信号
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        // 获取输入值
        yMax = yMaxEdit->text().toDouble();
        yMin = yMinEdit->text().toDouble();
        xMax = xMaxEdit->text().toDouble();
        xMin = xMinEdit->text().toDouble();

        // 打印结果
        qDebug() << "输入的坐标范围:";
        qDebug() << "Y最大值:" << yMax;
        qDebug() << "Y最小值:" << yMin;
        qDebug() << "X最大值:" << xMax;
        qDebug() << "X最小值:" << xMin;

        dialog.accept(); // 关闭对话框
    });

    // 显示对话框
    dialog.exec();
    mPlot4->xAxis->setRange(xMin, xMax);
    mPlot4->yAxis->setRange(yMin, yMax);
    mPlot4->replot();
}


void Widget::handleWaveDataUpdate()//这里更新图表槽函数
{
    //ui->above_num->setText(QString::number(lastrecyclenum));
    QString a=QString::number(allrecyclenum);
    ui->above_all_num->setText(a);
    writeTestNum(a);

    QList<QString> datalist;
    //运行时间
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
        //添加循环次数图表数据

        //图表添加数据
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
        //    mData4_1.append(pos1+3);
        //    mData4_2.append(pos2);
        //    mData5_1.append(pos1+1);
        //    mData5_2.append(pos2+2);
        //    mData5_3.append(pos1+3);
        //    mData5_4.append(pos2+4);
        //    mData5_5.append(pos1+5);
        //    mData5_6.append(pos2+6);
        //    mData5_7.append(pos1+7);
        //    mData5_8.append(pos2+8);

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
    // 1. 获取当前时间差
    QTime current = QTime::currentTime();
    int currentMsecs = this->xunhuanstarttime.msecsTo(current);

    // 2. 解析历史时间 oldtime (格式: "days.hh:mm:ss:zzz")
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

    // 3. 计算总时间（当前 + 历史）
    int totalMsecs = currentMsecs + oldMsecs;
    int totalDays = oldDays + (totalMsecs / (1000 * 60 * 60 * 24));
    totalMsecs %= (1000 * 60 * 60 * 24);

    // 4. 格式化为显示字符串
    QTime showTime = QTime(0, 0, 0, 0).addMSecs(totalMsecs);
    QString showStr = QString::number(totalDays) + "." + showTime.toString("hh:mm:ss:zzz");

    // 5. 更新UI并保存
    ui->above_time->setText(showStr);
    writeTestTime(showStr);
}

void Widget::checkcsv()
{
    QMutexLocker locker(&m_filenameMutex);
    QFileInfo fileInfo2(logname);

    if (fileInfo2.exists())
    {
        qint64 fileSize = fileInfo2.size(); // 获取文件大小，单位为字节
        if(fileSize>logsize)
        {

            logname=createLog();
        }

    }

    QFileInfo fileInfo(filename);

    if (fileInfo.exists())
    {
        qint64 fileSize = fileInfo.size(); // 获取文件大小，单位为字节
        csvsize=(ui->line_set_csv_size->text().toInt())*1000;
        if(fileSize>csvsize)
        {
            checkcsv_timer->stop();
            issaveincsv=0;
            int a=ui->line_set_save_s->text().toDouble();
            QTimer::singleShot(a*60 * 1000, this, [this]() {
                qDebug() << "等待时间到了";
                filename = createcsv(); // 创建新的CSV文件
                xunhuanfilename=createxunhuancsv();
                issaveincsv=1;
                checkcsv_timer->start(1000); // 重新启动计时器
            });
        }

    }

}



double Widget::getAInvalue(char* str)
{
    char* cmd=str;
    char buf[100];
    int Received;
    acsc_Transaction(hComm,cmd,strlen(cmd),buf,100,&Received,NULL);
    //qDebug()<<"Received = "<<Received;
    buf[Received] = '\0';
    QString dataStr = QString::fromUtf8(buf).trimmed().replace("\r", "");
    // 尝试将清理后的字符串转换为 double 类型
    bool ok;
    double value = dataStr.toDouble(&ok);
    return value;
}


//页眉切换界面
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

// 鼠标按下事件：记录拖动起始位置
void Widget::mousePressEvent(QMouseEvent *event)
{
    // 只有在非最大化状态下才允许拖动
    if (!isFullScreen() && event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStartPosition = event->globalPos() - this->pos(); // 使用 globalPos()
        event->accept();
    }
    QWidget::mousePressEvent(event); // 确保其他事件正常处理
}

// 鼠标移动事件：更新窗口位置
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    // 只有在非最大化状态下才允许拖动
    if (!isFullScreen() && m_isDragging && event->buttons() & Qt::LeftButton) {
        QPoint newPos = event->globalPos() - m_dragStartPosition; // 使用 globalPos()
        this->move(newPos);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

// 鼠标释放事件：结束拖动
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    // 只有在非最大化状态下才处理拖动结束
    if (!isFullScreen() && event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
    QWidget::mouseReleaseEvent(event);
}


//系统最小化
void Widget::on_system_min_clicked()
{
    this->showMinimized();
}

//系统全屏
void Widget::on_system_max_clicked()
{
    if (this->windowState() & Qt::WindowFullScreen) {
        this->showNormal(); // 还原窗口
        QIcon icon(":/image/restore.png");
        ui->system_max->setIcon(icon);
    } else {
        this->showFullScreen(); // 全屏显示
        QIcon icon(":/image/max.png");
        ui->system_max->setIcon(icon);
    }
}

//系统退出
void Widget::on_system_exit_clicked()
{
    if (logWindow) {
        logWindow->close(); // 关闭日志窗口
        delete logWindow;   // 删除日志窗口
    }
    this->close();
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




//开始界面使能
void Widget::on_start_enable_clicked()
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

//开始界面下使能
void Widget::on_start_disable_clicked()
{
    t_addlog(1,"电机下使能");
    if(final_axis==2)
    {
        acsc_DisableAll(hComm,NULL);
    }else{
        acsc_Disable(hComm,final_axis,NULL);
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
            acsc_SetVelocity(hComm,ACSC_AXIS_0,prot_vel*coefficient, NULL );
            acsc_SetVelocity(hComm,ACSC_AXIS_1,prot_vel*coefficient, NULL );
        }else{
            acsc_SetVelocity(hComm,final_axis,prot_vel*coefficient, NULL );
        }
        ui->line_test_vel->setText(QString::number(prot_vel));
    }else
    {
        if(final_axis==2)
        {
            acsc_SetVelocity(hComm,ACSC_AXIS_0,vel*coefficient, NULL );
            acsc_SetVelocity(hComm,ACSC_AXIS_1,vel*coefficient, NULL );
        }else{
            acsc_SetVelocity(hComm,final_axis,vel*coefficient, NULL );
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
            acsc_SetAcceleration(hComm,ACSC_AXIS_0,prot_accdec*coefficient, NULL );
            acsc_SetAcceleration(hComm,ACSC_AXIS_1,prot_accdec*coefficient, NULL );

            acsc_SetDeceleration(hComm,ACSC_AXIS_0,prot_accdec*coefficient, NULL );
            acsc_SetDeceleration(hComm,ACSC_AXIS_1,prot_accdec*coefficient, NULL );
        }else{
            acsc_SetAcceleration(hComm,final_axis,prot_accdec*coefficient, NULL );

            acsc_SetDeceleration(hComm,final_axis,prot_accdec*coefficient, NULL );
        }
        ui->line_test_accdec->setText(QString::number(prot_accdec));

    }
    else
    {
        if(final_axis==2)
        {
            acsc_SetAcceleration(hComm,ACSC_AXIS_0,accdec*coefficient, NULL );
            acsc_SetAcceleration(hComm,ACSC_AXIS_1,accdec*coefficient, NULL );
            acsc_SetDeceleration(hComm,ACSC_AXIS_0,accdec*coefficient, NULL );
            acsc_SetDeceleration(hComm,ACSC_AXIS_1,accdec*coefficient, NULL );
        }else{
            acsc_SetAcceleration(hComm,final_axis,accdec*coefficient, NULL );
            acsc_SetDeceleration(hComm,final_axis,accdec*coefficient, NULL );
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

//测试界面开始循环运动
void Widget::on_test_start_move_clicked()
{
    bool ok;
    if(ui->line_test_movenum->text().contains('/'))
    {
        QString a = ui->line_test_movenum->text();
        QString b = a.section('/', -1); // 提取最后一个\后的内容
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
    cycle_pointA=-(cycle_pointA+100);
    cycle_pointB = ui->line_test_B->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "非法输入", "请正确输入行程点B");
        return;
    }
    cycle_pointB=-(cycle_pointB+100);
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
        //QMessageBox::warning(this,"超范围","行程超出运动范围阈值");
        //return ;
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

            // 同时输出到日志和调试控制台
            //qWarning() << errorMsg;
            //t_addlog(2, errorMsg);

            return;
    }
    t_addlog(1,"开始循环运动");
    startcyclenum=cycle_num;
    //开始运动前锁住运行按钮，防止连点
    ui->test_start_move->setEnabled(false);
    if (future.isRunning()) {
        future.cancel(); // 如果之前的任务还在运行，先取消它
        //future.waitForFinished(); // 等待任务完全取消
        int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1 };
        acsc_HaltM(hComm,Axes,nullptr);
    }
    //默认低速回零
    double oldspeed=ui->line_test_vel->text().toDouble();
    double oldacc_dec=ui->line_test_accdec->text().toDouble();

    if(final_axis==2)
    {
        acsc_SetVelocity(hComm,ACSC_AXIS_0,100*coefficient, NULL );
        acsc_SetVelocity(hComm,ACSC_AXIS_1,100*coefficient, NULL );
    }else{
        acsc_SetVelocity(hComm,final_axis,100*coefficient, NULL );
    }
    if(final_axis==2)
    {
        acsc_SetAcceleration(hComm,ACSC_AXIS_0,1000*coefficient, NULL );
        acsc_SetAcceleration(hComm,ACSC_AXIS_1,1000*coefficient, NULL );

        acsc_SetDeceleration(hComm,ACSC_AXIS_0,1000*coefficient, NULL );
        acsc_SetDeceleration(hComm,ACSC_AXIS_1,1000*coefficient, NULL );
    }else{
        acsc_SetAcceleration(hComm,final_axis,1000*coefficient, NULL );

        acsc_SetDeceleration(hComm,final_axis,1000*coefficient, NULL );
    }
    if(final_axis==2)
    {
        acsc_ToPoint(hComm,0,0,-100,NULL);
        acsc_ToPoint(hComm,0,1,-100,NULL);
    }else{
        acsc_ToPoint(hComm,0,final_axis,-100,NULL);}
    if(final_axis==2)
    {
        acsc_WaitLogicalMotionEnd(hComm,0,30000);
        acsc_WaitLogicalMotionEnd(hComm,1,30000);
    }else{
        acsc_WaitLogicalMotionEnd(hComm,final_axis,30000);}
    if(final_axis==2)
    {
        acsc_SetVelocity(hComm,ACSC_AXIS_0,oldspeed*coefficient, NULL );
        acsc_SetVelocity(hComm,ACSC_AXIS_1,oldspeed*coefficient, NULL );
    }else{
        acsc_SetVelocity(hComm,final_axis,oldspeed*coefficient, NULL );
    }
    if(final_axis==2)
    {
        acsc_SetAcceleration(hComm,ACSC_AXIS_0,oldacc_dec*coefficient, NULL );
        acsc_SetAcceleration(hComm,ACSC_AXIS_1,oldacc_dec*coefficient, NULL );

        acsc_SetDeceleration(hComm,ACSC_AXIS_0,oldacc_dec*coefficient, NULL );
        acsc_SetDeceleration(hComm,ACSC_AXIS_1,oldacc_dec*coefficient, NULL );
    }else{
        acsc_SetAcceleration(hComm,final_axis,oldacc_dec*coefficient, NULL );

        acsc_SetDeceleration(hComm,final_axis,oldacc_dec*coefficient, NULL );
    }
    //准备开始循环运动
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
    flag=true;//是否进行循环标志
    auto func=[&](){
        while (flag && m_recycleMoveNum < cycle_num) {
            if(final_axis==2)
            {
                int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1};
                double aa=cycle_pointA;
                double Points[] = {aa, aa};
                acsc_ToPointM(hComm,0,Axes,Points,NULL );
            }else if(final_axis==0){
                acsc_ToPoint(hComm,0,final_axis,cycle_pointA,NULL);}
            else if(final_axis==1){
                acsc_ToPoint(hComm,1,final_axis,cycle_pointA,NULL);}
            else
            {

            }
            if(final_axis==2)
            {
                acsc_WaitLogicalMotionEnd(hComm,0,30000);
                acsc_WaitLogicalMotionEnd(hComm,1,30000);
            }else{
                acsc_WaitLogicalMotionEnd(hComm,final_axis,30000);}
            QThread::msleep(cycle_waittime);
            if(final_axis==2)
            {
                int Axes[] = { ACSC_AXIS_0, ACSC_AXIS_1,-1};
                double aa=cycle_pointB;
                double Points[] = {aa, aa};
                acsc_ToPointM(hComm,0,Axes,Points,NULL );
            }else if(final_axis==0){
                acsc_ToPoint(hComm,0,final_axis,cycle_pointB,NULL);}
            else if(final_axis==1){
                acsc_ToPoint(hComm,1,final_axis,cycle_pointB,NULL);}
            else
            {

            }
            if(final_axis==2)
            {
                acsc_WaitLogicalMotionEnd(hComm,0,30000);
                acsc_WaitLogicalMotionEnd(hComm,1,30000);
            }else{
                acsc_WaitLogicalMotionEnd(hComm,final_axis,30000);}
            QThread::msleep(cycle_waittime);
            emit updateWaveData();//发送信号
            m_recycleMoveNum++;
            lastrecyclenum=m_recycleMoveNum;
            ui->line_test_movenum->setText(QString::number(lastrecyclenum)+"/"+QString::number(startcyclenum));
            allrecyclenum++;
        }
        xunhuantimer->stop();
        xunhuanpausetime=QTime::currentTime();
        m_recycleMoveNum=0;
        ui->test_stop2->click();
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
    }else{acsc_Halt(hComm,final_axis,NULL);}

    flag=false;
    future.waitForFinished();
    m_recycleMoveNum=0;


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

    }else if(final_axis==0){
        acsc_ToPoint(hComm,0,final_axis,location,NULL);}
    else if(final_axis==1){
        acsc_ToPoint(hComm,0,final_axis,location,NULL);}
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
    if(final_axis==2)
    {
        acsc_ToPoint(hComm,ACSC_AMF_RELATIVE,0,location,NULL);
        acsc_ToPoint(hComm,ACSC_AMF_RELATIVE,1,-location,NULL);

    }else if(final_axis==0) {
        acsc_ToPoint(hComm,ACSC_AMF_RELATIVE,final_axis,location,NULL);}
    else if(final_axis==1) {
        acsc_ToPoint(hComm,ACSC_AMF_RELATIVE,final_axis,-location,NULL);}
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
        acsc_HaltM(hComm,Axes,NULL);
    }else{acsc_Halt(hComm,final_axis,NULL);}

    flag=false;
    future.waitForFinished();
    m_recycleMoveNum=0;
}


void Widget::on_btn_set_load_set_clicked()
{
    t_addlog(1,"软件参数加载");
    // 弹出文件打开对话框，让用户选择 XML 文件
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open XML File"), QDir::homePath(), tr("XML Files (*.xml)"));

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Operation Cancelled", "No file selected.");
        return;
    }

    // 加载 XML 文件
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

    // 获取根节点
    QDomElement root = doc.documentElement();

    // 检查根节点是否为 "Protect_Parameters"
    if (root.tagName() != "Protect_Parameters") {
        QMessageBox::warning(this, "Invalid File", "请选择保护参数文件，而不是运动参数文件");
        return;
    }

    // 读取子节点的值并更新 UI 控件
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


    // 创建 QDomDocument 对象
    QDomDocument doc;
    QDomElement root = doc.createElement("Protect_Parameters");  // 创建根节点
    doc.appendChild(root);

    // 创建子节点并添加数据
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





    // 弹出文件保存对话框，让用户选择保存路径和文件名
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
    // 获取根节点
    QDomElement root = doc2.documentElement();

    // 检查根节点是否为 "Protect_Parameters"
    if (root.tagName() != "Protect_Parameters") {
        QMessageBox::warning(this, "Invalid File", "请选择正确文件");
        return;
    }




    // 读取子节点的值并更新 UI 控件
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
    // 弹出目录选择对话框，获取新的路径
    QString newPath = QFileDialog::getExistingDirectory(
                this, tr("选择目录"),
                QDir::homePath() // 或者使用QDir::currentPath()
                );

    // 如果用户取消了选择，则返回
    if (newPath.isEmpty()) return;
    // 构建新的文件内容
    QString newContent;
    newContent += "datapath=" + newPath + "\n";
    if (!imagepathLine.isEmpty()) {
        newContent += imagepathLine + "\n";
    }
    // 写入新内容
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

    // 弹出目录选择对话框，获取新的路径
    QString newPath = QFileDialog::getExistingDirectory(
                this, tr("选择目录"),
                QDir::homePath() // 或者使用QDir::currentPath()
                );

    // 如果用户取消了选择，则返回
    if (newPath.isEmpty()) return;

    // 构建新的文件内容
    QString newContent;
    newContent += "imagepath=" + newPath + "\n";
    if (!imagepathLine.isEmpty()) {
        newContent += imagepathLine + "\n";
    }

    // 写入新内容
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
                passwordAdmin = line.section('=', 1, 1).trimmed(); // 提取等号后面的密码部分
            }
            else if (line.startsWith("password_Tester"))
            {
                passwordTester = line.section('=', 1, 1).trimmed(); // 提取等号后面的密码部分
            }
        }
        file.close();
        //以上获取文件内的密码
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

        // 替换为 QLineEdit 用于输入任意字符串
        QLineEdit *lineEdit = new QLineEdit(&dialog);
        lineEdit->setEchoMode(QLineEdit::Password);
        form.addRow(lineEdit);

        // Add Cancel and OK button
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);

        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, [&]() {
            QString inputPassword = lineEdit->text(); // 获取 QLineEdit 的值

            qDebug() << "Input Password:" << inputPassword; // 打印值
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
            dialog.accept(); // 关闭对话框
        });

        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, [&]() {
            dialog.reject(); // 关闭对话框
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
    bool ok = false; // 应该初始化ok变量
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
    //dialog.setMinimumSize(500, 80);  // 关键！强制设置最小尺寸



    dialog.setWindowTitle("输入测试编号名");
    dialog.setLabelText("请输入测试编号名:");
    dialog.setInputMode(QInputDialog::TextInput);

    QString folderName;
    if (dialog.exec() == QDialog::Accepted) {
        folderName = dialog.textValue();
        ok = true; // 用户点击了确定

        if (!folderName.isEmpty()) {
            //qDebug() << "用户输入的编号名:" << folderName;
            // 检查是否包含非法字符（Windows/Linux 文件名限制）
            QString invalidChars = "<>:\"/\\|?*";  // Windows 不允许的字符
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

            // 检查是否以空格或点开头/结尾
            if (folderName.startsWith(" ") || folderName.startsWith(".") ||
                    folderName.endsWith(" ") || folderName.endsWith(".")) {
                QMessageBox::warning(
                            nullptr,
                            "错误",
                            "编号名不能以空格或点开头/结尾！"
                );
                return;
            }

            // 检查是否为空（用户可能只输入空格）
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
        return;  // 用户取消输入或输入为空
    }

    QString value=folderName;
    // 示例：创建文件夹（需包含 <QDir>）
    // QDir().mkdir(folderName);
    //    t_addlog(1,"修改测试编号");
    //    bool ok;
    //    int value = QInputDialog::getInt(
    //                nullptr, "输入整数", "请输入一个整数:",
    //                0, -100, 100, 1, &ok
    //                );

    //    if (!ok) return;  // 用户取消输入，直接返回

    QString currentDir = QCoreApplication::applicationDirPath();
    QString filePath = currentDir + "/testnumber.txt";

    // 读取文件内容
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for reading:" << filePath;
        return;
    }

    QTextStream in(&file);
    QStringList lines;  // 存储所有行
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("testnumber=")) {
            line = "testnumber=" + value;  // 修改目标行
        }
        lines << line;  // 保留所有行
    }
    file.close();

    // 重新写入文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file for writing:" << filePath;
        return;
    }

    QTextStream out(&file);
    for (const QString &line : lines) {
        out << line << "\n";  // 写入所有行
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

    // 读取datapath
    QString dataPath;
    QFile configFile(configPath);
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&configFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("datapath=")) {
                dataPath = line.mid(9); // 去掉"datapath="
                break;
            }
        }
        configFile.close();
    }

    if (dataPath.isEmpty()) {
        qWarning() << "无法获取datapath配置";
        return "";
    }

    // 构建完整路径
    QDir dataDir(dataPath);
    QString dataFolderPath = dataDir.absoluteFilePath("data");
    QString testNumber = gettestnumber(); // 假设这个函数可用
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
            line = "testnum=" + value; // 替换原有的 testnum 行
        }
        lines << line; // 将处理后的行保存到列表中
    }

    // 如果文件中没有 testnum= 行，则添加一行
    if (lines.filter("testnum=").isEmpty()) {
        lines << "testnum=" + value;
    }

    // 清空文件并写入新的内容
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
            return line.mid(8); // 或 line.split('=').value(1, "0");
        }
    }
    file.close();
    return "0";
}
QTime Widget::parseSavedTime(const QString &timeStr) {
    // 格式示例: "1.23:59:59:999" (天数.时:分:秒:毫秒)
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
    // 1. 截取frame内容
    QPixmap screenshot = frame->grab();

    // 2. 读取配置文件路径
    QString currentDir = QCoreApplication::applicationDirPath();
    QString configPath = currentDir + "/savepath.txt";

    QString dataPath;
    QFile configFile(configPath);
    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&configFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("imagepath=")) {
                dataPath = line.mid(9); // 去掉"datapath="前缀
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
    // 3. 构建保存目录
    QDir dataDir(datapath2);
    QString imageFolderPath = dataDir.absoluteFilePath("image");

    // 如果目录不存在则创建
    if (!QDir(imageFolderPath).exists()) {
        if (!dataDir.mkdir("image")) {
            qWarning() << "创建图片目录失败:" << imageFolderPath;
            return;
        }
    }

    // 4. 生成带测试编号的子目录
    QString testNumber = gettestnumber(); // 假设这个函数可用
    QString targetFolder = imageFolderPath + "/" + testNumber;
    if (!QDir(targetFolder).exists()) {
        QDir().mkpath(targetFolder);
    }

    // 5. 生成带帧数和时间戳的文件名
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timestamp = currentTime.toString("yyyyMMdd_HHmmss_zzz"); // 例如: 20231115_143045_123
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
    // 6. 保存图片
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
            return; // 如果目录创建失败，直接返回
        }
    }

    QString folderPath = dataFolderPath + "/" + gettestnumber();
    QDir dir(folderPath);
    QFileInfoList files = dir.entryInfoList(QStringList() << "*Cyclic_Motion*.csv", QDir::Files);
    int n = files.size();
    qDebug() << "找到的CSV文件数量:" << n;

    // 计算所有文件的有效行数总和
    QList<int> validLineCounts;
    for (const QFileInfo &fileInfo : files) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "无法打开文件:" << file.fileName();
            validLineCounts.append(0);
            continue;
        }

        QTextStream in(&file);
        in.readLine(); // 跳过前两行
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

    // 计算总有效行数
    int totalValidLines = 0;
    for (int count : validLineCounts) {
        totalValidLines += count;
    }

    // 确定最终需要的行数
    const int TOTAL_NEEDED = 2000;
    QList<int> rowsPerFile;
    if (totalValidLines <= TOTAL_NEEDED) {
        // 如果总有效行数不足2000，获取所有数据
        for (int i = 0; i < n; ++i) {
            rowsPerFile.append(validLineCounts[i]);
        }
    } else {
        // 均匀分配抽取的行数
        for (int i = 0; i < n; ++i) {
            rowsPerFile.append(static_cast<int>(static_cast<double>(validLineCounts[i]) / totalValidLines * TOTAL_NEEDED));
        }

        // 调整分配，确保总和为2000
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

    // 读取并抽取数据
    for (int i = 0; i < files.size(); ++i) {
        if (rowsPerFile[i] <= 0) continue;

        QFile file(files[i].absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "无法打开文件:" << file.fileName();
            continue;
        }

        QTextStream in(&file);
        in.readLine(); // 跳过前两行
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

        // 抽取数据
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

    // 确保最终结果数量为2000或全部数据
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

    // 当前状态变为未选中且之前是选中状态
    if (arg1 == Qt::Unchecked && previousState == Qt::Checked) {
        // 创建消息框
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("警告"));
        msgBox.setText(tr("保护系统将关闭！"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);

        // 设置样式表使弹窗变大
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

        // 强制设置最小尺寸
        msgBox.setMinimumSize(400, 200);

        msgBox.exec();
    }

    // 更新前一个状态
    previousState = arg1;
}
