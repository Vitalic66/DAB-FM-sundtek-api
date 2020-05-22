#ifndef MYJOB_H
#define MYJOB_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include </opt/include/mediaclient.h>
//#include <iostream>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep

class FM_rds : public QObject
{
    Q_OBJECT
public:
    explicit FM_rds(QObject *parent = nullptr);

    //void start(QString name);

    //int rds(const char *device);


    bool mStop_rds;


signals:

    //void on_number(QString name, int number);

    void rds_out(QString rds_stream);
    void rds_prog_out(QString rds_prog);
    void finished_rds_reading();

public slots:

    int rds();
    void stop_rds_reading();

private:


    int fd;
};

#endif // MYJOB_H
