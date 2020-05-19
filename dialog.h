#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QtConcurrent>
#include <QFileInfo>
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QInputDialog>


#include <fm_rds.h>
#include <scan.h>
#include <tune.h>
#include <mute.h>
#include <file.h>
#include <globals.h>
#include <dabdata.h>

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

    void fm_refresh_all();

    void fm_refresh_after_scan();

    void fm_fill_list();

    void on_btn_main_to_scan_clicked();

    void on_btn_scan_to_main_clicked();

    bool dab_logo_exists(QString sid);

    void dab_show_fav_btn();
    void fm_show_fav_btn();

    QPixmap logo_dab(QString in);

    void on_btn_dab_st01_clicked();
    void on_btn_dab_st02_clicked();
    void on_btn_dab_st03_clicked();
    void on_btn_dab_st04_clicked();
    void on_btn_dab_st05_clicked();
    void on_btn_dab_st06_clicked();

    void on_btn_fm_st01_clicked();
    void on_btn_fm_st02_clicked();
    void on_btn_fm_st03_clicked();
    void on_btn_fm_st04_clicked();
    void on_btn_fm_st05_clicked();
    void on_btn_fm_st06_clicked();

    void tune_dab_wrapper(int btn_id);
    void tune_fm_wrapper(int btn_id);

    void on_btn_main_mute_clicked();

    void on_btn_scan_mute_clicked();

    void on_btn_tuner_mode_clicked();

    void on_btn_testbutton_clicked();

    void on_btn_rename_station_clicked();

    void on_btn_add_clicked();

    void on_btn_delete_clicked();

public slots:

    //void newNumber(QString name, int number);

    void rds_stream(QString radio_program);

    void prog_bar_dab_valueChanged(int prog_bar_value);

    void prog_bar_fm_valueChanged(int prog_bar_value);

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
    DabData mDabData;


    //QString tuner_mode = ""; //dab or fm

    QVector<int> fm_found_favs;
    QVector<int> dab_found_favs;

    QString path_dab_icons = "../stationlogos/";

    //stylesheets
    QString btn_default_rounded = ("border: 1px solid #ababab;border-radius: 20px;background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #efefef);");

};



#endif // DIALOG_H
