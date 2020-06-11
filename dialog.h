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
#include <QTimer>

#include <fm_rds.h>
#include <scan.h>
#include <tune.h>
#include <mute.h>
#include <file.h>
#include <globals.h>
#include <dabstrength.h>
#include <dab_mot.h>
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



    //Scan *mScan;
    //FM_rds *mFM_rds; //m1



signals:
    //void on_Stop(); //rds streaming stop fm_rds.cpp

    //void on_StopScan(); //dab fm scan progressbar stop scan.cpp

    void start_scan_fm();
    void start_scan_dab();
    void stop_scan();
    void start_rds();
    void stop_rds();

    void start_mot();
    void stop_mot();

    //void start_mot_process();
    //void stop_mot_process();

    void tuner_mode_changed();

    void start_dab_sig();
    void stop_dab_sig();

    void dab_hide_unhide(bool vis);

    //void on_btnTune_clicked();
    //void sendWorkSpeed(int workSpeed);
    //void sendForSize(int forsize);


private slots:

    //void onNumberChanged(int);
    void prog_bar_fm_valueChanged(int);



    //void setWorkSpeed(int workSpeed);
    //void setForSize(int forsize);



    //void receiveFinished();
    //void receiveProgress(int workDone);






    //void on_btnStart_clicked(); //start rds stream

    //void on_btnStop_clicked();  //stop rds stream

    //void on_btnMute_clicked();

    //void on_btnUnmute_clicked();

    //int set_mute(int fd, const char *arg);

    void on_btn_tune_clicked();

    //void on_btnDabTune_clicked();

    void on_btn_scan_clicked();

    //void disable_btn();

    void on_btn_add_fav_clicked();

    void on_btn_rem_fav_clicked();

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

    //void on_btn_main_mute_clicked();

    //void on_btn_scan_mute_clicked();

    //void on_btn_settings_mute_clicked();

    void on_btn_tuner_mode_clicked();



    void on_btn_rename_station_clicked();

    void on_btn_add_clicked();

    void on_btn_delete_clicked();

    void on_list_fm_itemSelectionChanged();

    void on_list_dab_itemSelectionChanged();

    void on_tabWidget_currentChanged(int index);

    void on_btn_settings_to_main_clicked();

    void on_btn_settings_to_scan_clicked();



    void on_btn_main_to_settings_clicked();

    void on_btn_scan_to_settings_clicked();

    void setup_connections_btn_mute();

    void setup_connections_btn_exit();

    //void setup_connections_btn_tuner_mode();

    //void setup_connections_btn_scan();

    void on_ho_sl_settings_delay_autoplay_fm_valueChanged(int value);



    void on_ho_sl_settings_delay_autoplay_dab_valueChanged(int value);

    //void on_btn_start_dab_sig_clicked();

    void gui_mode_dab();
    void gui_mode_fm();

public slots:

    //void newNumber(QString name, int number);

    void rds_stream(QString rds_data);

    void rds_prog(QString prog);

    //void dab_sig_stream(uint32_t strength, uint32_t status, uint8_t rssi, uint8_t fic);
    void dab_sig_stream(QString strength, QString status, QString rssi, QString fic);

    void prog_bar_dab_valueChanged(int prog_bar_value);

    //void prog_bar_fm_valueChanged(int prog_bar_value);

    //void fill_dab_list();

    void enable_disable_btn(bool btn_state);

    void show_progbars(bool vis);

    //void clear_rds_labels();

    //void change_tuner_mode();

    void gui_dab_hide_unhide(bool vis);

    //void dab_show_mot(const MOT_FILE &slide);
void dab_show_mot(QImage MOTImage);


private:
    Ui::Dialog *ui;

//    QThread *thread_fm_scan;
//    QThread *thread_dab_scan;

    //QThread *thread_scan, *thread_rds;

    QThread *thread_scan;
    QThread *thread_rds;
    QThread *thread_dab_sig;

    QThread *thread_mot;
    QThread *thread_dab_mot;
    //QThread *thread_dab_mot_process;

    //QImage motImage;

//    void setup_connections_fm_scan();
//    void setup_connections_dab_scan();

    void setup_connections_mot();
    //void setup_connections_mot_process();
    void setup_connections_scan();
    void setup_connections_fm_rds();
    void setup_connections_dab_sig();

    //void setup_button_connections();

    void dab_btn_changer();
    void fm_btn_changer();

    void tune_autoplay();



    //MyJob mJob;
    //FM_rds mRds;
//Scan mScan;
    Tune mTune;
    Mute mMute;
    File mFile;
    //DabData mDabData;
    DabStrength mDabStrength;
    //FM_rds *mFM_rds;


    //QString tuner_mode = ""; //dab or fm

    QVector<int> fm_found_favs;
    QVector<int> dab_found_favs;

    QString path_dab_icons = "../stationlogos/";

    //uint set_sleep_delay;

    //stylesheets
    QString btn_default_rounded = ("border: 1px solid #ababab;border-radius: 20px;background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #efefef);");
    QString btn_rect_red = ("border: 1px solid #ababab;background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #e03636);");
    QString btn_rect_green = ("border: 1px solid #ababab;background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #61db48);");

    void btn_mute_change_text();

};



#endif // DIALOG_H
