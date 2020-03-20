#include "mychart.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QtGlobal>
#include <QSharedPointer>
#include <QDebug>
#include <QtCore/QtMath>
#include "cdatacore.h"
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

myChart::myChart(std::function<void()> callback, std::function<void()> callbackRefresh, QWidget *parent)
    : QWidget(parent)
    , _updateCallback(callback)
    , _callbackRefresh(callbackRefresh)
    , _selectedGraph(nullptr)
    , _selectedIndex(-1)
    , _bSllRedraw(false)
{
    for(int i=0;i<50;i++)
    {
        QColor qc = QColor::fromHsl(qrand()%360,qrand()%256,qrand()%200);
        _defaultColors.push_back(qc);
    }

    _mainPlot = new QCustomPlot;
    _mainPlot->legend->setVisible(true);
    _mainPlot->setInteractions(QCP::iRangeZoom|QCP::iMultiSelect|
                               QCP::iSelectPlottables);
    _mainPlot->xAxis->setLabel("qr2");
    _mainPlot->yAxis->setLabel("RC");
    _mainPlot->setMinimumHeight(400);


    CUSTOMRANGE range = CDataCore::instance()->range();
    _mainPlot->xAxis->setRange(range.xL,range.xH);
    _mainPlot->yAxis->setRange(range.yL,range.yH);

    connect(_mainPlot,SIGNAL(selectionChangedByUser()),this,SLOT(onSelectionChanged()));
    connect(_mainPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(onMousePressEvent(QMouseEvent*)));
    connect(_mainPlot,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(onMouseReleaseEvent(QMouseEvent*)));
    connect(_mainPlot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(onMouseMoveEvent(QMouseEvent*)));

    _mainLayout = new QHBoxLayout;
    _mainLayout->addWidget(_mainPlot);
    _checkBoxLayout  = new QVBoxLayout;
    _mainLayout->addLayout(_checkBoxLayout);

    setLayout(_mainLayout);
}

myChart::~myChart()
{

}

void myChart::refresh(bool reset)
{
    //曲线选择框
    if(!reset)
    {
        for(auto i = 0; i < _checkBoxes.size();i++)
        {
            if(_checkBoxes[i])
            {
                delete _checkBoxes[i];
                _checkBoxes[i] = nullptr;
            }
        }
        _checkBoxes.clear();
        _bSllRedraw = false;
        _selectedGraph = nullptr;
        _selectedIndex = -1;
    }

    //曲线绘制开始
    _mainPlot->clearGraphs();

    CUSTOMRANGE range = CDataCore::instance()->range();
    _mainPlot->xAxis->setRange(range.xL,range.xH);
    _mainPlot->yAxis->setRange(range.yL,range.yH);

    QPen pen(Qt::red);
    pen.setWidth(2);

    //性能曲线绘制
    auto curveData = CDataCore::instance()->rawCurvePoints();

    QCPGraph * sllGraph = nullptr;
    QCPGraph * sclGraph = nullptr;
    QVector<double> sllDataX,sllDataY;
    QVector<double> sclDataX,sclDataY;
    if(curveData.size() > 0)
    {
        //SLL线
        sllGraph = _mainPlot->addGraph();
        sllGraph->setName(tr("SLL"));
        sllGraph->setPen(pen);
        sllDataX.push_back(0);
        sllDataY.push_back(1);
        sllGraph->setSelectable(QCP::stWhole);
        if(!reset)
            addCheckBox(new QCheckBox(tr("SLL")),QColor(Qt::red));

        //SCL线
        sclGraph = _mainPlot->addGraph();
        sclGraph->setName(tr("SCL"));
        pen.setColor(QColor(218,165,32));
        sclGraph->setPen(pen);
        sclDataX.push_back(0);
        sclDataY.push_back(1);
        sclGraph->setSelectable(QCP::stNone);
        if(!reset)
            addCheckBox(new QCheckBox(tr("SCL")),QColor(218,165,32));
    }

    int index = 0;
    for(auto it = curveData.begin();it != curveData.end();++it,index++)
    {
        qDebug()<<"curveData="<<it.key();
        //新增曲线
        auto newGraph = _mainPlot->addGraph();
        newGraph->setName(it.key());
        pen.setColor(_defaultColors[index]);
        newGraph->setPen(pen);
        newGraph->setSelectable(QCP::stNone);

        //新增checkbox
        if(!reset)
            addCheckBox(new QCheckBox(it.key()),_defaultColors[index]);

        QVector<double> graphDataX,graphDataY;
        for(int i=0;i<it.value().size();i++)
        {
            graphDataX.push_back(it.value()[i].qr2_new);
            graphDataY.push_back(it.value()[i].Rc_new);
            if(i == 0)
            {
                if(sllGraph)
                {
                    sllDataX.push_back(it.value()[i].qr2_new);
                    sllDataY.push_back(it.value()[i].Rc_new);
                }
                if(sclGraph)
                {
                    sclDataX.push_back(it.value()[i].SCL_qr2);
                    sclDataY.push_back(it.value()[i].Rc_new);
                }
            }
        }
        if(graphDataX.size() > 0 && graphDataY.size() > 0)
        {
            newGraph->addData(graphDataX,graphDataY,true);
        }
    }

    if(sllGraph)
    {
        sllGraph->addData(sllDataX,sllDataY,true);
    }

    if(sclGraph)
    {
        sclGraph->addData(sclDataX,sclDataY,true);
    }
    //曲线绘制结束

    //工作点绘制开始
    auto workPoints = CDataCore::instance()->rawPoints();
    if(workPoints.size() > 0)
    {
        auto pointsGraph = _mainPlot->addGraph();
        pointsGraph->setName(tr("Point"));
        pen.setColor(Qt::green);
        pointsGraph->setPen(pen);
        pointsGraph->setSelectable(QCP::stNone);
        pointsGraph->setLineStyle(QCPGraph::lsNone);
        pointsGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross,5));

        if(!reset)
            addCheckBox(new QCheckBox(tr("Point")),QColor(Qt::green));


        for(int j=0;j<workPoints.size();j++)
        {
            pointsGraph->addData(workPoints[j].qr2_new,workPoints[j].Rc_new);
        }
    }
    //工作点绘制结束

    //控件重绘  reset为true 则在外面管理控件重绘动作
    if(!reset)
        _mainPlot->replot();
}

void myChart::setRange(RANGE_SET range)
{
    _mainPlot->xAxis->setRange(range.XL,range.XH);
    _mainPlot->yAxis->setRange(range.YL,range.YH);

    CUSTOMRANGE rg;
    rg.xH = range.XH;
    rg.xL = range.XL;
    rg.yH = range.YH;
    rg.yL = range.YL;

    CDataCore::instance()->setRange(rg);

    _mainPlot->replot();
}

RANGE_SET myChart::getRange()
{
    RANGE_SET range;
    range.XL = _mainPlot->xAxis->range().lower;
    range.XH = _mainPlot->xAxis->range().upper;
    range.YH = _mainPlot->yAxis->range().upper;
    range.YL = _mainPlot->yAxis->range().lower;
    return range;
}

void myChart::getPicture(QPixmap &pixmap)
{
    pixmap = _mainPlot->toPixmap();
}

void myChart::addCheckBox(QCheckBox *checkBox,const QColor& color)
{
    _checkBoxes.push_back(checkBox);
    auto palette = checkBox->palette();
    palette.setColor(QPalette::WindowText,QColor(color));
    checkBox->setPalette(palette);
    checkBox->setMaximumWidth(60);
    checkBox->setChecked(true);
    checkBox->setToolTip(checkBox->text());
    connect(checkBox,SIGNAL(clicked(bool)),this,SLOT(onCheckBoxClick(bool)));
    _checkBoxLayout->addWidget(checkBox);
}

void myChart::onSelectionChanged()
{
    bool haveSelected = false;
    for(int i = 0 ; i < _mainPlot->selectedGraphs().size();i++)
    {
        haveSelected = true;
        _selectedGraph = _mainPlot->selectedGraphs().at(i);
        _selectedGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle,10));
    }

    if(haveSelected == false)
    {
        if(_selectedGraph)
        {
            _selectedGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
            _selectedGraph = nullptr;
        }
    }
}

void myChart::onMousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
    {
        return;
    }

    if(_selectedGraph == nullptr)
    {
        return;
    }

    QVariant var;
    _selectedGraph->selectTest(event->pos(),true,&var);

    if (var.value<QCPDataSelection>().isEmpty())
    {
        return;
    }

    int selectedIndex = var.value<QCPDataSelection>().dataRange().begin();

    _selectedIndex = selectedIndex;
}

void myChart::onMouseReleaseEvent(QMouseEvent*)
{
    _selectedIndex = -1;
}

void myChart::onMouseMoveEvent(QMouseEvent *event)
{
    if(_selectedGraph == nullptr)
    {
        return;
    }

    if(_selectedIndex <= 0)
    {
        return;
    }

    //拷贝SLL线数据到QVector中
    if(_bSllRedraw == false)
    {
        _oldSllKey.clear();
        _oldSllValue.clear();
        for(int i = 0;i < _selectedGraph->dataCount();i++)
        {
            _oldSllKey.push_back(_selectedGraph->data().data()->at(i)->key);
            _oldSllValue.push_back(_selectedGraph->data().data()->at(i)->value);
        }
        addCheckBox(new QCheckBox(tr("SLL Old")),QColor(Qt::red));
        _bSllRedraw = true;
    }

    double xVal = _mainPlot->xAxis->pixelToCoord(event->pos().x());
    double yVal = _mainPlot->yAxis->pixelToCoord(event->pos().y());

    CDataCore::instance()->setData(xVal,yVal,_selectedIndex);

    //刷新sll scl线
    QCPGraph * sllGraph = nullptr;
    QCPGraph * sclGraph = nullptr;
    QVector<double> sllDataX,sllDataY;
    QVector<double> sclDataX,sclDataY;
    for(int i = 0 ; i < _mainPlot->graphCount();i++)
    {
        if(_mainPlot->graph(i)->name().compare("SLL") == 0)
        {
            sllGraph = _mainPlot->graph(i);
            sllDataX.push_back(0);
            sllDataY.push_back(1);
        }
        if(_mainPlot->graph(i)->name().compare("SCL") == 0)
        {
            sclGraph = _mainPlot->graph(i);
            sclDataX.push_back(0);
            sclDataY.push_back(1);
        }
    }

    int index = 0;
    auto curveData = CDataCore::instance()->rawCurvePoints();
    for(auto it = curveData.begin();it != curveData.end();++it,index++)
    {
        for(int i=0;i < it.value().size();i++)
        {
            if(i == 0)
            {
                if(sllGraph)
                {
                    sllDataX.push_back(it.value()[i].qr2_new);
                    sllDataY.push_back(it.value()[i].Rc_new);
                }
                if(sclGraph)
                {
                    sclDataX.push_back(it.value()[i].SCL_qr2);
                    sclDataY.push_back(it.value()[i].Rc_new);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(sllGraph)
    {
        sllGraph->setData(sllDataX,sllDataY,true);
    }

    if(sclGraph)
    {
        sclGraph->setData(sclDataX,sclDataY,true);
    }
    //刷新scl sll线结束

    //增加虚线显示原始SLL线
    QCPGraph* oldSLLGraph = nullptr;
    for(int i = 0 ; i < _mainPlot->graphCount();i++)
    {
        if(_mainPlot->graph(i)->name().compare("SLL Old") == 0)
        {
            oldSLLGraph = _mainPlot->graph(i);
            break;
        }
    }
    if(oldSLLGraph == nullptr)
    {
        oldSLLGraph = _mainPlot->addGraph();
        oldSLLGraph->setName(tr("SLL Old"));
        QPen pen(Qt::red);
        pen.setStyle(Qt::DashLine);
        pen.setWidth(2);
        oldSLLGraph->setPen(pen);
        oldSLLGraph->setLineStyle(QCPGraph::lsLine);
        oldSLLGraph->setSelectable(QCP::stNone);
        oldSLLGraph->setData(_oldSllKey,_oldSllValue,true);
    }

    _mainPlot->replot();

    _updateCallback();
}

void myChart::onCheckBoxClick(bool)
{
    for(int i = 0 ; i < _mainPlot->graphCount();i++)
    {
        if(_checkBoxes[i]->isChecked())
        {
            _mainPlot->graph(i)->setVisible(true);
        }
        else
        {
            _mainPlot->graph(i)->setVisible(false);
        }
    }
    _mainPlot->replot();
}
