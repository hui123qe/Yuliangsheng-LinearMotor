#include "mychartview.h"

MyChartView::MyChartView(QChart *chart, QWidget *parent)
    : QChartView(chart,parent)
{
    this->setMouseTracking(false);
    //setCursor(QCursor(Qt::PointingHandCursor)); //设置鼠标指针为手指形
}

void MyChartView::saveAxisRange()
{
    //保存原始坐标轴
    minmaxY.clear();
    QValueAxis *axisX = (QValueAxis *)this->chart()->axisX();
    QList<QAbstractAxis*> axisY = this->chart()->axes(Qt::Vertical);
    //QValueAxis *axisY = (QValueAxis *)this->chart()->axisY();
    m_xMin = axisX->min();
    m_xMax = axisX->max();
    for (int i = 0; i < axisY.count(); i++)
    {
        minmaxY.append(((QValueAxis *)axisY.at(i))->min());
        minmaxY.append(((QValueAxis *)axisY.at(i))->max());
    }
    //    m_yMin = axisY->min();
    //    m_yMax = axisY->max();
    //只用保存一次
    m_alreadySaveRange = true;
}

void MyChartView::restoreAxisRange()
{
    this->chart()->axisX()->setRange(m_xMin, m_xMax);
    QList<QAbstractAxis*> axisY = this->chart()->axes(Qt::Vertical);
    for (int i = 0; i < axisY.count(); i++)
    {
        ((QValueAxis *)axisY.at(i))->setRange(minmaxY.at(2 * i),minmaxY.at(2 * i + 1));
    }
}

void MyChartView::wheelEvent(QWheelEvent *event)
{
    const QPoint curPos = event->pos();
    QPointF curVal = this->chart()->mapToValue(QPointF(curPos));

    if (!m_alreadySaveRange)
    {
        this->saveAxisRange();
    }
    //判断是否执行缩放
    bool xZoom = true;
    bool yZoom = true;
    //X轴
    QValueAxis *axisX = dynamic_cast<QValueAxis*>(this->chart()->axisX());
    const double xMin = axisX->min();
    const double xMax = axisX->max();
    const double xCentral = curVal.x();
    //Y轴
    QList<QAbstractAxis*> axisY = this->chart()->axes(Qt::Vertical);
    const double yCentral = curVal.y();
    //如果按下ctrl 离哪边近缩放哪边
    if (m_isctrlDown)
    {
        //距离y轴比例（注意距离y是和x有关）
        double yProportion = (xCentral - xMin) / (xMax - xMin);

        const double yMin = ((QValueAxis *)axisY.at(0))->min();
        const double yMax = ((QValueAxis *)axisY.at(0))->max();
        //距离y轴比例（和y有关）
        double xProportion = (yCentral - yMin) / (yMax - yMin);
        //距离哪边近缩放哪边
        if(xProportion < yProportion) //如果距离x近
        {
            yZoom = false;
        }
        else
        {
            xZoom = false;
        }
        //表示事件已经被处理，这样就不会传递给上面的函数
        //event->accept();
    }
    //根据有没有按下ctrl键 执行缩放X,Y轴
    if(yZoom)
    {
        //Y轴
        allYAxisChange(event->delta() > 0);
    }
    if(xZoom)
    {
        XAxisChange(event->delta() > 0);
    }
}

void MyChartView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        m_isctrlDown = true;
    }
}

void MyChartView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        m_isctrlDown = false;
    }
}

void MyChartView::leaveEvent(QEvent *event)
{
    m_coordItem->hide();
}

void MyChartView::XAxisChange(bool enlarge)
{
    //如果还没有保存，就保存一次原始XY坐标轴
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }

    QValueAxis *axisX = dynamic_cast<QValueAxis*>(this->chart()->axisX());
    const double xMin = axisX->min();
    const double xMax = axisX->max();
    const double xCentral = (xMax + xMin) / 2;

    double leftOffset;
    double rightOffset;
    if (enlarge)
    {//放大
        leftOffset = 1.0 / factor * (xCentral - xMin);
        rightOffset = 1.0 / factor * (xMax - xCentral);
    }
    else
    {//缩小
        leftOffset = 1.0 * factor * (xCentral - xMin);
        rightOffset = 1.0 * factor * (xMax - xCentral);
    }

    this->chart()->axisX()->setRange(xCentral - leftOffset, xCentral + rightOffset);
}

void MyChartView::YAxisChange(bool enlarge, int Yindex)
{
    //如果还没有保存，就保存一次原始XY坐标轴
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    QList<QAbstractAxis*> axisY=this->chart()->axes(Qt::Vertical);
    if (Yindex >= 0 && Yindex < axisY.count())
    {
        const double yMin = ((QValueAxis *)axisY.at(Yindex))->min();
        const double yMax = ((QValueAxis *)axisY.at(Yindex))->max();
        const double yCentral = (yMax + yMin) / 2;
        double bottomOffset;
        double topOffset;
        if (enlarge)
        {
            //放大
            bottomOffset = 1.0 / factor * (yCentral - yMin);
            topOffset = 1.0 / factor * (yMax - yCentral);
        }
        else
        {
            //缩小
            bottomOffset = 1.0 * factor * (yCentral - yMin);
            topOffset = 1.0 * factor * (yMax - yCentral);
        }

        axisY.at(Yindex)->setRange(yCentral - bottomOffset, yCentral + topOffset);
    }
}

void MyChartView::allYAxisChange(bool enlarge)
{
    QList<QAbstractAxis*> axisY=this->chart()->axes(Qt::Vertical);
    for (int i = 0; i < axisY.count(); i++)
    {
        YAxisChange(enlarge, i);
    }
}
void MyChartView::XAxisMoveScale(bool positive , double scale)
{
    //如果还没有保存，就保存一次原始XY坐标轴
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    //计算移动距离
    double offset =this->chart()->plotArea().width() * scale;
    //按dx和dy指定的距离滚动图表的可见区
    if(positive)
    {
        this->chart()->scroll(offset, 0);
    }
    else
    {
        this->chart()->scroll(-offset, 0);
    }
}

void MyChartView::YAxisMoveScale(bool positive , int Yindex, double scale)
{
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    //计算移动距离
    double offset = this->chart()->plotArea().height() * scale;
    if(!positive)
        offset = -offset;
    //按dx和dy指定的距离滚动图表的可见区
    QList<QAbstractAxis*> axisY=this->chart()->axes(Qt::Vertical);
    qreal cur_y_min = ((QValueAxis *)axisY.at(Yindex))->min();
    qreal cur_y_max = ((QValueAxis *)axisY.at(Yindex))->max();
    ((QValueAxis *)axisY.at(Yindex))->setRange(cur_y_min + offset, cur_y_max + offset);

}

void MyChartView::allYAxisMoveScale(bool positive, double scale)
{
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    //计算移动距离
    double offset =this->chart()->plotArea().height() * scale;
    //按dx和dy指定的距离滚动图表的可见区
    if(positive)
    {
        this->chart()->scroll(0, offset);
    }
    else
    {
        this->chart()->scroll(0, -offset);
    }
}

void MyChartView::XAxisMoveDis(double distance)
{
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    this->chart()->scroll(distance, 0);
}

void MyChartView::YAxisMoveDis(int Yindex, double distance)
{
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    //按dx和dy指定的距离滚动图表的可见区域
    QList<QAbstractAxis*> axisY=this->chart()->axes(Qt::Vertical);
    qreal cur_y_min = ((QValueAxis *)axisY.at(Yindex))->min();
    qreal cur_y_max = ((QValueAxis *)axisY.at(Yindex))->max();
    ((QValueAxis *)axisY.at(Yindex))->setRange(cur_y_min + distance, cur_y_max + distance);
}

void MyChartView::allYAxisMoveDis(double distance)
{
    if(!m_alreadySaveRange){
        this->saveAxisRange();
    }
    this->chart()->scroll(0, distance);
}

void MyChartView::changeScale(double scale)
{
    if(scale>1)
    {
        factor=scale;
    }
}

void MyChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_lastPoint = event->pos();
        //鼠标左键按下
        if(isDrag)
            m_isMouseLeftDown=true;
    }
}

void MyChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_coordItem)
    {
        m_coordItem = new QGraphicsTextItem(this->chart());
        m_coordItem->setZValue(5);
        m_coordItem->setDefaultTextColor(QColor(60, 187, 251));
        m_coordItem->show();
    }


    QValueAxis *axisX = (QValueAxis *)this->chart()->axisX();
    QList<QAbstractAxis*> axisY=this->chart()->axes(Qt::Vertical);
    float max_x = axisX->max();
    float min_x = axisX->min();
    float max_y = ((QValueAxis *)axisY.at(0))->max();
    float min_y = ((QValueAxis *)axisY.at(0))->min();

    //QValueAxis *axisY = (QValueAxis *)this->chart()->axisY();
    const QPoint curPos = event->pos();
    QPointF curVal = this->chart()->mapToValue(QPointF(curPos));
    QString coordStr;
    // 检测鼠标位置是否在某条曲线上
    if(isShowCursorPoint)
    {
        QString currentSeriesName = "未知曲线";
        qreal threshold = 4.0;  // 设置阈值为5.0（可以根据需要调整）
        QPointF nearestPoint;   // 用于存储最近的点
        qreal minDistance = threshold + 1;  // 初始化最小距离为阈值加1，确保初始时不会满足条件
        QString nearestSeriesName;  // 用于存储最近的曲线名称

        // 遍历所有曲线
        for (QAbstractSeries* series : this->chart()->series()) {
            if (series->type() == QAbstractSeries::SeriesTypeLine) {
                QLineSeries* lineSeries = static_cast<QLineSeries*>(series);
                qreal curveMinDistance = threshold + 1;  // 当前曲线的最小距离
                QPointF curveNearestPoint;  // 当前曲线的最近点

                // 遍历曲线上的所有点
                for (const QPointF& point : lineSeries->points()) {
                    // 计算点与鼠标位置的欧几里得距离
                    qreal distance = qSqrt(qPow(point.x() - curVal.x(), 2) + qPow(point.y() - curVal.y(), 2));

                    // 如果当前点的距离小于曲线的最小距离，并且在阈值范围内
                    if (distance < curveMinDistance && distance < threshold) {
                        curveMinDistance = distance;  // 更新曲线的最小距离
                        curveNearestPoint = point;    // 更新曲线的最近点
                    }
                }

                // 如果当前曲线有满足条件的点，并且距离比之前记录的最小距离更小
                if (curveMinDistance < minDistance && curveMinDistance < threshold) {
                    minDistance = curveMinDistance;  // 更新全局最小距离
                    nearestPoint = curveNearestPoint;  // 更新全局最近点
                    nearestSeriesName = series->name();  // 更新最近的曲线名称
                }
            }
        }
        // 如果找到了满足条件的曲线和点
        if (minDistance != threshold + 1) {
            currentSeriesName = nearestSeriesName;
            coordStr += "曲线名称: " + currentSeriesName + "\n";
            QString pointx = QString::number(nearestPoint.x());
            QString pointy = QString::number(nearestPoint.y());
            coordStr += "点X :" + pointx + "\n" + "点Y :" + pointy + "\n";
        }
    }
    if(isShowCursorAxis)
    {
        coordStr += "X :" + QString::number(curVal.x())+ "\n"
                    + "Y" + "1" + ":" + QString::number(curVal.y());
        if(axisY.count() > 1)
        {
            double proportion = (curVal.y() - min_y)/(max_y - min_y);
            for (int i = 1; i < axisY.count(); i++)
            {
                float max = ((QValueAxis *)axisY.at(i))->max();
                float min = ((QValueAxis *)axisY.at(i))->min();
                double position = (max - min) * proportion + min;
                coordStr=coordStr+"\n"+"Y" + QString::number(i) + ":" + QString::number(position);
            }
        }
    }
    //鼠标超过chart范围，将不再显示指向坐标的数值
    if(curVal.x() >= min_x && curVal.x() <= max_x && curVal.y() >= min_y && curVal.y() <= max_y){
        m_coordItem->show();
        //显示在鼠标指向位置右偏移20px
        m_coordItem->setPos(event->pos().x() - m_coordItem->boundingRect().width(), event->pos().y() - m_coordItem->boundingRect().height());
        m_coordItem->setPlainText(coordStr);
    }
    else{
        //超过XY轴范围，隐藏
        m_coordItem->hide();
    }

    //鼠标左键按下
    if(m_isMouseLeftDown){
        QPoint offset = curPos - m_lastPoint;
        m_lastPoint = curPos;
        //如果还没有保存，就保存一次原始XY坐标轴
        if(!m_alreadySaveRange){
            this->saveAxisRange();
        }
        //左右移， 上下移 都可以，没有数据不移动
        this->chart()->scroll(-offset.x(), offset.y());
    }
}

void MyChartView::mouseReleaseEvent(QMouseEvent *event)
{
    m_isMouseLeftDown = false;
    if(event->button() == Qt::RightButton){
        //已保存坐标
        if(m_alreadySaveRange)
        {
            restoreAxisRange();
        }
    }
}
