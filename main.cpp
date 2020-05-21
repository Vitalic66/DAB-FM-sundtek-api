#include "dialog.h"
//#include "tune.h"
#include <QApplication>
#include <QThread> //new
#include <scan.h> //new

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

//    QThread cThread;
//    Scan cObject;

//    cObject.DoSetup(cThread);
//    cObject.moveToThread(&cThread);

//    cThread.start();

    return a.exec();
}
