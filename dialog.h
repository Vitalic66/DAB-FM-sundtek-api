#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QtConcurrent>


#include <fm_rds.h>
#include <scan.h>
#include <tune.h>
#include <mute.h>
#include <file.h>
#include <globals.h>

#include </opt/include/mediaclient.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();



signals:
    void on_Stop();

    void on_StopScan();
    //void on_btnTune_clicked();



private slots:
    void on_btnStart_clicked();

    void on_btnStop_clicked();

    //void on_btnMute_clicked();

    //void on_btnUnmute_clicked();

    //int set_mute(int fd, const char *arg);

    void on_btnTune_clicked();

    void on_btnDabTune_clicked();

    void on_btnDabScanFreq_clicked();

    //void disable_btn();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_btn_add_fav_clicked();

    void on_bnt_rem_fav_clicked();

    void dab_refresh_all();

    void dab_refresh_after_scan();

    void dab_fill_list();

public slots:

    //void newNumber(QString name, int number);

    void rds_stream(QString radio_program);

    void prog_bar_dab_valueChanged(int prog_bar_value);

    //void fill_dab_list();

    void enable_disable_btn(bool btn_state);

private:
    Ui::Dialog *ui;
    //MyJob mJob;
    FM_rds mRds;
    Scan mScan;
    Tune mTune;
    Mute mMute;
    File mFile;

    int fd;
    QString tuner_state = ""; //dab or fm




};



#endif // DIALOG_H
