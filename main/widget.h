#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QPixmap>
#include "ttools.h"
#include <QList>
#include "ACSC.h"
#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "qcustomplot.h"
#include "logwindow.h"
#include "savelog.h"
#include "TemperatureModule.h"
#include "ITCPDevice.h"
#include "ModbusTemperatureServer.h"
#include <algorithm>
#include <atomic>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class DataAcquisitionService;

static double getAInvalue(HANDLE hComm, char* str);

struct SampleFrame
{
    qint64 timeStampMs{0};
    bool temperatureValid{false};
    int state0{0};
    int state1{0};
    int movestate0{0};
    int movestate1{0};
    double pos1{0};
    double pos2{0};
    double vel1{0};
    double vel2{0};
    double acc1{0};
    double acc2{0};
    double current1{0};
    double current2{0};
    double vol1{0};
    double vol2{0};

    double tem1{0};
    double tem2{0};
    double temptc1{0};
    double temptc2{0};

    double ntc1_1{0}, ntc1_2{0}, ntc1_3{0}, ntc1_4{0};
    double ntc2_1{0}, ntc2_2{0}, ntc2_3{0}, ntc2_4{0};

    double ptc1_1{0}, ptc1_2{0};
    double ptc2_1{0}, ptc2_2{0};

    double Flowrate1{0};
    double Flowrate2{0};
    double inwater1{0};
    double inwater2{0};
    double outwater1{0};
    double outwater2{0};
    double qiya{0};
    double qiliu{0};

};

template<typename T>
class RingBuffer
{
public:
    explicit RingBuffer(int maxSize = 2000)
        : m_maxSize(maxSize)
    {
        m_buf.resize(maxSize);
    }

    bool push(const T& item)
    {
        QMutexLocker lock(&m_mutex);
        if(m_count >= m_maxSize)
        {
            m_head = (m_head +1) % m_maxSize;
            --m_count;
        }
        m_buf[m_tail] = item;
        m_tail = (m_tail +1) % m_maxSize;
        ++m_count;
        m_cond.wakeOne();
        return true;
    }

    QVector<T> popBatch(int count)
    {
        QMutexLocker lock(&m_mutex);
        QVector<T> res;
        res.reserve(count);
        while(m_count >0 && res.size() < count)
        {
            res.append(m_buf[m_head]);
            m_head = (m_head +1) % m_maxSize;
            --m_count;
        }
        return res;
    }

    bool getLatest(T& out)
    {
        QMutexLocker lock(&m_mutex);
        if(m_count <=0)
            return false;
        int idx = (m_tail -1 + m_maxSize) % m_maxSize;
        out = m_buf[idx];
        return true;
    }

    int size()
    {
        QMutexLocker lock(&m_mutex);
        return m_count;
    }

    void clear()
    {
        QMutexLocker lock(&m_mutex);
        m_head =0;
        m_tail =0;
        m_count =0;
    }

private:
    QVector<T> m_buf;
    int m_maxSize;
    int m_head{0};
    int m_tail{0};
    int m_count{0};
    QMutex m_mutex;
    QWaitCondition m_cond;
};


class AcquireThread : public QThread
{
    Q_OBJECT
public:
    explicit AcquireThread(QObject *parent = nullptr);
    ~AcquireThread() override;

    void setHandle(HANDLE h);
    void setDevicePtr(ITCPDevice* dev);
    RingBuffer<SampleFrame>* getRingBuf();

protected:
    void run() override;

private:
    HANDLE m_hComm{ACSC_INVALID};
    ITCPDevice* m_device{nullptr};
    RingBuffer<SampleFrame> m_ringBuf;

    bool acquireOneFrame(SampleFrame& frame);
};


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    //void timerTask();

    void updateUI();

    void getinfo();

    void handleWaveDataUpdate();

    void updatexunhuanTime();

    void checkcsv();

    void on_tabWidget_currentChanged(int index);

    void on_system_min_clicked();

    void on_system_max_clicked();

    void on_system_exit_clicked();






    void on_checkBox_start_tem1_stateChanged(int arg1);

    void on_checkBox_start_tem2_stateChanged(int arg1);

    void on_checkBox_start_current1_stateChanged(int arg1);

    void on_checkBox_start_current2_stateChanged(int arg1);

    void on_comboBox_currentIndexChanged(int index);



    void on_start_stop_clicked();

    void on_test_set_clicked();

    void on_test_enable_clicked();

    void on_test_disable_clicked();

    void on_test_start_move_clicked();

    void on_test_stop2_clicked();

    void on_test_jog_pressed();

    void on_test_jog_released();

    void on_test_jog__pressed();

    void on_test_jog__released();

    void on_test_ptp_clicked();

    void on_test_ptpr_clicked();

    void on_test_clear_error_clicked();

    void on_test_stop_clicked();

    void on_btn_set_load_set_clicked();

    void on_btn_set_save_set_clicked();

    void on_btn_set_change_csv_path_clicked();

    void on_btn_set_change_image_path_clicked();

    void on_btn_set_open_csv_path_clicked();

    void on_btn_set_opene_image_path_clicked();

    void on_pushButton_clicked();


    //图表相关

    void onMousePress(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);


    void on_checkBox_start_tem_old1_stateChanged(int arg1);

    void on_checkBox_start_tem_old2_stateChanged(int arg1);

    void on_checkBox_start_current_old1_stateChanged(int arg1);

    void on_checkBox_start_current_old2_stateChanged(int arg1);

    void on_checkBox_test1_pos1_stateChanged(int arg1);

    void on_checkBox_test1_pos2_stateChanged(int arg1);

    void on_checkBox_test2_vel1_stateChanged(int arg1);

    void on_checkBox_test2_vel2_stateChanged(int arg1);

    void on_checkBox_test2_acc1_stateChanged(int arg1);

    void on_checkBox_test2_acc2_stateChanged(int arg1);

    void on_checkBox_test3_tem1_stateChanged(int arg1);

    void on_checkBox_test3_tem2_stateChanged(int arg1);

    void on_checkBox_test4_current1_stateChanged(int arg1);

    void on_checkBox_test4_current2_stateChanged(int arg1);

    void on_change_test_num_clicked();

    void on_save_image5_clicked();

    void on_save_image1_clicked();

    void on_save_image2_clicked();

    void on_save_image3_clicked();

    void on_save_image4_clicked();

    void on_dingwei5_clicked();

    void on_dingwei1_clicked();

    void on_dingwei2_clicked();

    void on_dingwei3_clicked();

    void on_dingwei4_clicked();

    void on_t_log_clicked();

    void on_set_protect_key_stateChanged(int arg1);

signals:
    void updateWaveData();

protected:
    //鼠标事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    //图表相关
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::Widget *ui;

    //创建工具类
    ttools tt;

    int i = 0;
    //初始化函数
    void log_init();    //日志初始化
    void system_init(); //系统设置
    void widget_init(); //控件设置
    void configureResponsiveLayout();
    void style_init();  //控件样式
    void chart_init();  //图表初始化
    void connect_init();//信号槽
    void connect_ACS();
    bool connect_NtcPtc();

    //鼠标事件相关参数
    bool m_isDragging = false;    // 是否正在拖动
    QPoint m_dragStartPosition;   // 拖动起始位置

    //图表相关
    bool mIsUserInteracting = false;
    QVector<double> mTimeData;
    QVector<double> mnumData;


    QCustomPlot *mPlot1;
    QCPGraph *mGraph1_1,*mGraph1_2;
    QVector<double> mData1_1,mData1_2;

    QCustomPlot *mPlot2;
    QCPGraph *mGraph2_1,*mGraph2_2,*mGraph2_3,*mGraph2_4;
    QVector<double> mData2_1,mData2_2,mData2_3,mData2_4;

    QCustomPlot *mPlot3;
    QCPGraph *mGraph3_1,*mGraph3_2;
    QVector<double> mData3_1,mData3_2;

    QCustomPlot *mPlot4;
    QCPGraph *mGraph4_1,*mGraph4_2;
    QVector<double> mData4_1,mData4_2;

    QCustomPlot *mPlot5;
    QCPGraph *mGraph5_1,*mGraph5_2,*mGraph5_3,*mGraph5_4,*mGraph5_5,*mGraph5_6,*mGraph5_7,*mGraph5_8;
    QVector<double> mData5_1,mData5_2,mData5_3,mData5_4,mData5_5,mData5_6,mData5_7,mData5_8;

    int movesign1=1,movesign2=1,movesign3=1,movesign4=1,movesign5=1;


    void refresh_olddata();
    QList<QList<QList<double>>> result;
    //全部定时器
    QTimer* getinfotimer1;//获取数据定时器
    //QThread* m_workThread;
    AcquireThread* m_acquireThread{nullptr}; //新采集线程

    //运动指令目标轴
    int final_axis=2;

    //设备句柄
    HANDLE hComm;

    //速度加速度减速度的设置系数
    //long double coefficient = sqrt(2.0L);

    //循环运动参数
    int cycle_num,cycle_pointA,cycle_pointB,cycle_waittime;
    QFuture<void>future;
    std::atomic<int> m_recycleMoveNum{0}; //往复运动次数
    QTimer *xunhuantimer;
    QTime xunhuanstarttime,xunhuanpausetime;
    int lastrecyclenum,allrecyclenum;//最后一次循环运动次数，累积总循环次数
    int startcyclenum;

    // 读写test  time/num
    QString getTestDataFolderPath();
    void writeTestTime(const QString &value);
    QString readTestTime();
    void writeTestNum(const QString &value);
    QString readTestNum();
    QTime parseSavedTime(const QString &timeStr);

    QString oldtime;

    //初始化xml参数
    void init_protcet_xml();


    //数据日志保存
    QTimer *checkcsv_timer;
    QString logname;
    int logsize;


    //记录程序启动时间
    QDateTime currentTime; //当前时间
    qint64 millisecondsSinceEpoch; //毫秒级时间戳


    //double getAInvalue(char* str);


    //温度查找字典
    std::map<double,double> lookupDict;


    std::atomic<int> issavelog;

    //电流温度最大值最小值
    double max_tem1=-0.111,max_tem2=-0.111,max_current1=-0.111,max_current2=-0.111;
    double final_max_tem1=-0.111,final_max_tem2=-0.111,final_max_current1=-0.111,final_max_current2=-0.111;

    void user_enableall();
    void user_disableall();


    int m_abnorTemperature,m_warnTemperature,m_abnorTemperature2,m_warnTemperature2;
    double m_abnorCurrent,m_warnCurrent,m_abnorCurrent2,m_warnCurrent2,m_shuiliuliang1,m_shuiliuliang2,m_atm1,m_atm2;
    QVector<double> m_TempAlarm1,m_TempAlarm2;
    QVector<double> m_CurrAlarm1,m_CurrAlarm2;
    QVector<double> m_shuiliul1,m_shuiliul2;



    //截图
    void saveFrameimage(int frame_num,QFrame* frame);

    //日志相关
    LogWindow *logWindow;
    bool  temlogsign1=true;bool  temlogsign2=true;bool  temlogsign3=true;bool  temlogsign4=true;
    bool  currentlogsign1=true;bool  currentlogsign2=true;bool  currentlogsign3=true;bool  currentlogsign4=true;
    bool  flowlogsign1=true;bool  flowlogsign2=true;bool  flowlogsign3=true;bool  flowlogsign4=true;
    bool  atmlogsign1=true;bool  atmlogsign2=true;bool  atmlogsign3=true;bool  atmlogsign4=true;
    void t_addlog(int level,const QString& message);

    //非按钮急停
    void stopstop();

    //新温度模块
    TemperatureModule *tempmodbus;


    ITCPDevice* device;
    DataAcquisitionService* acquisitionService_;
    bool startMotionRecording(double velocity,
                              double acceleration,
                              int waitTimeMilliseconds);
    void stopMotionRecording();
    void stopTestMotion();
    bool motionStopRequested_;
};


#endif // WIDGET_H



















