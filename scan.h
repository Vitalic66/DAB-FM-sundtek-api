#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QVector>
#include <QThread>
//#include <QStringList>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
#include <tune.h>
#include <globals.h>

class Scan : public QObject
{
    Q_OBJECT
public:
    explicit Scan(QObject *parent = nullptr);    

    int media_scan_dabfrequencies(char *device, int devfd, int console, int running);

    int media_scan_dabservices(char *device);

    int dab_scan_wrapped();
    QVector<QVector<QString>> dab_vec_vec;
    QVector<QString> dab_vec;
    bool mStop_dab_scan;

signals:

    void progress_scan_dab(int prog_bar_dab);
    void enable_buttons(bool btn_st);
    void write_to_file();
    void finished_scan();

public slots:

    void stop_scan();

private:
    //QVector<QVector<uint32_t>> dab_vec_vec;

    //QVector<uint32_t> dab_sid_vec;
    QVector<QString> dab_name_vec;
    QVector<QString> dab_sid_vec;
    Tune mTune;


    //mStop_dac_scan = false;

//public:

};

#endif // SCAN_H
