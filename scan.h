#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QVector>
#include <QThread>
#include <QDebug>   //new
#include <QMutex>

//#include <QStringList>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
#include <tune.h>
#include <globals.h>

class Scan : public QObject
//class Scan : public QThread //new2
{
    Q_OBJECT
public:
    explicit Scan(QObject *parent = nullptr);

    void run();
    bool Stop;
    bool mStop_fm_scan;

    QVector<QString> dab_vec;
    QVector<QString> fm_vec;

    //int fm_scan_wrapped();

    int media_scan_fm_frequencies(char *device, int devfd);

/*
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
*/
   // void DoSetup(QThread &cThread); //new

signals:

    void sendProgress(int workDone);
    void sendFinished();


    void NumberChanged(int);
void progress_scan_fm(int prog_bar_fm);
    //void progress_scan_fm(int);
    void finished_scan_fm();

/*
    void progress_scan_dab(int prog_bar_dab);
    void progress_scan_fm(int prog_bar_fm);
    void enable_buttons(bool btn_st);
    void show_progbar_fm(bool vis);
    void show_progbar_dab(bool vis);
    void write_to_file();
    void finished_scan();
    void finished_scan_fm();
*/
public slots:

    void fm_scan_wrapper();

/*
    void stop_scan_dab();
    void stop_scan_fm();
*/
    //void DoWork(); //new

private:

int workDone;

/*
    //QVector<QVector<uint32_t>> dab_vec_vec;

    //QVector<uint32_t> dab_sid_vec;
    QVector<QString> dab_name_vec;
    QVector<QString> fm_name_vec;
    QVector<QString> dab_sid_vec;


    Tune mTune;


    //mStop_dac_scan = false;

//public:
*/
};

#endif // SCAN_H
