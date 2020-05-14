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
    int rds();


signals:

    //void on_number(QString name, int number);

    void rds_out(QString radio_program);

public slots:
    void stop();

private:

    bool mStop;
    int fd;
};

#endif // MYJOB_H
