#ifndef MYJOB_H
#define MYJOB_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QApplication>

#include </opt/include/mediaclient.h>
//#include <iostream>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep


class FM_rds : public QObject
      //class FM_rds : public QThread m2
{
    Q_OBJECT

private:


    int workSpeed;
    int forsize;
    int fd;




    QMutex lock;


public:
    explicit FM_rds(QObject *parent = nullptr);

    //void start(QString name);

    //int rds(const char *device);

    //void run(); //m2
    bool mStop_rds;

    QString prog_chars;
    QString rds_chars;

signals:

    //void on_number(QString name, int number);

    void rds_out(QString rds_stream);
    void rds_prog_out(QString rds_prog);
    void finished_rds_reading();

public slots:

    int rds();

    //void doWork();

    void start_rds_reading(); //m2
    void stop_rds_reading(); //m2

    void receiveWorkSpeed(int _workSpeed); //m2
    void receiveForSize(int _forsize); //m2


};

#endif // MYJOB_H
