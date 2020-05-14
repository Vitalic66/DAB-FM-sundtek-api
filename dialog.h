#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QtConcurrent>


#include <myjob.h>
#include <scan.h>
#include <tune.h>
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
    //void on_btnTune_clicked();

private slots:
    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_btnMute_clicked();

    void on_btnUnmute_clicked();

    int set_mute(int fd, const char *arg);

    void on_btnTune_clicked();

    void on_btnDabTune_clicked();

    void on_btnDabScanFreq_clicked();



public slots:

    //void newNumber(QString name, int number);

    void rds_stream(QString radio_program);

    void prog_bar_dab_valueChanged(int prog_bar_value);

    void fill_dab_list();

private:
    Ui::Dialog *ui;
    //MyJob mJob;
    MyJob rds;
    Scan mScan;
    Tune mTune;
    int fd;




};



#endif // DIALOG_H
