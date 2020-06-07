#include "dialog.h"
//#include "tune.h"
#include <QApplication>
#include <QThread> //new
#include <globals.h> //new

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QThread::currentThread()->setPriority(QThread::HighestPriority);

    Dialog w;



    //w.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    w.show();



    return a.exec();
}
