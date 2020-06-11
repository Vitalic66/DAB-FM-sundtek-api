#ifndef MYJOB_H
#define MYJOB_H

#include <QObject>
#include <QDebug>
#include <QThread>
//#include <QMutex>
#include <QApplication>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
#include <charsets.h>


class FM_rds : public QObject
      //class FM_rds : public QThread m2
{
    Q_OBJECT

private:

    int fd;
    int rdsfd;
    bool mStop_rds;
    QString prog_chars;
    QString rds_chars;
    QString qprog_name;
    QString qrds_text;

    std::vector<uint8_t> c_prog_chars;
    std::vector<uint8_t> c_text_chars;

    int rds();

    //QMutex lock;


public:
    explicit FM_rds(QObject *parent = nullptr);

signals:

    void rds_out(QString rds_stream);
    void rds_prog_out(QString rds_prog);
    void finished_rds_reading();
    //void clear_lbl(QString);

public slots:

    void start_rds_reading();
    void stop_rds_reading();
};

#endif // MYJOB_H
