#ifndef MYCHART_H
#define MYCHART_H

#include <QWidget>
#include "qcustomplot.h"
#include <QMap>
#include "commondefine.h"
#include <map>
#include "calcclosestpoint.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <algorithm>
#include <functional>
#include <QCheckBox>

struct POINTVECTOR
{
    QVector<double> x;//线的x轴坐标
    QVector<double> y;//线的y轴坐标
    QColor color;//线的颜色
    void * graph; //线的指针对象
    bool showScatter;//线上是否显示点
    POINTVECTOR()
    {
        x.clear();
        y.clear();
        color = Qt::black;
        graph = nullptr;
        showScatter = true;
    }
};

typedef QMap<QString,POINTVECTOR> CURVENAME2DATAMAP;

class myChart : public QWidget
{
    Q_OBJECT
public:
    explicit myChart(std::function<void()> callback,std::function<void()> callbackRefresh,QWidget *parent = 0);
    ~myChart();
    void refresh(bool reset = false);

    //设置量程
    void setRange(RANGE_SET range);

    //获取量程
    RANGE_SET getRange();

    void setB1(double b1);
    void getPicture(QPixmap &pixmap);
    void addCheckBox(QCheckBox * checkBox, const QColor &color);
public slots:
    void onSelectionChanged();
    void onMousePressEvent(QMouseEvent *event);
    void onMouseReleaseEvent(QMouseEvent *);
    void onMouseMoveEvent(QMouseEvent * event);
    void onCheckBoxClick(bool);
private:
    //曲线默认颜色
    QVector<QColor>                     _defaultColors;
    //图像控件
    QCustomPlot *                       _mainPlot;
    //布局控件
    QHBoxLayout *                       _mainLayout;
    QVBoxLayout *                       _checkBoxLayout;
    //数据回调 用于修改点后通知表格
    std::function<void()>               _updateCallback;
    //界面重置回调
    std::function<void()>               _callbackRefresh;
    //选择框 用于隐藏显示曲线
    QVector<QCheckBox *>                _checkBoxes;
    //当前选中的曲线
    QCPGraph *                          _selectedGraph;
    //当前选中的曲线选中点的索引
    int                                 _selectedIndex;
    //移动曲线后，标记原先位置的曲线用虚线
    bool                                _bSllRedraw;
    QVector<double>                     _oldSllKey;
    QVector<double>                     _oldSllValue;
};

#endif // MYCHART_H
