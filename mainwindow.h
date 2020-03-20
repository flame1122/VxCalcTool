#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMenuBar>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QGroupBox>
#include "commondefine.h"
#include "mychart.h"
#include "qacalcdialog.h"
#include "qoriginalcurveconvert.h"
#include "mytableview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent * event);

protected:
    void _initToolBar();
    void _initMenu();
    void _initCenterArea();
    void _initFormAppearance();
private:
    void _createNewProject();
    void _openProject();
    void _closeSoftware();
    QString _openPointCollection();
    void _onlineHelp();
    void _about();
    void _openProcess(const QString & fileName);
    void _importOriginalPoints(const QString &fileName);
    void _flowMesusrementCalc();
    void _antisurgeValueSizingCalc();
    void _enableToolBar(bool enable);
    void _loadParam();
    void _addCustomCurve();
    void _addCustomPoints();
    void _refreshWorkCondition(const QString& fileName = "");
private:
    /////////////////////////////////
    /// Control Area
    /////////////////////////////////
    QToolBar* _projectToolBar;
    QToolBar* _functionToolBar;
    QToolBar* _toolToolBar;
    QAction * _newProjectAction;
    QAction * _openProjectAction;
    QAction * _closeSoftwareAction;
    QAction * _pointCollectionAction;
    QAction * _equationEditorAction;
    QAction * _openCalcAction;
    QAction * _aCalcAction;
    QAction * _flowMeasurementAction;
    QAction * _antisurgeValueSizingCalcAction;
    QAction * _originalCurveConvertAction;
    //QAction * _originalPointsImportAction;
    QAction * _setChartRangeAction;

    QAction * _saveAction;
    QAction * _generateAction;
    //QAction * _customCurveAction;
    QAction * _customPointsAction;
    QAction * _graphFitting;

    QMenu * _customFormulaMenu;

    QAction * _onlineHelpAction;
    QAction * _aboutAction;

    QACalcDialog * _aCalcDlg;
    QOriginalCurveConvertDlg * _orignalCurveDlg;

    QLabel  * _statusBarLabel;
    QSplitter* _mainSplitter;
    QSplitter * _sidebarSplitter;
    myTableView * _myTableView;

    QVector<QGroupBox*> _groupBox;
    QVector<QLabel*> _xLabel;
    QVector<QLabel*> _yLabel;
    QVector<QLineEdit*> _xLineEdit;
    QVector<QLineEdit*> _yLineEdit;

    QComboBox *    _workConditionCombox;

    /////////////////////////////////
    /// Variable Area
    /////////////////////////////////
    QString                             _currentProject;

    bool                                _needSave;
    CurveName2CurveDataMap              _curveData;

    //double  _ps;
    //double  _ts;
    double  _density;
    double  _mw;
    double  _zs;
    double  _fmdA;
};

#endif // MAINWINDOW_H
