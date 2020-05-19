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

    int fm_scan_wrapped();

    //int media_scan_fm_frequencies(char *device, int devfd, int console, int running);
    int media_scan_fm_frequencies(char *device, int devfd);

    QVector<QString> dab_vec;
    QVector<QString> fm_vec;

    bool mStop_dab_scan;
    bool mStop_fm_scan;

signals:

    void progress_scan_dab(int prog_bar_dab);
    void progress_scan_fm(int prog_bar_fm);
    void enable_buttons(bool btn_st);
    void write_to_file();
    void finished_scan();
    void finished_scan_fm();

public slots:

    void stop_scan_dab();
    void stop_scan_fm();

private:
    //QVector<QVector<uint32_t>> dab_vec_vec;

    //QVector<uint32_t> dab_sid_vec;
    QVector<QString> dab_name_vec;
    QVector<QString> fm_name_vec;
    QVector<QString> dab_sid_vec;


    Tune mTune;


    //mStop_dac_scan = false;

//public:

};

#endif // SCAN_H
