#ifndef MYSINGLECHART_H
#define MYSINGLECHART_H

#include <QWidget>

#include"mychartview.h"

class MySingleChart : public QWidget
{
    Q_OBJECT

public:
    //目前最多1个x轴
    MySingleChart(int Ynumber = 1, int seriesnumber = 1, QWidget *parent = nullptr);
    ~MySingleChart();
    //图例(曲线标识说明)的显示位置
    enum Direct{
        No,
        top,
        bottom,
        left,
        right
    };
    Q_ENUM(Direct)

    ///********************图表相关********************///
    /// 所有图表功能相关的函数
    /// 主要包括显示坐标和点,拖拽,放大缩小,图例和保存图片等
public slots:
    //是否跟随鼠标显示轴坐标,默认开启
    void isShowCursorAxisPos(bool isshow);
    //是否跟随鼠标显示最近的点属于的轴和坐标，默认关闭
    void isShowCursorNearPoint(bool isshow);
    //是否可以拖拽 默认可以
    void isDrag(bool isdrag);
    //设置缩放比例，默认1.5倍
    void setScaleFactor(double factor);
    //按比例放大缩小x轴y轴,从中心点
    void XAxisChangeScale(bool enlarge);
    void YAxisChangeScale(bool enlarge, int Yindex = 0);
    void allYAxisChangeScale(bool enlarge);
    //按比例移动x轴y轴
    void XAxisMoveScale(bool positive, double scale = 0.1);
    void YAxisMoveScale(bool positive, int Yindex = 0, double scale = 0.1);
    void allYAxisMoveScale(bool positive, double scale = 0.1);
    //按距离移动x轴y轴
    void XAxisMovePixel(double pixel = 100);
    void YAxisMovePixel(int Yindex = 0, double pixel = 100);
    void allYAxisMovePixel(double pixel = 100);
    //保存轴最大最小值等,以供右键恢复使用,最好每次改变调用一下
    void saveAxisRange();
    //按保存的最大最小值恢复
    void restoreAxisRange();

    //是否开启图例以及反向
    void setLegend(Direct direct = top);
    //设置是否可以看见数据点
    void setSeriesPointVisible(bool isvisibel);
    //自适应大小,自带保存当前最大最小值
    void setSeriesAppropriateView(int Ynumer, int seriesindex, double scale = 0.8);
    void setYAxisAppropriateView(int Ynumer, double scale = 0.8);
    void setAppropriateView(double scale = 0.8);
    //保存图片
    bool savePicture(bool finishsaveopen = false , bool openpicture = false);
    bool savePictureIn(QString filename);

    ///********************轴相关********************///
    /// 所有和轴相关的函数,包括添加移除,改变左右,改变名称
    /// 显示隐藏轴和刻度,改变颜色,单位,最大最小值,获取数量等

    //创建Y轴
    void addYAxis(bool alignleft = true);
    void addYAxisToNumber(int number, bool alignleft = true);
    void removeYAxis(int Yindex);
    void removeAllYAxis();
    //改变y轴的左右
    void setYAxisAlign(int Yindex, bool alignleft = true);
    //改变x轴的上下
    void setXAxisAlign(bool alignbottom = true);
    //设置x轴y轴名
    void setXAxisTitle(QString title = "时间：ms");
    void setYAxisTitle(int Yindex, QString title = "Count");
    //是否网格线可见以及刻度线数量
    void setAxisGridLineVisible(bool visible);
    void setXAxisTickCountNumber(int tickcount);
    void setYAxisTickCountNumber(int tickcount);
    //显示轴,隐藏轴，是否显示了轴
    bool isYAxisShow(int Yindex);
    void setYAxisShowHide(int Yindex, bool isshow);
    void setAllYAxisOneSeriesShowHideProperty(int seriesindex, bool isshow);
    void setAllYAxisShowHide(bool isshow);
    //改变轴线，网格线，标题和刻度颜色
    void setXAxisLineColor(QColor color);
    void setXAxisGridLineColor(QColor color);
    void setXAxisTilteColor(QColor color);
    void setXAxisLabelColor(QColor color);
    void setXAxisTitleLabelColor(QColor color);
    void setXAxisTitleLabelLineColor(QColor color);
    void setXAxisAllColor(QColor color);

    void setYAxisLineColor(int Yindex, QColor color);
    void setYAxisGridLineColor(int Yindex, QColor color);
    void setYAxisTilteColor(int Yindex, QColor color);
    void setYAxisLabelColor(int Yindex, QColor color);
    void setYAxisTitleLabelColor(int Yindex, QColor color);
    void setYAxisTitleLabelLineColor(int Yindex, QColor color);
    void setYAxisAllColor(int Yindex, QColor color);

    void setAllYColor(QColor color);
    void setAllAxisColor(QColor color);

    void resetXAxisColor();
    void resetYAxisColor(int Yindex);
    void resetAllYAxisColor();
    void resetAllAxisColor();
    //设置x轴y轴标题的大小,防止m以及²,³看不清
    void setXAxisTitleSize(int size);
    void setYAxisTitleSize(int Yindex, int size);
    void setAllYAxisTitleSize(int size);
    //设置轴显示整数还是浮点数
    void setXAxisInteger();
    void setYAxisInteger(int Yindex);
    void setAllYAxisInteger();
    void setXAxisFloat(int flo = 2);
    void setYAxisFloat(int Yindex, int flo = 2);
    void setAllYAxisFloat(int flo = 2);
    //设置x轴y轴最大最小值
    void setXAxisMin(double min);
    void setXAxisMax(double max);
    void setXAxisMinMAx(double min, double max);
    void moveXAxisDistance(double dis);
    void setYAxisMin(int Yindex, double min);
    void setYAxisMax(int Yindex, double max);
    void setYAxisMinMAx(int Yindex, double min, double max);
    void setAllYAxisMinMAx(double min, double max);
    void moveYAxisDistance(int Yindex, double dis);
    void moveAllYAxisDistance(double dis);
    //获取y轴数,判断是不是轴数
    int getYAxisNum();
    bool isYAxisExist(int Yindex);
    bool isYAxisHaveSeries(int Yindex);

    ///********************曲线相关********************///
    /// 所有和曲线相关的函数,包括创建和移除曲线,设置名称,显示隐藏
    /// 获取数量,改变颜色等

    //添加移除曲线
    void addYAxisSeries(int Yindex);
    void addYAxisSeriesToNumber(int Yindex, int seriesnum);
    void removeYAxisSeries(int Yindex, int seriesindex);
    void removeAxisYAllSeries(int Yindex);
    //设置曲线名
    void setSeriesName(int Yindex, int seriesindex, QString name="");
    //显示曲线,隐藏曲线，是否显示了曲线
    bool getSeriesShowProperty(int Yindex, int series);
    bool isSeriesShow(int Yindex, int series);
    void setSeriesShowHideProperty(int Yindex, int series, bool isshow);
    void setYAxisAllSeriesShowHideProperty(int Yindex, bool isshow);
    //获取每个y轴曲线数
    int  getYAxisSeriesNum(int Yindex);
    //判断当前曲线数是否在y轴
    bool isYAxisSeriesExist(int Yindex, int seriesindex);
    //改变第几个曲线的颜色
    void setSeriesColor(int Yindex, int seriesindex,  QColor color);
    //改变第几个轴和曲线的颜色
    void setAxisAllSeriesColor(int Yindex,  QColor color);
    void setAxisAndAllSeriesColor(int Yindex,  QColor color);

    ///********************数据相关********************///
    /// 所有和数据相关的函数,包括设置存储,添加或者替换数据
    /// 清空数据,获取数据,计算最大最小值并且获取等

    //刷新一遍全部数据
    void refreshAllData();
    //设置每个曲线最大储存数据量，为0时是无限，不过会对性能有影响
    void setMaxDataCount(int count);
    //如果设置了最大数据存储量,每够一定数量会发送信号,把数据释放,不够的时候还会有剩余,这个使用最后剩余的数量
    QList<QPointF> getRemainData(int Yindex, int seriesindex);
    //获取当前每个曲线最大数据量
    int getMaxDataCount();
    //添加数据，替换数据，清空某个曲线数据,清空全部曲线数据
    void addSeriesYData(int Yindex, int seriesindex, double y);
    void addSeriesData(int Yindex, int seriesindex, double x, double y);
    void replaceSeriesData(int Yindex, int seriesindex ,const QList<QPointF> &data);
    //按点数移动，适合x按+1添加的情况
    void addSeriesYDataAndMoveWithCount(int Yindex, int seriesindex, double y);
    void addSeriesDataAndMoveWithCount(int Yindex, int seriesindex, double x, double y);
    void replaceSeriesDataAndMoveWithCount(int Yindex, int seriesindex, const QList<QPointF> &data);
    //按x最大值移动，适合随机添加的情况
    void addSeriesDataAndMoveWithMax(int Yindex, int seriesindex, double x, double y);
    void replaceSeriesDataAndMoveWithMax(int Yindex, int seriesindex, const QList<QPointF> &data);
    //清空数据
    void clearYAxisData(int Yindex);
    void clearAllYAxisData();
    void clearSeriesData(int Yindex, int seriesindex);
    void clearAllSeriesData();
    //获取数据量和数据
    int getSeriesDataNum(int Yindex, int seriesindex);
    bool isDataNumExist(int Yindex, int seriesindex, int index);
    double getDataX(int Yindex, int seriesindex, int index);
    double getDataY(int Yindex, int seriesindex, int index);
    QPointF getData(int Yindex, int seriesindex, int index);
    QList<QPointF> getData(int Yindex, int seriesindex);
    //是否计算y的最大最小值，主要是为了显示数据使用，会对性能有影响，默认不使用
    void setIsCalculateMaxMinY(bool ismaxminy);
    //是否计算x最大最小值，主要是为了moveWithMax使用,使用moveWithMax自动开启，如果add则从使用时开始计算
    void setIsCalculateMaxMinX(bool ismaxminx);
    //设置是否使用时钟替换数据,以及频率,默认100ms,连续add太耗性能,可以按频率是刷新显示,该记录还是记录
    void setUseTimer(bool use);
    void setTimerFrequency(int fre);
    //获取曲线Y值最大最小值，如果没有开启，则会手动计算，如果使用add，从开启时计算,建议add
    double getSeriesMaxValueY(int Yindex, int seriesindex);
    double getSeriesMaxValueX(int Yindex, int seriesindex);
    double getSeriesMinValueY(int Yindex, int seriesindex);
    double getSeriesMinValueX(int Yindex, int seriesindex);
    int getSeriesMaxValueIndex(int Yindex, int seriesindex);
    int getSeriesMinValueIndex(int Yindex, int seriesindex);
    //获取单轴Y值最大最小值
    bool getYAxisMaxValueSeries(int Yindex, int &seriesindex, int &index);
    bool getYAxisMinValueSeries(int Yindex, int &seriesindex, int &index);
    double getYAxisMaxValueX(int Yindex);
    double getYAxisMinValueX(int Yindex);
    double getYAxisMaxValueY(int Yindex);
    double getYAxisMinValueY(int Yindex);
    //获取全部轴Y值最大最小值
    bool getAllYAxisMaxValueYAxis(int &Yindex, int &seriesindex, int &index);
    bool getAllYAxisMinValueYAxis(int &Yindex, int &seriesindex, int &index);
    double getAllYAxisMaxValueX();
    double getAllYAxisMinValueX();
    double getAllYAxisMaxValueY();
    double getAllYAxisMinValueY();
    //获取x值最大值最小值
    double getSeriesMaxX(int Yindex, int seriesindex);
    double getSeriesMinX(int Yindex, int seriesindex);
    double getSeriesMaxXIndex(int Yindex, int seriesindex);
    double getSeriesMinXIndex(int Yindex, int seriesindex);
    double getXAxisMaxValueInYAxis(int Yindex);
    double getXAxisMinValueInYAxis(int Yindex);
    double getXAxisMaxValue();
    double getXAxisMinValue();

    ///********************重置相关********************///
    ///主要是重置整个图表和测试添加一些数据

    //重置，恢复默认
    void reset(int Yindex = 1, int serienumber = 1);
    //测试,在几个轴几个曲线上添加范围是多少的多少个数据点
    void test(int axisnum = 1, int seriesnum = 1, int datanum = 100, int miny = 0, int maxy = 1000);

signals:
    //如果限制数量，则每次满数量，发送信号，可以用于存储
    void fullData(QList<QPointF> data);

private:
    //x和y轴
    QValueAxis *axisX;
    QList<QValueAxis*> axisY;
    //曲线列表
    QList<QList<QLineSeries *>> myLineSeries;
    QList<QList<QScatterSeries*>> myScatterSeries;
    bool isShowScatter = false;
    //显示隐藏属性
    QList<QList<bool>> mySeriesShowProPerty;
    //实际数据
    QList<QList<QList<QPointF>>> myDataPoint;
    //每个曲线最大储存的数据量如果是0则无限，不过可能会对性能有影响
    int defaultDataCount = 30000;
    int maxDataCount = 30000;
    //当前记录数据量，为了发送信号和保存等
    QList<QList<int>> currentDataCount;
    //是否计算y的最大最小值，主要是为了显示数据使用
    bool isMaxMinY = false;
    //是否计算x最大最小值，主要是为了moveWithMax使用
    bool isMaxMinX = false;

    QList<QList<int>> myMaxDataIndex;
    QList<QList<int>> myMinDataIndex;
    QList<QList<int>> myMaxXIndex;
    QList<QList<int>> myMinXIndex;

    //默认颜色
    QPen xPen;
    QBrush xbrush;
    //默认初始最大最小值
    int defaultMin = 0;
    int defaultMax = 1000;
    int defaultGrideLineNum = 11;
    int defaultAxisTitleFontSize = 16;
    //画图的charts
    QChart *myChart;
    //视图用来展示charts，附带放大缩小等功能
    MyChartView* m_ChartView;

    //创建X轴
    QValueAxis* creatAxis(QString name = "时间：ms");
    void addXAxis();
    void removeXAxis();
    //移动轴
    void moveWithCount(int Yindex, int seriesindex);
    void moveWithMax(int Yindex, int seriesindex);
    //计算是否移除数据
    bool ifRemoveData(int Yindex, int seriesindex, bool isreplace);
    //替换数据
    void repleaseData(int Yindex, int seriesindex);

    //隐藏显示曲线
    void setSeriesShowHide(int Yindex, int series, bool isshow);
    //获取最大最小值点
    bool isMaxMinPointExist(int Yindex, int seriesindex);
    bool isMaxMinXExist(int Yindex, int seriesindex);
    QPointF getMaxValuePoint(int Yindex, int seriesindex);
    QPointF getMinValuePoint(int Yindex, int seriesindex);
    QPointF getMaxXPoint(int Yindex, int seriesindex);
    QPointF getMinXPoint(int Yindex, int seriesindex);
    //计算最大最小值,如果没有开启计算功能,则用这种方式获取,建议使用add方式并且开启功能
    double calculateSeriesMaxValueY(int Yindex, int seriesindex);
    double calculateSeriesMaxValueX(int Yindex, int seriesindex);
    double calculateSeriesMinValueY(int Yindex, int seriesindex);
    double calculateSeriesMinValueX(int Yindex, int seriesindex);
    int calculateSeriesMaxValueIndex(int Yindex, int seriesindex);
    int calculateSeriesMinValueIndex(int Yindex, int seriesindex);
    double calculateSeriesMaxX(int Yindex, int seriesindex);
    double calculateSeriesMinX(int Yindex, int seriesindex);
    double calculateSeriesMaxXIndex(int Yindex, int seriesindex);
    double calculateSeriesMinXIndex(int Yindex, int seriesindex);
    //重置最大最小索引
    void resetMaxMinIndex(int Yindex, int seriesindex);
    //比较最大最小值
    void compareMaxMinValue(int Yindex, int seriesindex, int index, double y);
    void compareMaxMinX(int Yindex, int seriesindex, int index, double x);
    //设置比例
    void setXAxisScale(double maxX, double minX, double scale = 0.8);
    void setYAxisScale(int Yindex, double maxY, double minY, double scale = 0.8);
    //隐藏点的图例
    void hidePointLegend();
    //无效值
    int noNum = 0;
    int noIndex = -1;
    double infiniteMax = qInf();
    double infiniteMin = -qInf();

    //时钟频率以及是否使用时钟,主要是为了防止卡顿
    int defaultTimerFrequenct = 100;
    bool defaultUseTimer = true;
    bool isUseTimer = false;
    QTimer* freshTiemr;
};

#endif // MYSINGLECHART_H
