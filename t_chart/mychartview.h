#ifndef MYCHARTVIEW_H
#define MYCHARTVIEW_H

#include <QObject>
#include <QtCharts>

class MyChartView : public QChartView
{
    Q_OBJECT

public:
    explicit MyChartView(QChart *chart, QWidget *parent = nullptr);

    //保存原始坐标轴，以及恢复
    void saveAxisRange();
    void restoreAxisRange();
    //注意要显示后才能使用
    //x轴y轴放大缩小,从中心点
    void XAxisChange(bool enlarge);
    void YAxisChange(bool enlarge, int Yindex);
    void allYAxisChange(bool enlarge);
    //x轴y轴移动 默认单次比例
    void XAxisMoveScale(bool positive , double scale = 0.1);
    void YAxisMoveScale(bool positive , int Yindex = 0, double scale = 0.1);
    void allYAxisMoveScale(bool positive, double scale = 0.1);
    void XAxisMoveDis(double distance = 100);
    void YAxisMoveDis(int Yindex = 0, double distance = 100);
    void allYAxisMoveDis(double distance = 100);
    //改变缩放比例
    void changeScale(double scale);

    bool isShowCursorAxis = true;
    bool isShowCursorPoint = false;
    bool isDrag = true;

protected:
    //按住拖动
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    //如果保存了数据，右键恢复保存的初始状态
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    //滚轮放大缩小功能
    virtual void wheelEvent(QWheelEvent *event) override;
    //按下ctrl键实现y轴放大缩小，展示不使用，直接是一起放大缩小
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    //移出事件
    virtual void leaveEvent(QEvent *event) override;

private:
    QPoint m_lastPoint;
    bool m_isMouseLeftDown = false;
    bool m_isctrlDown = false;
    bool m_alreadySaveRange = false;
    //保存原始数据
    double m_xMin, m_xMax;
    QList<double> minmaxY;
    QGraphicsTextItem* m_coordItem = nullptr;

    //默认缩放比例
    double factor = 1.5;
};
#endif // MYCHARTVIEW_H
