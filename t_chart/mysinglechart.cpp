#include "mysinglechart.h"

// #include <QOpenGLWidget>
// #include <QOpenGLFunctions>

MySingleChart::MySingleChart(int Yindex, int seriesindex, QWidget *parent  )
    : QWidget (parent)
{
    myChart = new QChart();
    //myChart->setTheme(QChart::ChartThemeLight);
    //myChart->setTheme(QChart::ChartThemeDark);
    myChart->setBackgroundVisible(false);
    // 禁用动画
    myChart->setAnimationOptions(QChart::NoAnimation);
    // 绑定Chart到ChartView
    m_ChartView = new MyChartView(myChart,this);
    //反锯齿
    m_ChartView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);             // 设置消除锯齿，曲线平滑

    //保护
    if(Yindex <= noNum)
        Yindex = 1;
    if(seriesindex < noNum)
        seriesindex = noNum;
    //初始化坐标轴 和曲线
    addXAxis();
    addYAxisToNumber(Yindex);
    for (int i = 0; i < Yindex; ++i) {
        addYAxisSeriesToNumber(i, seriesindex);
    }
    setXAxisTitleSize(defaultAxisTitleFontSize);
    setAllYAxisTitleSize(defaultAxisTitleFontSize);
    // 设置使用OpenGL加速  会让抗锯齿无效果
    //    m_Series.at(0)->setUseOpenGL(true);
    //    glEnable(GL_MULTISAMPLE);

    //保存一下
    saveAxisRange();
    //创建刷新时钟
    freshTiemr = new QTimer(this);
    freshTiemr->setInterval(defaultTimerFrequenct);
    freshTiemr->callOnTimeout(this, &MySingleChart::refreshAllData);
    setUseTimer(defaultUseTimer);
    myChart->layout()->setContentsMargins(0, 0, 0, 0);
    myChart->setBackgroundRoundness(0);//设置背景区域无圆角!!!
    //设置布局
    QHBoxLayout *pHLayout = new QHBoxLayout(this);
    pHLayout->setContentsMargins(0, 0, 0, 0);
    pHLayout->addWidget(m_ChartView);
    setLayout(pHLayout);
    //设置透明
    //setAutoFillBackground(false);
    //setAttribute(Qt::WA_TranslucentBackground);
    //m_ChartView->setStyleSheet("background: transparent");
    //m_Chart->setBackgroundVisible(false);
    xPen = axisX->gridLinePen();
    xbrush = axisX->titleBrush();
}

MySingleChart::~MySingleChart()
{
    removeAllYAxis();
}

void MySingleChart::isShowCursorAxisPos(bool isshow)
{
    m_ChartView->isShowCursorAxis = isshow;
}

void MySingleChart::isShowCursorNearPoint(bool isshow)
{
    m_ChartView->isShowCursorPoint = isshow;
}

void MySingleChart::isDrag(bool isdrag)
{
    m_ChartView->isDrag = isdrag;
}

void MySingleChart::setScaleFactor(double factor)
{
    m_ChartView->changeScale(factor);
}

void MySingleChart::XAxisChangeScale(bool enlarge)
{
    m_ChartView->XAxisChange(enlarge);
}

void MySingleChart::YAxisChangeScale(bool enlarge, int Yindex)
{
    m_ChartView->YAxisChange(Yindex, enlarge);
}

void MySingleChart::allYAxisChangeScale(bool enlarge)
{
    m_ChartView->allYAxisChange(enlarge);
}

void MySingleChart::XAxisMoveScale(bool positive, double scale)
{
    m_ChartView->XAxisMoveScale(positive, scale);
}

void MySingleChart::YAxisMoveScale(bool positive, int Yindex, double scale)
{
    m_ChartView->YAxisMoveScale(Yindex, positive, scale);
}

void MySingleChart::allYAxisMoveScale(bool positive, double scale)
{
    m_ChartView->allYAxisMoveScale(positive, scale);
}

void MySingleChart::XAxisMovePixel(double pixel)
{
    m_ChartView->XAxisMoveDis(pixel);
}

void MySingleChart::YAxisMovePixel(int Yindex, double pixel)
{
    m_ChartView->YAxisMoveDis(Yindex, pixel);
}

void MySingleChart::allYAxisMovePixel(double pixel)
{
    m_ChartView->allYAxisMoveDis(pixel);
}

void MySingleChart::saveAxisRange()
{
    m_ChartView->saveAxisRange();
}

void MySingleChart::restoreAxisRange()
{
    m_ChartView->restoreAxisRange();
}

void MySingleChart::setYAxisShowHide(int Yindex, bool isshow)
{
    //如果存在y轴
    if(isYAxisExist(Yindex))
    {
        axisY.at(Yindex)->setVisible(isshow);
        int num = getYAxisSeriesNum(Yindex);
        for (int i = 0; i < num; ++i) {
            setSeriesShowHide(Yindex, i, isshow);
        }
    }
}

void MySingleChart::setAllYAxisOneSeriesShowHideProperty(int seriesindex, bool isshow)
{
    int num = getYAxisNum();
    for (int i = 0; i < num; ++i) {
        setSeriesShowHideProperty(i, seriesindex, isshow);
    }
}

void MySingleChart::setAllYAxisShowHide(bool isshow)
{
    int num = axisY.count();
    for (int i = 0; i < num; ++i) {
        setYAxisShowHide(i, isshow);
    }
}

void MySingleChart::reset(int Yindex, int serienumber)
{
    setUseTimer(defaultUseTimer);
    setTimerFrequency(defaultTimerFrequenct);
    setMaxDataCount(defaultDataCount);
    setIsCalculateMaxMinY(false);
    setIsCalculateMaxMinX(false);
    removeAllYAxis();
    removeXAxis();
    //初始化坐标轴 和曲线
    addXAxis();
    addYAxisToNumber(Yindex);
    for (int i = 0; i < Yindex; ++i) {
        addYAxisSeriesToNumber(i, serienumber);
    }
    setXAxisTitleSize(defaultAxisTitleFontSize);
    setAllYAxisTitleSize(defaultAxisTitleFontSize);
    setSeriesPointVisible(false);
    m_ChartView->saveAxisRange();
}

void MySingleChart::setXAxisTitle(QString title)
{
    axisX->setTitleText(title);
}

void MySingleChart::setYAxisTitle(int Yindex, QString title)
{
    axisY.at(Yindex)->setTitleText(title);
}


void MySingleChart::setXAxisMinMAx(double min, double max)
{
    axisX->setRange(min, max);
    m_ChartView->saveAxisRange();
}

void MySingleChart::moveXAxisDistance(double dis)
{
    setXAxisMinMAx(axisX->min() + dis, axisX->max() + dis);
}

void MySingleChart::setYAxisMin(int Yindex, double min)
{
    if(isYAxisExist(Yindex))
    {
        axisY.at(Yindex)->setMin(min);
        m_ChartView->saveAxisRange();
    }
}

void MySingleChart::setYAxisMax(int Yindex, double max)
{
    if(isYAxisExist(Yindex))
    {
        axisY.at(Yindex)->setMax(max);
        m_ChartView->saveAxisRange();
    }
}

void MySingleChart::setYAxisMinMAx(int Yindex, double min, double max)
{
    if(isYAxisExist(Yindex))
    {
        axisY.at(Yindex)->setRange(min, max);
        m_ChartView->saveAxisRange();
    }
}

void MySingleChart::setAllYAxisMinMAx(double min, double max)
{
    int num = getYAxisNum();
    for (int i = 0; i < num; ++i) {
        setYAxisMinMAx(i, min, max);
    }
}

void MySingleChart::moveYAxisDistance(int Yindex, double dis)
{
    if(isYAxisExist(Yindex))
        setYAxisMinMAx(Yindex, axisY.at(Yindex)->min() + dis, axisY.at(Yindex)->max() + dis);
}

void MySingleChart::moveAllYAxisDistance(double dis)
{
    int num = getYAxisNum();
    for (int i = 0; i < num; ++i) {
        moveYAxisDistance(i, dis);
    }
}

int MySingleChart::getYAxisNum()
{
    return axisY.count();
}

bool MySingleChart::isYAxisExist(int Yindex)
{
    if(Yindex >= 0 && Yindex <= getYAxisNum())
        return true;
    else
        return false;
}

bool MySingleChart::isYAxisHaveSeries(int Yindex)
{
    if(isYAxisExist(Yindex))
    {
        int num = myLineSeries.at(Yindex).count();
        if(num > 0)
            return true;
    }
    return false;
}

int MySingleChart::getYAxisSeriesNum(int Yindex)
{
    if(isYAxisExist(Yindex))
        return myLineSeries.at(Yindex).count();
    return noNum;
}

bool MySingleChart::isYAxisSeriesExist(int Yindex, int seriesindex)
{
    if(isYAxisExist(Yindex))
        if(seriesindex >= 0 && seriesindex < myLineSeries.at(Yindex).count())
            return true;
    return false;
}

void MySingleChart::addYAxisSeries(int Yindex)
{
    if(isYAxisExist(Yindex))
    {
        //添加曲线(必须先添加曲线再绑定轴!)
        QLineSeries* mSeries = new QLineSeries();
        myChart->addSeries(mSeries);
        //绑定轴
        mSeries->attachAxis(axisX);
        mSeries->attachAxis(axisY.at(Yindex));
        mSeries->setName("轴"+QString::number(Yindex) + "曲线" + QString::number(getYAxisSeriesNum(Yindex)));
        //建立保存数据
        myLineSeries[Yindex].append(mSeries);

        //添加点
        QScatterSeries *scatterSeries = new QScatterSeries();
        // 设置散点系列的样式
        scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle); // 设置为圆形
        scatterSeries->setBorderColor(Qt::blue); // 边框颜色
        scatterSeries->setBrush(Qt::white); // 填充颜色，设置为白色可实现空心效果
        scatterSeries->setMarkerSize(10); // 点的大小
        myChart->addSeries(scatterSeries);
        //绑定轴
        scatterSeries->attachAxis(axisX);
        scatterSeries->attachAxis(axisY.at(Yindex));
        //是否可见
        scatterSeries->setVisible(isShowScatter);
        // 遍历图例中的标记，隐藏 QScatterSeries 的图例项
        hidePointLegend();
        //建立保存数据
        myScatterSeries[Yindex].append(scatterSeries);
        mySeriesShowProPerty[Yindex].append(true);
        currentDataCount[Yindex].append(noNum);
        //添加数据
        QList<QPointF> data;
        myDataPoint[Yindex].append(data);

        myMaxDataIndex[Yindex].append(noIndex);
        myMinDataIndex[Yindex].append(noIndex);
        myMaxXIndex[Yindex].append(noIndex);
        myMinXIndex[Yindex].append(noIndex);

    }
}

void MySingleChart::addYAxisSeriesToNumber(int Yindex, int seriesnum)
{
    int num = seriesnum - getYAxisSeriesNum(Yindex);
    for (int i = 0; i < num; ++i) {
        addYAxisSeries(Yindex);
    }
}

void MySingleChart::removeYAxisSeries(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //移除曲线
        //mySeries.at(Yindex).at(seriesindex)->detachAxis(axisX);
        //mySeries.at(Yindex).at(seriesindex)->detachAxis(axisY.at(Yindex));
        myChart->removeSeries(myLineSeries.at(Yindex).at(seriesindex));
        delete myLineSeries.at(Yindex).at(seriesindex);
        myLineSeries[Yindex][seriesindex] = nullptr;
        myLineSeries[Yindex].removeAt(seriesindex);

        myChart->removeSeries(myScatterSeries.at(Yindex).at(seriesindex));
        delete myScatterSeries.at(Yindex).at(seriesindex);
        myScatterSeries[Yindex][seriesindex] = nullptr;
        myScatterSeries[Yindex].removeAt(seriesindex);
        mySeriesShowProPerty[Yindex].removeAt(seriesindex);
        currentDataCount[Yindex].removeAt(seriesindex);
        myDataPoint[Yindex].removeAt(seriesindex);
        myMaxDataIndex[Yindex].removeAt(seriesindex);
        myMinDataIndex[Yindex].removeAt(seriesindex);
        myMaxXIndex[Yindex].removeAt(seriesindex);
        myMinXIndex[Yindex].removeAt(seriesindex);
    }
}

void MySingleChart::removeAxisYAllSeries(int Yindex)
{
    int num = getYAxisSeriesNum(Yindex);
    for (int i = 0; i < num; ++i) {
        removeYAxisSeries(Yindex, 0);
    }
}

void MySingleChart::setSeriesPointVisible(bool isvisibel)
{
    isShowScatter = isvisibel;
    int num = axisY.count();
    for (int i = 0; i < num; ++i) {
        int snum = myLineSeries.at(i).count();
        for (int j = 0; j < snum; ++j) {
            setSeriesShowHideProperty(i, j, getSeriesShowProperty(i, j));
        }
    }
}

bool MySingleChart::isYAxisShow(int Yindex)
{
    if(isYAxisExist(Yindex))
        return axisY.at(Yindex)->isVisible();
    return false;
}

void MySingleChart::moveWithCount(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        int x = getData(Yindex, seriesindex).last().x();
        if(x > axisX->max())
        {
            moveXAxisDistance(x - axisX->max());
            // qreal dis = myChart->plotArea().width() / (axisX->max() - axisX->min());
            // myChart->scroll((x - axisX->max())*dis, 0);
        }
       // qApp->processEvents();
    }
}

void MySingleChart::moveWithMax(int Yindex, int seriesindex)
{
    if (isYAxisSeriesExist(Yindex, seriesindex)) // 检查Y轴系列是否存在
    {
        // 获取当前系列的数据
        QList<QPointF> dataPoints = myDataPoint.at(Yindex).at(seriesindex);
        // 获取当前系列的最大x值
        double maxDataX =getSeriesMaxX(Yindex, seriesindex);

        // 如果最大x值超过x轴的最大值
        if (maxDataX > axisX->max())
        {
            // 计算需要滚动的x值差
            // 计算数据单位的偏移量
            moveXAxisDistance(maxDataX - axisX->max());
            //// 计算像素单位的偏移量
            // qreal x = myChart->plotArea().width() / (axisX->max() - axisX->min()); // 每个数据单位对应的像素宽度
            // qreal scrollXPixels = scrollXDataUnits * x; // 将数据单位的偏移量转换为像素单位
            //// 将图表滚动到显示最大x值的位置
            //myChart->scroll(scrollXPixels, 0);
        }
       // qApp->processEvents();
    }
}

bool MySingleChart::ifRemoveData(int Yindex, int seriesindex, bool isreplace)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //如果要删除数据
        if(maxDataCount > 0)
        {
            int num = getSeriesDataNum(Yindex, seriesindex);
            if(!isreplace)
            {
                //add每次加1
                currentDataCount[Yindex][seriesindex]++;
                //如果够数量就发送一次并且清空计数
                if(currentDataCount.at(Yindex).at(seriesindex) == maxDataCount)
                {
                    emit fullData(getData(Yindex, seriesindex));
                    currentDataCount[Yindex][seriesindex] = noNum;
                }
                //如果总数量超标
                if(num > maxDataCount)
                {
                    //因为是add,每次只会多一个,删除第一个
                    myDataPoint[Yindex][seriesindex] = myDataPoint[Yindex][seriesindex].mid(1);
                    return true;
                }
            }
            else
            {
                //如果数量超标
                if(num > maxDataCount)
                {
                    //如果移除的数量大于了保存数量,分段发送并且删除前面n个
                    do{
                        num -= maxDataCount;
                        emit fullData(myDataPoint[Yindex][seriesindex].mid(0,maxDataCount));
                        myDataPoint[Yindex][seriesindex] = myDataPoint[Yindex][seriesindex].mid(maxDataCount);
                    }
                    while(num > maxDataCount);
                    //删除前deletecount个
                    myDataPoint[Yindex][seriesindex] = myDataPoint[Yindex][seriesindex].mid(num);
                    return true;
                }
                //replease情况下最后剩下的计数当时数量(如果不超过最大)
                currentDataCount[Yindex][seriesindex] = num;
            }
        }
        else
        {
            //重置计数
            currentDataCount[Yindex][seriesindex] = 0;
        }
        return false;
    }
    return false;
}

void MySingleChart::repleaseData(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        myLineSeries.at(Yindex).at(seriesindex)->replace(myDataPoint.at(Yindex).at(seriesindex));
        myScatterSeries.at(Yindex).at(seriesindex)->replace(myDataPoint.at(Yindex).at(seriesindex));
        //如果不添加，在大量刷新数据时会卡顿！！因为添加数据耗时太长队列塞满了没空处理界面刷新
        //qApp->processEvents();
        moveWithCount(Yindex, seriesindex);
    }
}

bool MySingleChart::isMaxMinPointExist(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
        return myMaxDataIndex.at(Yindex).at(seriesindex) >= 0;
    else
        return false;
}

bool MySingleChart::isMaxMinXExist(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
        return myMaxXIndex.at(Yindex).at(seriesindex) >= 0;
    else
        return false;
}


QPointF MySingleChart::getMaxValuePoint(int Yindex, int seriesindex)
{
    if(isMaxMinPointExist(Yindex, seriesindex))
        return myDataPoint.at(Yindex).at(seriesindex).at(myMaxDataIndex.at(Yindex).at(seriesindex));
    else
        return QPointF(infiniteMin, infiniteMin);
}

QPointF MySingleChart::getMinValuePoint(int Yindex, int seriesindex)
{
    if(isMaxMinPointExist(Yindex, seriesindex))
        return myDataPoint.at(Yindex).at(seriesindex).at(myMinDataIndex.at(Yindex).at(seriesindex));
    else
        return QPointF(infiniteMax, infiniteMax);
}

QPointF MySingleChart::getMaxXPoint(int Yindex, int seriesindex)
{
    if(isMaxMinXExist(Yindex, seriesindex))
    {
        return myDataPoint.at(Yindex).at(seriesindex).at(myMaxXIndex.at(Yindex).at(seriesindex));
    }
    else
        return QPointF(infiniteMin, infiniteMin);
}

QPointF MySingleChart::getMinXPoint(int Yindex, int seriesindex)
{
    if(isMaxMinXExist(Yindex, seriesindex))
        return myDataPoint.at(Yindex).at(seriesindex).at(myMinXIndex.at(Yindex).at(seriesindex));
    else
        return QPointF(infiniteMax, infiniteMax);
}

double MySingleChart::calculateSeriesMaxValueY(int Yindex, int seriesindex)
{
    double max = infiniteMin;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        for (int i = 0; i < num; ++i) {
            double data = getDataY(Yindex, seriesindex, i);
            if(data > max)
                max = data;
        }
    }
    return max;
}

double MySingleChart::calculateSeriesMaxValueX(int Yindex, int seriesindex)
{
    int x = infiniteMin;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        double max = infiniteMin;
        for (int i = 0; i < num; ++i) {
            double data = getDataY(Yindex, seriesindex, i);
            if(data > max)
            {
                max = data;
                x = getDataX(Yindex, seriesindex, i);
            }
        }
    }
    return x;
}

double MySingleChart::calculateSeriesMinValueY(int Yindex, int seriesindex)
{
    double min = infiniteMax;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        for (int i = 0; i < num; ++i) {
            double data = getDataY(Yindex, seriesindex, i);
            if(data < min)
                min = data;
        }
    }
    return min;
}

double MySingleChart::calculateSeriesMinValueX(int Yindex, int seriesindex)
{
    int x = infiniteMax;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        double min = infiniteMax;
        for (int i = 0; i < num; ++i) {
            double data = getDataY(Yindex, seriesindex, i);
            if(data < min)
            {
                min = data;
                x = getDataX(Yindex, seriesindex, i);
            }
        }
    }
    return x;
}

int MySingleChart::calculateSeriesMaxValueIndex(int Yindex, int seriesindex)
{
    int index = noIndex;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        double max = infiniteMin;
        for (int i = 0; i < num; ++i) {
            double data = getDataY(Yindex, seriesindex, i);
            if(data > max)
            {
                max = data;
                index = i;
            }
        }
    }
    return index;
}

int MySingleChart::calculateSeriesMinValueIndex(int Yindex, int seriesindex)
{
    int index = noIndex;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        double min = infiniteMax;
        for (int i = 0; i < num; ++i) {
            double data = getDataY(Yindex, seriesindex, i);
            if(data < min)
            {
                min = data;
                index = i;
            }
        }
    }
    return index;
}

double MySingleChart::calculateSeriesMaxX(int Yindex, int seriesindex)
{
    double max = infiniteMin;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        for (int i = 0; i < num; ++i) {
            double data = getDataX(Yindex, seriesindex, i);
            if(data > max)
                max = data;
        }
    }
    return max;
}

double MySingleChart::calculateSeriesMinX(int Yindex, int seriesindex)
{
    double min = infiniteMax;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        for (int i = 0; i < num; ++i) {
            double data = getDataX(Yindex, seriesindex, i);
            if(data < min)
                min = data;
        }
    }
    return min;
}

double MySingleChart::calculateSeriesMaxXIndex(int Yindex, int seriesindex)
{
    int index = noIndex;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        double max = infiniteMin;
        for (int i = 0; i < num; ++i) {
            double data = getDataX(Yindex, seriesindex, i);
            if(data > max)
            {
                max = data;
                index = i;
            }
        }
    }
    return index;
}

double MySingleChart::calculateSeriesMinXIndex(int Yindex, int seriesindex)
{
    int index = noIndex;
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        //计算最大
        int num = getSeriesDataNum(Yindex, seriesindex);
        double min = infiniteMax;
        for (int i = 0; i < num; ++i) {
            double data = getDataX(Yindex, seriesindex, i);
            if(data < min)
            {
                min = data;
                index = i;
            }
        }
    }
    return index;
}

void MySingleChart::resetMaxMinIndex(int Yindex, int seriesindex)
{
    myMaxDataIndex[Yindex][seriesindex] = noIndex;
    myMinDataIndex[Yindex][seriesindex] = noIndex;
    myMaxXIndex[Yindex][seriesindex] = noIndex;
    myMinXIndex[Yindex][seriesindex] = noIndex;
}

void MySingleChart::compareMaxMinValue(int Yindex, int seriesindex, int index, double y)
{
    double max = getSeriesMaxValueY(Yindex, seriesindex);
    double min = getSeriesMinValueY(Yindex, seriesindex);
    //不能只比较一次，否则第一个比较，如果只有一个数据，那么就会只给最大值赋值
    if(y > max)
        myMaxDataIndex[Yindex][seriesindex] = index;
    if(y < min)
        myMinDataIndex[Yindex][seriesindex] = index;
}

void MySingleChart::compareMaxMinX(int Yindex, int seriesindex, int index, double x)
{
    //如果存在,比较
    double max = getSeriesMaxX(Yindex, seriesindex);
    double min = getSeriesMinX(Yindex, seriesindex);
    if(x > max)
        myMaxXIndex[Yindex][seriesindex] = index;
    if(x < min)
        myMinXIndex[Yindex][seriesindex] = index;

}

void MySingleChart::setXAxisScale(double maxX, double minX, double scale)
{
    // 检查scale是否有效（0< scale <= 1）
    if (scale > 0 && scale <= 1.0)
    {
        // 计算x的范围
        double x_range = maxX - minX;
        // 计算扩展部分（两侧各占(1 - scale) / 2）
        double extension = x_range * (1.0 - scale) / 2.0;
        // 计算整个范围的最大值和最小值
        double scall_x_max = maxX + extension;
        double scall_x_min = minX - extension;
        setXAxisMinMAx(scall_x_min, scall_x_max);
    }
}

void MySingleChart::setYAxisScale(int Yindex, double maxY, double minY, double scale)
{
    if (scale > 0 && scale <= 1.0)
    {
        if(isYAxisShow(Yindex))
        {
            // 计算x的范围
            double y_range = maxY - minY;
            // 计算扩展部分（两侧各占(1 - scale) / 2）
            double extension = y_range * (1.0 - scale) / 2.0;
            // 计算整个范围的最大值和最小值
            double scall_y_max = maxY + extension;
            double scall_y_min = minY - extension;
            setYAxisMinMAx(Yindex, scall_y_min, scall_y_max);
        }
    }
}

void MySingleChart::hidePointLegend()
{
    foreach (QLegendMarker *marker, myChart->legend()->markers()) {
        if (marker->series()->type() == QScatterSeries::SeriesTypeScatter) {
            marker->setVisible(false); // 隐藏 QScatterSeries 的图例项
        }
    }
}

void MySingleChart::addSeriesYData(int Yindex, int seriesindex, double y)
{
    if(getSeriesDataNum(Yindex, seriesindex) > 0)
        addSeriesData(Yindex, seriesindex, getData(Yindex, seriesindex).last().x()+1, y);
    else
        addSeriesData(Yindex, seriesindex, 0, y);

}

void MySingleChart::addSeriesData(int Yindex, int seriesindex, double x, double y)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        int num = getSeriesDataNum(Yindex, seriesindex);
        myDataPoint[Yindex][seriesindex].append(QPointF(x,y));
        if(!ifRemoveData(Yindex, seriesindex, false))
        {
            if(isMaxMinY)
                //y最大最小值
                compareMaxMinValue(Yindex, seriesindex, num, y);
            if(isMaxMinX)
                //x最大和最小
                compareMaxMinX(Yindex, seriesindex, num, num);
        }
        else
        {
            //因为删除了最大最小，重置
            resetMaxMinIndex(Yindex,seriesindex);
            for (int i = 0; i < num; ++i) {
                if(isMaxMinY)
                    compareMaxMinValue(Yindex, seriesindex, i, getDataY(Yindex, seriesindex, i));
                if(isMaxMinX)
                    compareMaxMinX(Yindex, seriesindex, i, getDataX(Yindex, seriesindex, i));
            }
        }
        if(!isUseTimer)
            repleaseData(Yindex,seriesindex);
    }
}

void MySingleChart::replaceSeriesData(int Yindex, int seriesindex, const QList<QPointF> &data)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        myDataPoint[Yindex][seriesindex]=data;
        //因为替换,重置索引
        resetMaxMinIndex(Yindex, seriesindex);
        //先替换并且去掉多余的
        ifRemoveData(Yindex, seriesindex, true);
        //然后计算最大最小
        int num = getSeriesDataNum(Yindex, seriesindex);
        for (int i = 0; i < num; ++i) {
            if(isMaxMinY)
                compareMaxMinValue(Yindex, seriesindex, i, getDataY(Yindex, seriesindex, i));
            if(isMaxMinX)
                compareMaxMinX(Yindex, seriesindex, i, getDataX(Yindex, seriesindex, i));
        }
        if(!isUseTimer)
            repleaseData(Yindex, seriesindex);
    }
}

void MySingleChart::addSeriesYDataAndMoveWithCount(int Yindex, int seriesindex, double y)
{
    addSeriesYData(Yindex, seriesindex, y);
    //如果数量超过往右边移动
   // moveWithCount(Yindex, seriesindex);
}

void MySingleChart::addSeriesDataAndMoveWithCount(int Yindex, int seriesindex, double x, double y)
{
    addSeriesData(Yindex, seriesindex, x, y);
    //如果数量超过往右边移动
    moveWithCount(Yindex, seriesindex);
}

void MySingleChart::replaceSeriesDataAndMoveWithCount(int Yindex, int seriesindex, const QList<QPointF> &data)
{
    replaceSeriesData(Yindex, seriesindex, data);
    //如果数量超过往右边移动
    moveWithCount(Yindex, seriesindex);
}

void MySingleChart::addSeriesDataAndMoveWithMax(int Yindex, int seriesindex, double x, double y)
{
    setIsCalculateMaxMinX(true);
    addSeriesData(Yindex, seriesindex, x, y);
    //如果数量超过往右边移动
    moveWithMax(Yindex, seriesindex);
}

void MySingleChart::replaceSeriesDataAndMoveWithMax(int Yindex, int seriesindex, const QList<QPointF> &data)
{
    setIsCalculateMaxMinX(true);
    replaceSeriesData(Yindex, seriesindex, data);
    //如果数量超过往右边移动
    moveWithMax(Yindex, seriesindex);
}

void MySingleChart::clearYAxisData(int Yindex)
{
    int num = getYAxisSeriesNum(Yindex);
    for (int i = 0; i < num; ++i) {
        clearSeriesData(Yindex, i);
    }
}

void MySingleChart::clearAllYAxisData()
{
    int num = axisY.count();
    for (int i = 0; i < num; ++i) {
        clearYAxisData(i);
    }
}

void MySingleChart::clearSeriesData(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        myDataPoint[Yindex][seriesindex].clear();
        resetMaxMinIndex(Yindex, seriesindex);
        repleaseData(Yindex, seriesindex);
    }
}

void MySingleChart::clearAllSeriesData()
{
    int num = axisY.count();
    for (int i = 0; i < num; i++)
    {
        clearYAxisData(i);
    }
}

int MySingleChart::getSeriesDataNum(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
        return myDataPoint.at(Yindex).at(seriesindex).count();
    return noNum;
}

bool MySingleChart::isDataNumExist(int Yindex, int seriesindex, int index)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
        if(index < getSeriesDataNum(Yindex, seriesindex))
            return true;
    return false;
}

double MySingleChart::getDataX(int Yindex, int seriesindex, int index)
{
    if(isDataNumExist(Yindex, seriesindex, index))
        return myDataPoint.at(Yindex).at(seriesindex).at(index).x();
    return infiniteMin;
}

double MySingleChart::getDataY(int Yindex, int seriesindex, int index)
{
    if(isDataNumExist(Yindex, seriesindex, index))
        return myDataPoint.at(Yindex).at(seriesindex).at(index).y();
    return infiniteMin;
}

QPointF MySingleChart::getData(int Yindex, int seriesindex, int index)
{
    if(isDataNumExist(Yindex, seriesindex, index))
        return myDataPoint.at(Yindex).at(seriesindex).at(index);
    return QPointF(infiniteMin,infiniteMin);
}

QList<QPointF> MySingleChart::getData(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex) && getSeriesDataNum(Yindex, seriesindex))
        return myDataPoint.at(Yindex).at(seriesindex);
    return QList<QPointF>({QPointF(infiniteMin,infiniteMin)});
}

void MySingleChart::setIsCalculateMaxMinY(bool ismaxminy)
{
    isMaxMinY = ismaxminy;
}

void MySingleChart::setIsCalculateMaxMinX(bool ismaxminx)
{
    isMaxMinX = ismaxminx;
}

void MySingleChart::setUseTimer(bool use)
{
    isUseTimer = use;
    if(isUseTimer)
        freshTiemr->start();
    else
        freshTiemr->stop();
}

void MySingleChart::setTimerFrequency(int fre)
{
    if(fre > 0)
        freshTiemr->setInterval(fre);
}

double MySingleChart::getSeriesMaxValueY(int Yindex, int seriesindex)
{
    if(isMaxMinY)
        return getMaxValuePoint(Yindex, seriesindex).y();
    else
        return calculateSeriesMaxValueY(Yindex, seriesindex);
}

double MySingleChart::getSeriesMaxValueX(int Yindex, int seriesindex)
{
    if(isMaxMinY)
        return getMaxValuePoint(Yindex, seriesindex).x();
    else
        return calculateSeriesMaxValueX(Yindex, seriesindex);
}

double MySingleChart::getSeriesMinValueY(int Yindex, int seriesindex)
{
    if(isMaxMinY)
        return getMinValuePoint(Yindex, seriesindex).y();
    else
        return calculateSeriesMinValueY(Yindex, seriesindex);
}

double MySingleChart::getSeriesMinValueX(int Yindex, int seriesindex)
{
    if(isMaxMinY)
        return getMinValuePoint(Yindex, seriesindex).x();
    else
        return calculateSeriesMinValueX(Yindex, seriesindex);
}

int MySingleChart::getSeriesMaxValueIndex(int Yindex, int seriesindex)
{
    if(isMaxMinY)
    {
        if(isMaxMinPointExist(Yindex, seriesindex))
            return myMaxDataIndex.at(Yindex).at(seriesindex);
        return noIndex;
    }
    else
        return calculateSeriesMaxValueIndex(Yindex, seriesindex);
}

int MySingleChart::getSeriesMinValueIndex(int Yindex, int seriesindex)
{
    if(isMaxMinY)
    {
        if(isMaxMinPointExist(Yindex, seriesindex))
            return myMinDataIndex.at(Yindex).at(seriesindex);
        return noIndex;
    }
    else
        return calculateSeriesMinValueIndex(Yindex, seriesindex);
}

bool MySingleChart::getYAxisMaxValueSeries(int Yindex, int &seriesindex, int &index)
{
    //获取初始最大值没有就是正负无穷
    double MaxMinData = infiniteMin;
    int num = getYAxisSeriesNum(Yindex);
    seriesindex = noIndex;
    //比较和替换
    for (int i = 0; i < num; ++i) {
        double data = getSeriesMaxValueY(Yindex, i);
        if (data > MaxMinData)
        {
            seriesindex = i;
            MaxMinData = data;
        }
    }
    //获取索引
    index = getSeriesMaxValueIndex(Yindex, seriesindex);
    if(seriesindex != noIndex)
        return true;
    else
        return false;
}

bool MySingleChart::getYAxisMinValueSeries(int Yindex, int &seriesindex, int &index)
{
    double MaxMinData = infiniteMax;
    int num = getYAxisSeriesNum(Yindex);
    seriesindex = noIndex;
    for (int i = 0; i < num; ++i) {
        double data = getSeriesMinValueY(Yindex, i);
        if (data < MaxMinData)
        {
            seriesindex = i;
            MaxMinData = data;
        }
    }
    index = getSeriesMinValueIndex(Yindex, seriesindex);
    if(seriesindex != noIndex)
        return true;
    else
        return false;
}

double MySingleChart::getYAxisMaxValueX(int Yindex)
{
    int seriesindex ;
    int index;
    if(getYAxisMaxValueSeries(Yindex, seriesindex, index))
    {
        return getDataX(Yindex, seriesindex, index);
    }
    return infiniteMin;
}

double MySingleChart::getYAxisMinValueX(int Yindex)
{
    int seriesindex ;
    int index;
    if(getYAxisMinValueSeries(Yindex, seriesindex, index))
    {
        return getDataX(Yindex, seriesindex, index);
    }
    return infiniteMax;
}

double MySingleChart::getYAxisMaxValueY(int Yindex)
{
    int seriesindex ;
    int index;
    if(getYAxisMaxValueSeries(Yindex, seriesindex, index))
    {
        return getDataY(Yindex, seriesindex, index);
    }
    return infiniteMin;
}

double MySingleChart::getYAxisMinValueY(int Yindex)
{
    int seriesindex ;
    int index;
    if(getYAxisMinValueSeries(Yindex, seriesindex, index))
    {
        return getDataY(Yindex, seriesindex, index);
    }
    return infiniteMax;
}

bool MySingleChart::getAllYAxisMaxValueYAxis(int &Yindex, int &seriesindex, int &index)
{
    int num =getYAxisNum();
    //获取给初始最大，没有就是正负无穷
    Yindex = noIndex;
    double MaxMinData = infiniteMin;
    for (int i = 0; i < num; ++i) {
        double data = getYAxisMaxValueY(i);
        if (data > MaxMinData)
        {
            Yindex = i;
            MaxMinData = data;
        }
    }
    getYAxisMaxValueSeries(Yindex, seriesindex, index);
    if(Yindex != noIndex)
        return true;
    else
        return false;
}

bool MySingleChart::getAllYAxisMinValueYAxis(int &Yindex, int &seriesindex, int &index)
{
    int num =getYAxisNum();
    //获取给初始最大，没有就是正负无穷
    Yindex = noIndex;
    double MaxMinData = infiniteMax;
    for (int i = 0; i < num; ++i) {
        double data = getYAxisMinValueY(i);
        if (data < MaxMinData)
        {
            Yindex = i;
            MaxMinData = data;
        }
    }
    getYAxisMinValueSeries(Yindex, seriesindex, index);
    if(Yindex != noIndex)
        return true;
    else
        return false;
}

double MySingleChart::getAllYAxisMaxValueX()
{
    int Yindex;
    int seriesindex ;
    int index;
    if(getAllYAxisMaxValueYAxis(Yindex, seriesindex, index))
    {
        return getDataX(Yindex, seriesindex, index);
    }
    return infiniteMin;
}

double MySingleChart::getAllYAxisMinValueX()
{
    int Yindex;
    int seriesindex ;
    int index;
    if(getAllYAxisMinValueYAxis(Yindex, seriesindex, index))
    {
        return getDataX(Yindex, seriesindex, index);
    }
    return infiniteMax;
}

double MySingleChart::getAllYAxisMaxValueY()
{
    int Yindex;
    int seriesindex ;
    int index;
    if(getAllYAxisMaxValueYAxis(Yindex, seriesindex, index))
    {
        return getDataY(Yindex, seriesindex, index);
    }
    return infiniteMin;
}

double MySingleChart::getAllYAxisMinValueY()
{
    int Yindex;
    int seriesindex ;
    int index;
    if(getAllYAxisMinValueYAxis(Yindex, seriesindex, index))
    {
        return getDataY(Yindex, seriesindex, index);
    }
    return infiniteMax;
}

double MySingleChart::getSeriesMaxX(int Yindex, int seriesindex)
{
    if(isMaxMinX)
        return getMaxXPoint(Yindex, seriesindex).x();
    else
        return calculateSeriesMaxX(Yindex, seriesindex);
}

double MySingleChart::getSeriesMinX(int Yindex, int seriesindex)
{
    if(isMaxMinX)
        return getMinXPoint(Yindex, seriesindex).x();
    else
        return calculateSeriesMinX(Yindex, seriesindex);
}

double MySingleChart::getSeriesMaxXIndex(int Yindex, int seriesindex)
{
    if(isMaxMinX)
    {
        if(isMaxMinXExist(Yindex, seriesindex))
            return myMaxXIndex.at(Yindex).at(seriesindex);
        return noIndex;
    }
    else
        return calculateSeriesMaxXIndex(Yindex, seriesindex);
}

double MySingleChart::getSeriesMinXIndex(int Yindex, int seriesindex)
{
    if(isMaxMinX)
    {
        if(isMaxMinXExist(Yindex, seriesindex))
            return myMinXIndex.at(Yindex).at(seriesindex);
        return noIndex;
    }
    else
        return calculateSeriesMinXIndex(Yindex, seriesindex);
}

double MySingleChart::getXAxisMaxValueInYAxis(int Yindex)
{
    int num = getYAxisSeriesNum(Yindex);
    double maxmin = infiniteMin;
    for (int i = 0; i < num; ++i) {
        double x = getSeriesMaxX(Yindex, i);
        if(x > maxmin)
            maxmin = x;
    }
    return maxmin;
}

double MySingleChart::getXAxisMinValueInYAxis(int Yindex)
{
    int num = getYAxisSeriesNum(Yindex);
    double maxmin = infiniteMax;
    for (int i = 0; i < num; ++i) {
        double x = getSeriesMinX(Yindex, i);
        if(x < maxmin)
            maxmin = x;
    }
    return maxmin;
}

double MySingleChart::getXAxisMaxValue()
{
    int num = getYAxisNum();
    double maxmin = infiniteMin;
    for (int i = 0; i < num; ++i) {
        double x = getXAxisMaxValueInYAxis(i);
        if(x > maxmin)
            maxmin = x;
    }
    return maxmin;
}

double MySingleChart::getXAxisMinValue()
{
    int num = getYAxisNum();
    double maxmin = infiniteMax;
    for (int i = 0; i < num; ++i) {
        double x = getXAxisMinValueInYAxis(i);
        if(x < maxmin)
            maxmin = x;
    }
    return maxmin;
}

void MySingleChart::setLegend(Direct direct)
{
    Qt::Alignment alignment(direct);

    if (!alignment) {
        myChart->legend()->hide();
    } else {
        myChart->legend()->setAlignment(alignment);
        myChart->legend()->show();
    }
}

void MySingleChart::setSeriesAppropriateView(int Ynumer, int seriesindex, double scale)
{
    if(getSeriesDataNum(Ynumer, seriesindex))
    {
        double x_max = getSeriesMaxX(Ynumer, seriesindex);
        double x_min = getSeriesMinX(Ynumer, seriesindex);
        setXAxisScale(x_max, x_min, scale);
        double y_max = getSeriesMaxValueY(Ynumer, seriesindex);
        double y_min = getSeriesMinValueY(Ynumer, seriesindex);
        setYAxisScale(Ynumer, y_max, y_min, scale);
        saveAxisRange();
    }
}

void MySingleChart::setYAxisAppropriateView(int Ynumer, double scale)
{
    if(getYAxisSeriesNum(Ynumer))
    {
        double x_max = getXAxisMaxValueInYAxis(Ynumer);
        double x_min = getXAxisMinValueInYAxis(Ynumer);
        setXAxisScale(x_max, x_min, scale);
        double y_max = getYAxisMaxValueY(Ynumer);
        double y_min = getYAxisMinValueY(Ynumer);
        setYAxisScale(Ynumer, y_max, y_min, scale);
        saveAxisRange();
    }
}

void MySingleChart::setAppropriateView(double scale)
{
    int num = getYAxisNum();
    if(num)
    {
        double x_max = getXAxisMaxValue();
        double x_min = getXAxisMinValue();
        setXAxisScale(x_max, x_min, scale);
        for (int i = 0; i < num; ++i) {
            double y_max = getYAxisMaxValueY(i);
            double y_min = getYAxisMinValueY(i);
            setYAxisScale(i, y_max, y_min, scale);
        }
        saveAxisRange();
    }
}

bool MySingleChart::savePicture(bool finishsaveopen, bool openpicture)
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("保存图像"),
                                                    "",
                                                    tr("*.bmp;; *.png;; *.jpg;; *.tif")); //选择路径
    if(savePictureIn(filename))
    {
        if(finishsaveopen)
        {
            if(!openpicture)
            {
                QFileInfo fileInfo(filename);
                filename = fileInfo.path();
            }
            QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
        }
        return true;
    }
    return false;
}

bool MySingleChart::savePictureIn(QString filename)
{
    if (filename.isEmpty())
    {
        return false;
    }
    QPixmap p = this->grab();
    QImage image = p.toImage();
    if (!(image.save(filename))) //保存图像
    {
        return false;
    }
    return true;
}

void MySingleChart::setSeriesName(int Yindex, int seriesindex, QString name)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
    {
        myLineSeries.at(Yindex).at(seriesindex)->setName(name);
        myScatterSeries.at(Yindex).at(seriesindex)->setName(name);
    }
}

bool MySingleChart::getSeriesShowProperty(int Yindex, int series)
{
    if(isYAxisSeriesExist(Yindex, series))
        return mySeriesShowProPerty.at(Yindex).at(series);
    return false;
}

bool MySingleChart::isSeriesShow(int Yindex, int series)
{
    if(isYAxisSeriesExist(Yindex, series))
        return myLineSeries.at(Yindex).at(series)->isVisible();
    return false;
}

void MySingleChart::setSeriesShowHideProperty(int Yindex, int series, bool isshow)
{
    if(isYAxisSeriesExist(Yindex, series))
    {
        //先设置属性
        mySeriesShowProPerty[Yindex][series] = isshow;
        setSeriesShowHide(Yindex, series, isshow);
    }
}

void MySingleChart::setSeriesShowHide(int Yindex, int series, bool isshow)
{
    if(isYAxisSeriesExist(Yindex, series))
    {
        if(isYAxisShow(Yindex))
        {
            myLineSeries.at(Yindex).at(series)->setVisible(isshow && getSeriesShowProperty(Yindex, series));
            myScatterSeries.at(Yindex).at(series)->setVisible(isshow && isShowScatter && getSeriesShowProperty(Yindex, series));
            hidePointLegend();
        }
        else
        {
            myLineSeries.at(Yindex).at(series)->setVisible(false);
            myScatterSeries.at(Yindex).at(series)->setVisible(false);
        }
    }
}

void MySingleChart::setYAxisAllSeriesShowHideProperty(int Yindex, bool isshow)
{
    int num = getYAxisSeriesNum(Yindex);
    for (int i = 0; i < num; ++i) {
        setSeriesShowHideProperty(Yindex, i, isshow);
    }
}



void MySingleChart::setAxisGridLineVisible(bool visible)
{
    axisX->setGridLineVisible(visible);  //网格线可见
    int num = axisY.count();
    for (int i = 0; i < num; i++) {
        axisY.at(i)->setGridLineVisible(visible);  //网格线可见
    }
}

void MySingleChart::setXAxisTickCountNumber(int tickcount)
{
    axisX->setTickCount(tickcount);
}

void MySingleChart::setYAxisTickCountNumber(int tickcount)
{
    int num = axisY.count();
    for (int i = 0; i < num; i++) {
        axisY.at(i)->setTickCount(tickcount);   //刻度线数量
    }
}

void MySingleChart::setXAxisLineColor(QColor color)
{
    //轴线
    axisX->setLinePenColor(color);
}


void MySingleChart::setXAxisGridLineColor(QColor color)
{
    //网格线
    axisX->setGridLineColor(color);
}

void MySingleChart::setXAxisTilteColor(QColor color)
{
    //标题
    axisX->setTitleBrush(QBrush(color));
}

void MySingleChart::setXAxisLabelColor(QColor color)
{
    //刻度
    axisX->setLabelsColor(color);
}

void MySingleChart::setXAxisTitleLabelColor(QColor color)
{
    setXAxisTilteColor(color);
    setXAxisLabelColor(color);
}

void MySingleChart::setXAxisTitleLabelLineColor(QColor color)
{
    setXAxisTitleLabelColor(color);
    setXAxisLineColor(color);
}

void MySingleChart::setXAxisAllColor(QColor color)
{
    setXAxisTitleLabelLineColor(color);
    setXAxisGridLineColor(color);
}



void MySingleChart::setYAxisLineColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
        axisY.at(Yindex)->setLinePenColor(color);
}

void MySingleChart::setYAxisGridLineColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
        axisY.at(Yindex)->setGridLineColor(color);
}

void MySingleChart::setYAxisTilteColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
        axisY.at(Yindex)->setTitleBrush(QBrush(color));
}

void MySingleChart::setYAxisLabelColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
        axisY.at(Yindex)->setLabelsColor(color);
}

void MySingleChart::setYAxisTitleLabelColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
    {
        setYAxisTilteColor(Yindex, color);
        setYAxisLabelColor(Yindex, color);
    }
}

void MySingleChart::setYAxisTitleLabelLineColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
    {
        setYAxisTitleLabelColor(Yindex, color);
        setYAxisLineColor(Yindex, color);
    }
}

void MySingleChart::setYAxisAllColor(int Yindex, QColor color)
{
    if(isYAxisExist(Yindex))
    {
        setYAxisTitleLabelLineColor(Yindex, color);
        setYAxisGridLineColor(Yindex, color);
    }
}

void MySingleChart::setAllYColor(QColor color)
{
    int num = axisY.count();
    for (int i = 0; i < num; ++i) {
        setYAxisAllColor(i, color);
    }
}

void MySingleChart::setXAxisInteger()
{
    axisX->setLabelFormat("%d");
}

void MySingleChart::setYAxisInteger(int Yindex)
{
    if(isYAxisExist(Yindex))
        axisY.at(Yindex)->setLabelFormat("%d");
}

void MySingleChart::setAllYAxisInteger()
{
    int num = axisY.count();
    for (int i = 0; i < num; i++) {
        axisY.at(i)->setLabelFormat("%d");
    }
}

void MySingleChart::setXAxisFloat(int flo)
{
    axisX->setLabelFormat("%." + QString::number(flo) + "f");
}

void MySingleChart::setYAxisFloat(int Yindex, int flo)
{
    if(isYAxisExist(Yindex))
        axisY.at(Yindex)->setLabelFormat("%." + QString::number(flo) + "f");
}

void MySingleChart::setAllYAxisFloat(int flo)
{
    int num = axisY.count();
    for (int i = 0; i < num; i++) {
        axisY.at(i)->setLabelFormat("%." + QString::number(flo) + "f");
    }
}

void MySingleChart::setXAxisMin(double min)
{
    axisX->setMin(min);
    m_ChartView->saveAxisRange();
}

void MySingleChart::setXAxisMax(double max)
{
    axisX->setMax(max);
    m_ChartView->saveAxisRange();
}

void MySingleChart::setSeriesColor(int Yindex, int seriesindex,  QColor color)
{
    if(isYAxisExist(Yindex)){
        myLineSeries.at(Yindex).at(seriesindex)->setColor(color);
        myScatterSeries.at(Yindex).at(seriesindex)->setColor(color);
    }
}

void MySingleChart::setAxisAllSeriesColor(int Yindex,  QColor color)
{
    int num = getYAxisSeriesNum(Yindex);
    for (int i = 0; i < num; ++i) {
        setSeriesColor(Yindex, i, color);
    }
}

void MySingleChart::setAxisAndAllSeriesColor(int Yindex, QColor color)
{
    setAxisAllSeriesColor(Yindex, color);
    setYAxisTitleLabelLineColor(Yindex, color);
}

void MySingleChart::refreshAllData()
{
    for (int i = 0; i < getYAxisNum(); ++i) {
        for (int j = 0; j < getYAxisSeriesNum(i); ++j) {
            repleaseData(i,j);
        }
    }
}

void MySingleChart::setMaxDataCount(int count)
{
    if(count >= 0)
    {
        maxDataCount = count;
        //如果当前已经有数据了,则计算一遍
        for (int i = 0; i < getYAxisNum(); ++i) {
            for (int j = 0; j < getYAxisSeriesNum(i); ++j) {
                if(getSeriesDataNum(i, j) > 0)
                    ifRemoveData(i, j, true);
            }
        }
    }
}

QList<QPointF> MySingleChart::getRemainData(int Yindex, int seriesindex)
{
    if(isYAxisSeriesExist(Yindex, seriesindex))
        return myDataPoint[Yindex][seriesindex].mid(getSeriesDataNum(Yindex, seriesindex) - currentDataCount.at(Yindex).at(seriesindex));
    return QList<QPointF>({QPointF(infiniteMin,infiniteMin)});
}

int MySingleChart::getMaxDataCount()
{
    return maxDataCount;
}

void MySingleChart::setAllAxisColor(QColor color)
{
    setXAxisAllColor(color);
    setAllYColor(color);
}

void MySingleChart::resetXAxisColor()
{
    axisX->setGridLinePen(xPen);
    axisX->setLinePen(xPen);
    axisX->setTitleBrush(xbrush);
    axisX->setLabelsBrush(xbrush);
}

void MySingleChart::resetYAxisColor(int Yindex)
{
    if(isYAxisExist(Yindex))
    {
        axisY.at(Yindex)->setGridLinePen(xPen);
        axisY.at(Yindex)->setLinePen(xPen);
        axisY.at(Yindex)->setTitleBrush(xbrush);
        axisY.at(Yindex)->setLabelsBrush(xbrush);
    }
}

void MySingleChart::resetAllYAxisColor()
{
    int num = axisY.count();
    for (int i = 0; i < num; ++i) {
        resetYAxisColor(i);
    }
}

void MySingleChart::resetAllAxisColor()
{
    // 设置网格线颜色
    resetXAxisColor();
    resetAllYAxisColor();
}

void MySingleChart::setXAxisTitleSize(int size)
{
    QFont xFont = axisX->titleFont(); // 获取当前X轴标题字体
    xFont.setPixelSize(size);          // 设置X轴标题字体大小
    axisX->setTitleFont(xFont);      // 应用新的字体设置
}

void MySingleChart::setYAxisTitleSize(int Yindex, int size)
{
    if(isYAxisExist(Yindex))
    {
        QFont yFont = axisY.at(Yindex)->titleFont(); // 获取当前X轴标题字体
        yFont.setPixelSize(size);          // 设置Y轴标题字体大小
        axisY.at(Yindex)->setTitleFont(yFont);      // 应用新的字体设置
    }
}

void MySingleChart::setAllYAxisTitleSize(int size)
{
    int num = getYAxisNum();
    for (int i = 0; i < num; ++i) {
        setYAxisTitleSize(i, size);
    }
}

void MySingleChart::test(int axisnum, int seriesnum, int datanum, int miny, int maxy)
{
    int change = 1000/datanum;
    //const double xScale = (2 * M_PI * 10) / 1000;
    double x;
    double y;
    // 使用 C++11 的随机数生成
    std::random_device rd; // 用于获取随机种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 算法
    std::uniform_int_distribution<> dis(miny, maxy); // 定义范围

    addYAxisToNumber(axisnum);
    for (int i = 0; i < axisnum; ++i) {
        addYAxisSeriesToNumber(i, seriesnum);
    }

    for (int i = 0; i < axisnum; ++i) {
        for (int j = 0; j < seriesnum; ++j) {
            for (int z = 0; z < datanum; ++z) {
                // 计算x值
                x = z * change ;
                // 计算对应的y值，这里假设振幅为1
                y =dis(gen);
                // 将QPointF添加到列表中
                addSeriesDataAndMoveWithCount(i, j, x, y);
            }
        }
    }
}

QValueAxis *MySingleChart::creatAxis(QString name)
{
    QValueAxis* axis = new QValueAxis();
    axis->setRange(defaultMin, defaultMax);
    axis->setTickCount(defaultGrideLineNum);   //刻度线数量
    axis->setTitleText(name);  // 轴名称
    axis->setGridLineVisible(true);  //网格线可见
    axis->setLabelFormat("%d");
    return axis;
}

void MySingleChart::addXAxis()
{
    axisX = creatAxis();
    myChart->addAxis(axisX, Qt::AlignBottom);
}

void MySingleChart::removeXAxis()
{
    myChart->removeAxis(axisX);
    delete axisX;
    axisX =nullptr;
}

void MySingleChart::addYAxis(bool alignleft)
{
    axisY.append(creatAxis("轴" + QString::number(getYAxisNum()) + ":Count"));
    // Y轴位于左还是右
    if(alignleft)
        myChart->addAxis(axisY.at(axisY.count() - 1), Qt::AlignLeft);
    else
        myChart->addAxis(axisY.at(axisY.count() - 1), Qt::AlignRight);
    //初始化Y轴对应曲线数组
    QList<QLineSeries *> axislineseries;
    myLineSeries.append(axislineseries);
    QList<QScatterSeries *> axisscatterseries;
    myScatterSeries.append(axisscatterseries);
    QList<bool> axisseriesshow;
    mySeriesShowProPerty.append(axisseriesshow);
    QList<int> datacount;
    currentDataCount.append(datacount);
    //初始化Y轴对应数组
    QList<QList<QPointF>> axisdata;
    myDataPoint.append(axisdata);
    QList<int> maxdataindex;
    myMaxDataIndex.append(maxdataindex);
    QList<int> mindataindex;
    myMinDataIndex.append(mindataindex);
    QList<int> maxxindex;
    myMaxXIndex.append(maxxindex);
    QList<int> minxindex;
    myMinXIndex.append(minxindex);
    saveAxisRange();
}

void MySingleChart::addYAxisToNumber(int number, bool alignleft)
{
    int num = number - axisY.count();
    for (int i = 0; i < num; ++i) {
        addYAxis(alignleft);
    }
}

void MySingleChart::removeYAxis(int Yindex)
{
    if(isYAxisExist(Yindex))
    {
        removeAxisYAllSeries(Yindex);
        myLineSeries.removeAt(Yindex);
        myScatterSeries.removeAt(Yindex);
        mySeriesShowProPerty.removeAt(Yindex);
        currentDataCount.removeAt(Yindex);
        myDataPoint.removeAt(Yindex);
        myMaxDataIndex.removeAt(Yindex);
        myMinDataIndex.removeAt(Yindex);
        myMaxXIndex.removeAt(Yindex);
        myMinXIndex.removeAt(Yindex);
        //删除Y轴
        myChart->removeAxis(axisY.at(Yindex));
        delete axisY.at(Yindex);
        axisY[Yindex] = nullptr;
        axisY.removeAt(Yindex);
        saveAxisRange();
    }

}

void MySingleChart::removeAllYAxis()
{
    // 遍历外层列表,不能把 axisY.count()写在里面,每次循环判断一次
    int num = axisY.count();
    for (int i = 0; i < num; ++i) {
        // 遍历内层列表
        removeYAxis(0);
    }
    myLineSeries.clear();  // 清空外层列表
    myScatterSeries.clear();
    axisY.clear();
}

void MySingleChart::setYAxisAlign(int Yindex, bool alignleft)
{
    if(isYAxisExist(Yindex))
    {
        myChart->removeAxis(axisY.at(Yindex));
        if(alignleft)
            myChart->addAxis(axisY.at(Yindex), Qt::AlignLeft);
        else
            myChart->addAxis(axisY.at(Yindex), Qt::AlignRight);
    }
}

void MySingleChart::setXAxisAlign(bool alignbottom)
{
    myChart->removeAxis(axisX);
    if(alignbottom)
        myChart->addAxis(axisX, Qt::AlignBottom);
    else
        myChart->addAxis(axisX, Qt::AlignTop);

}
