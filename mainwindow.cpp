#include "mainwindow.h"
#include <QIcon>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QTimer>
#include "qrangsetdialog.h"
#include "qnewprojectdialog.h"
#include "pointcollectordlg.h"
#include "qflowcalcdialog.h"
#include "antisurgevaluesizingcalcdialog.h"
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "xlsxformat.h"
#include "cdatacore.h"
#include "qcustompointsdialog.h"
#include "qcustomcurvesdialog.h"
#include <QFileInfo>
#include "qgraphfittingdlg.h"
#include <qwindow.h>
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

using namespace QXlsx;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,_aCalcDlg(nullptr)
    ,_orignalCurveDlg(nullptr)
    ,_currentProject("")
    ,_needSave(false)
    ,_density(-1)
    ,_mw(-1)
    ,_zs(-1)
    ,_fmdA(-1)
{
    _initMenu();
    _initToolBar();
    _initCenterArea();
    _initFormAppearance();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(_needSave)
    {
        auto returnButton = QMessageBox::question(this,tr("提示"),tr("检测到组态尚未保存，是否继续关闭？"),
                             tr("确认"),tr("取消"));
        if(returnButton != 0)
        {
            event->ignore();
        }
        else
        {
            event->accept();
        }
    }
}

void MainWindow::_initToolBar()
{
    _projectToolBar = addToolBar(tr("文件"));

    _projectToolBar->addAction(_newProjectAction);
    _projectToolBar->addAction(_openProjectAction);

    _functionToolBar = addToolBar(tr("功能"));

    _functionToolBar->addAction(_saveAction);
    _functionToolBar->addAction(_aCalcAction);
    _functionToolBar->addAction(_pointCollectionAction);
    _functionToolBar->addAction(_setChartRangeAction);
    //_functionToolBar->addAction(_customCurveAction);
    _functionToolBar->addAction(_customPointsAction);
    _functionToolBar->addAction(_generateAction);
    _functionToolBar->addAction(_graphFitting);
    _functionToolBar->addAction(_openCalcAction);
    _functionToolBar->addAction(_flowMeasurementAction);
    _functionToolBar->addAction(_antisurgeValueSizingCalcAction);
    //_functionToolBar->addSeparator();
    //_functionToolBar->addAction(_onlineHelpAction);

    _workConditionCombox = new QComboBox;
    _workConditionCombox->setMinimumWidth(150);
    connect(_workConditionCombox,&QComboBox::currentTextChanged,[=](){
        if(!CDataCore::instance()->load(_currentProject,_workConditionCombox->currentText()))
        {
            _importOriginalPoints(_workConditionCombox->currentText());
        }
        else
        {
            _myTableView->refreshWithoutCalc();
        }
    });
    _functionToolBar->addWidget(new QLabel("   "));
    _functionToolBar->addWidget(new QLabel(tr("工况：")));
    _functionToolBar->addWidget(_workConditionCombox);
}

void MainWindow::_initMenu()
{
    auto menu = menuBar();
    auto fileMenu = menu->addMenu(tr("文件"));

    _newProjectAction = fileMenu->addAction(QIcon(":img/new.ico"),tr("新建工程"));
    connect(_newProjectAction,&QAction::triggered,[=](){
        _createNewProject();
    });

    _openProjectAction = fileMenu->addAction(QIcon(":img/open.ico"),tr("打开工程"));
    connect(_openProjectAction,&QAction::triggered,[=](){
        _openProject();
    });

    _closeSoftwareAction = fileMenu->addAction(QIcon(":img/exit.ico"),tr("退出"));
    connect(_closeSoftwareAction,&QAction::triggered,[=](){
        _closeSoftware();
    });

    auto functionMenu = menu->addMenu("功能");

    _saveAction = functionMenu->addAction(QIcon(":img/Save.ico"),tr("保存"));
    connect(_saveAction,&QAction::triggered,[=](){
        QPixmap pixmap;
        _myTableView->getChartPtr()->getPicture(pixmap);
        CDataCore::instance()->save(_currentProject,_workConditionCombox->currentText(),pixmap);
        QMessageBox::information(this,"提示","保存成功");
    });

    _aCalcAction = functionMenu->addAction(QIcon(":img/calculator_a.ico"),tr("流量系数(A)计算"));
    connect(_aCalcAction,&QAction::triggered,[=](){
        if(_currentProject.isEmpty())
        {
            QMessageBox::warning(this,"警告","请新建或打开组态");
            return;
        }

        QACalcDialog _aCalcDlg(_currentProject);
        _aCalcDlg.exec();
        _mw = _aCalcDlg.getValue("Mw");
        _zs = _aCalcDlg.getValue("Zs");
        _fmdA = _aCalcDlg.getValue("A");
        _density = _aCalcDlg.getValue("StdDensity");
        //_ps = _aCalcDlg.getValue("Ps") * 0.001;
        //_ts = _aCalcDlg.getValue("Ts");
        _myTableView->update(_mw,_zs,_fmdA);
    });

    _pointCollectionAction = functionMenu->addAction(QIcon(":img/crosshair.ico"),tr("图片取点"));
    connect(_pointCollectionAction,&QAction::triggered,[=](){
        QString fileName = _openPointCollection();
        _importOriginalPoints(fileName);
        _refreshWorkCondition(fileName);
    });

    _setChartRangeAction = functionMenu->addAction(QIcon(":img/chart.ico"),tr("设置量程"));
    connect(_setChartRangeAction,&QAction::triggered,[=](){
        QRangSetDialog dlg(_myTableView->getChartPtr()->getRange());
        dlg.exec();
        _myTableView->getChartPtr()->setRange( dlg.getRange() );
    });

//    _customCurveAction = functionMenu->addAction(QIcon(":img/custom_curve.ico"),tr("自定义曲线"));
//    connect(_customCurveAction,&QAction::triggered,[=](){
//        _addCustomCurve();
//    });

    _customPointsAction = functionMenu->addAction(QIcon(":img/dots.ico"),tr("自定义工作点"));
    connect(_customPointsAction,&QAction::triggered,[=](){
        _addCustomPoints();
    });


    _generateAction = functionMenu->addAction(QIcon(":img/reset.ico"),tr("重置曲线"));
    connect(_generateAction,&QAction::triggered,[=](){
        _myTableView->refresh();
    });

    _graphFitting = functionMenu->addAction(QIcon(":img/graphFitting.ico"),tr("曲线拟合"));
    connect(_graphFitting,&QAction::triggered,[=](){
        QGraphFittingDlg dlg(_currentProject);
        dlg.exec();
    });

    _openCalcAction = functionMenu->addAction(QIcon(":img/calculator.ico"),tr("计算器"));
    connect(_openCalcAction,&QAction::triggered,[=](){
        _openProcess(QString("calc.exe"));
    });

    _flowMeasurementAction = functionMenu->addAction(QIcon(":img/celiu.ico"),tr("测流计算器"));
    connect(_flowMeasurementAction,&QAction::triggered,[=](){
        _flowMesusrementCalc();
    });

    _antisurgeValueSizingCalcAction = functionMenu->addAction(QIcon(":img/calc.ico"),tr("防喘阀流通能力计算器"));
    connect(_antisurgeValueSizingCalcAction,&QAction::triggered,[=]{
        _antisurgeValueSizingCalc();
    });



    auto helpMenu = menu->addMenu(tr("帮助"));

//    _onlineHelpAction = helpMenu->addAction(QIcon(":img/help.ico"),tr("帮助"));
//    connect(_onlineHelpAction,&QAction::triggered,[&](){
//        _onlineHelp();
//    });

    _aboutAction = helpMenu->addAction(QIcon(":img/about.ico"),tr("关于..."));
    connect(_aboutAction,&QAction::triggered,[&](){
        _about();
    });

    _enableToolBar(false);
}

void MainWindow::_initCenterArea()
{
    _mainSplitter = new QSplitter(Qt::Horizontal);

    _myTableView = new myTableView;
    _myTableView->setMinimumSize(1000,600);
    _mainSplitter->addWidget(_myTableView);

    setCentralWidget(_mainSplitter);
}

void MainWindow::_initFormAppearance()
{
    setWindowIcon(QIcon(":img/trends.ico"));
    setWindowTitle(tr("机组计算工具"));
}

void MainWindow::_createNewProject()
{
    QNewProjectDialog dlg;
    dlg.exec();
    if(dlg.configPath().isEmpty())
    {
        return;
    }
    _currentProject = dlg.configPath();
    if(_currentProject.isEmpty())
        return;
    QString title = tr("机组计算工具");
    title.append("——");
    title.append(_currentProject);
    setWindowTitle(title);
    _enableToolBar(true);
    CDataCore::instance()->clearRawData();
    CDataCore::instance()->resetParam();
    _refreshWorkCondition();
    _myTableView->refreshWithoutCalc();
}

void MainWindow::_openProject()
{   
    auto fileName = QFileDialog::getOpenFileName(this,
          tr("打开工程"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
                                            , tr("Project Files (*.cPrj)"));

    if(fileName.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(fileName);

    QString path = fileInfo.absolutePath();
    if(!path.isEmpty())
    {
        _currentProject = path + "/";

        QString title = tr("机组计算工具");
        title.append("——");
        title.append(_currentProject);
        setWindowTitle(title);
        _enableToolBar(true);
        _loadParam();
        _refreshWorkCondition();
        CDataCore::instance()->load(_currentProject,_workConditionCombox->currentText());
        _myTableView->refreshWithoutCalc();
    }
}

void MainWindow::_closeSoftware()
{
    qApp->exit();
}

QString MainWindow::_openPointCollection()
{
    if(_currentProject.isEmpty())
    {
        QMessageBox::warning(this,"警告","请新建或打开组态");
        return "";
    }

    QString filePath = _currentProject;
    filePath.append("/A.xls");
    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists())
    {
        QMessageBox::warning(this,"警告","请先计算流量系数A后再进行取点");
        return "";
    }

    PointCollectorDlg dlg;
    dlg.init(_mw,_zs,_currentProject,_workConditionCombox->currentText());
    dlg.exec();

    return dlg.getFileName();
}

void MainWindow::_onlineHelp()
{

}

void MainWindow::_about()
{
    QMessageBox::about(this,tr("关于机组计算工具"),tr("VxCalcTool V1.00.00.00-C"));
}

void MainWindow::_openProcess(const QString &fileName)
{
    QProcess process;
    process.startDetached(fileName);
    process.waitForFinished();
}

void MainWindow::_importOriginalPoints(const QString &fileName)
{
    QString filePath = _currentProject + fileName + ".rd";

    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists())
    {
        return;
    }

    QXlsx::Document xlsx(filePath);
    if(!xlsx.isLoadPackage())
    {
        QMessageBox::warning(this,"错误","导入失败");
        return;
    }

    auto pointCount = xlsx.read(10,2).toInt();
    auto curvePointCount = xlsx.read(11,2).toInt();

    CDataCore::instance()->clearRawData();

    for(int i = 0 ; i < curvePointCount;i++)
    {
        SSL_DATA data;
        data.W = xlsx.read(i+14,6).toDouble();
        data.Pd = xlsx.read(i+14,7).toDouble();
        data.Ps = xlsx.read(i+14,8).toDouble();
        data.Ts = xlsx.read(i+14,9).toDouble();
        data.Point = xlsx.read(i+14,1).toString();
        data.kavg = 1;
        data.eff =  1;
        CDataCore::instance()->addRawCurvePoints(data);
    }

    for(int i = 0 ; i < pointCount;i++)
    {
        SSL_DATA data;
        data.W = xlsx.read(i+2,15).toDouble();
        data.Pd = xlsx.read(i+2,16).toDouble();
        data.Ps = xlsx.read(i+2,17).toDouble();
        data.Ts = xlsx.read(i+2,18).toDouble();
        data.Point = xlsx.read(i+2,10).toString();
        data.kavg = 1;
        data.eff =  1;
        CDataCore::instance()->addRawPoints(data);
    }

    _myTableView->refresh();
}

void MainWindow::_flowMesusrementCalc()
{
    QFlowCalcDialog dlg;
    dlg.exec();
}

void MainWindow::_antisurgeValueSizingCalc()
{
    AntisurgeValueSizingCalcDialog dlg;
    dlg.exec();
}

void MainWindow::_enableToolBar(bool enable)
{
    _newProjectAction->setEnabled(true);
    _openProjectAction->setEnabled(true);
    _closeSoftwareAction->setEnabled(true);

    _pointCollectionAction->setEnabled(enable);
    _setChartRangeAction->setEnabled(enable);
    _saveAction->setEnabled(enable);
    _generateAction->setEnabled(enable);
    //_customCurveAction->setEnabled(enable);
    _customPointsAction->setEnabled(enable);
    _graphFitting->setEnabled(enable);

    _openCalcAction->setEnabled(true);
    _aCalcAction->setEnabled(enable);
    _flowMeasurementAction->setEnabled(true);
    _antisurgeValueSizingCalcAction->setEnabled(true);

    //_onlineHelpAction->setEnabled(true);
    _aboutAction->setEnabled(true);
}

void MainWindow::_loadParam()
{
    QString filePath = _currentProject;
    filePath.append("A.xls");

    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists())
    {
        return;
    }

    QXlsx::Document xlsx(filePath);
    if(!xlsx.isLoadPackage())
    {
        return;
    }

    _mw = xlsx.read(8,2).toDouble();
    _zs = xlsx.read(7,2).toDouble();
    _fmdA = xlsx.read(33,2).toDouble();
    _myTableView->update(_mw,_zs,_fmdA);
}

void MainWindow::_addCustomCurve()
{
    QCustomCurvesDialog dlg(_currentProject);
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }
    _myTableView->addCustomCurve(dlg.getValue());
}

void MainWindow::_addCustomPoints()
{
    QCustomPointsDialog dlg;
    if(dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    CUSTOMPOINT point = dlg.getValue();

    _myTableView->addCustomPoint(point);
}

void MainWindow::_refreshWorkCondition(const QString& fileName)
{
    _workConditionCombox->blockSignals(true);
    _workConditionCombox->clear();
    QDir dir(_currentProject);
    QStringList nameFilters;
    nameFilters<<"*.rd";
    QStringList files = dir.entryList(nameFilters,QDir::Files|QDir::Readable,QDir::Name);
    for(int i =0;i < files.size();i++)
    {
        QFileInfo fileInfo(files.at(i));
        _workConditionCombox->addItem(fileInfo.baseName());
    }
    if(!fileName.isEmpty())
    {
        _workConditionCombox->setCurrentText(fileName);
    }
    _workConditionCombox->blockSignals(false);
}
