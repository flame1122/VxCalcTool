#-------------------------------------------------
#
# Project created by QtCreator 2018-07-17T08:38:21
#
#-------------------------------------------------
QT       += core gui

RC_ICONS = trends.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets  printsupport axcontainer

TARGET = VxCalcTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
        mainwindow.cpp \
    mychart.cpp \
    qcustomplot.cpp \
    qacalcdialog.cpp \
    qoriginalcurveconvert.cpp \
    mytableview.cpp \
    qrangsetdialog.cpp \
    qnewprojectdialog.cpp \
    pointcollectordlg.cpp \
    coordinatesetting.cpp \
    decimalsetting.cpp \
    paintarea.cpp \
    qpointcollectortableview.cpp \
    cdatacore.cpp \
    qflowcalcdialog.cpp \
    creportexportimpl.cpp \
    antisurgevaluesizingcalcdialog.cpp \
    qcustomformuladlg.cpp \
    qpointdatacore.cpp \
    qcustompointsdialog.cpp \
    qcustomcurvesdialog.cpp \
    qcreatesubitem.cpp \
    qtl/sqlite3.c \
    qgraphfittingdlg.cpp \
    qaddcustomlinedialog.cpp

HEADERS  += mainwindow.h \
    commondefine.h \
    mychart.h \
    qcustomplot.h \
    qacalcdialog.h \
    qoriginalcurveconvert.h \
    mytableview.h \
    qrangsetdialog.h \
    calcclosestpoint.h \
    qnewprojectdialog.h \
    pointcollectordlg.h \
    coordinatesetting.h \
    decimalsetting.h \
    paintarea.h \
    qpointcollectortableview.h \
    cdatacore.h \
    qflowcalcdialog.h \
    creportexportimpl.h \
    antisurgevaluesizingcalcdialog.h \
    qcustomformuladlg.h \
    qpointdatacore.h \
    qcustompointsdialog.h \
    qcustomcurvesdialog.h \
    qcreatesubitem.h \
    qtl/apply_tuple.h \
    qtl/qtl_common.hpp \
    qtl/qtl_database_pool.hpp \
    qtl/qtl_mysql.hpp \
    qtl/qtl_mysql_pool.hpp \
    qtl/qtl_odbc.hpp \
    qtl/qtl_odbc_pool.hpp \
    qtl/qtl_sqlite.hpp \
    qtl/qtl_sqlite_pool.hpp \
    qtl/sqlite3.h \
    qgraphfittingdlg.h \
    qaddcustomlinedialog.h

RESOURCES += \
    res.qrc

FORMS += \
    qacalcdialog.ui \
    qoriginalcurveconvert.ui \
    qrangsetdialog.ui \
    qnewprojectdialog.ui \
    pointcollectordlg.ui \
    qflowcalcdialog.ui \
    antisurgevaluesizingcalcdialog.ui \
    qcustomformuladlg.ui \
    qcustompointsdialog.ui \
    qcustomcurvesdialog.ui \
    qcreatesubitem.ui \
    qgraphfittingdlg.ui \
    qaddcustomlinedialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/QXlsx/lib/ -lQXlsx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/QXlsx/lib/ -lQXlsx
else:unix: LIBS += -L$$PWD/QXlsx/lib/ -lQXlsx

LIBS+= -L$$PWD

INCLUDEPATH += $$PWD/QXlsx/header
DEPENDPATH += $$PWD/QXlsx

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/QXlsx/lib/libQXlsx.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/QXlsx/lib/libQXlsx.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/QXlsx/lib/QXlsx.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/QXlsx/lib/QXlsx.lib
else:unix: PRE_TARGETDEPS += $$PWD/QXlsx/lib/libQXlsx.a

DISTFILES += \
    sqlite3.def

win32:MOC_DIR=./temp

DESTDIR=../../out/Release/VxCalcTool
