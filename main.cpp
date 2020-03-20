#include "mainwindow.h"
#include <QApplication>
#include <QLibrary>
#include <QMessageBox>
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

typedef int (__stdcall *CheckDogSecrityEx)( char* );


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 
    MainWindow w;
    w.show();

    return a.exec();
}
