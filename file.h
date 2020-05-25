#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QCollator>
#include <QDebug>


//#include <scan.h>
#include <tune.h>
#include <globals.h>


using namespace std;

class File : public QObject
{
    Q_OBJECT
public:
    explicit File(QObject *parent = nullptr);

    void dab_read_file();

    void fm_read_file();

    void read_settings_file();

    void write_settings_file();

    QString set_autoplay;

    QString last_played_tuner_type;
    QString last_played_freq;
    QString last_played_sid;

    //Scan mScan;
    Tune mTune;

signals:

public slots:

    void dab_write_file();

    void dab_fav_write_file();

    void fm_write_file();

    void fm_fav_write_file();

private:

    QStringList sort_list(QStringList list);

    QString path_dab = "../.dab.txt";
    QString path_dab_fav = "../.dab_fav.txt";

    QString path_fm = "../.fm.txt";
    QString path_fm_fav = "../.fm_fav.txt";

    QString path_settings = "../.dabfm_settings.txt";





};

#endif // FILE_H
