#include "dialog.h"
#include "ui_dialog.h"

QString g_tuner_mode;

QString g_last_state_mute_unmute;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //start mediaclient in case it did not...
    QProcess::execute("/opt/bin/mediaclient --shutdown");
    QThread::msleep(600);
    QProcess::execute("/opt/bin/mediaclient --start");
    QThread::msleep(300);


    mFile.dab_read_file();
    Dialog::dab_fill_list(); //dab_vec_vec to fm_list

//    net_open("/dev/dab0", O_RDWR);
//    net_close(fd);

//    fd = net_open("/dev/dab0", O_RDWR);
//    mMute.set_mute(fd, "off");

    Dialog::dab_show_fav_btn();

}

Dialog::~Dialog()
{
    emit on_Stop();
    emit on_StopScan();
    QProcess::execute("/opt/bin/mediaclient --shutdown");
    delete ui;
}

void Dialog::on_btnStart_clicked()
{


    //connect(&mJob,&MyJob::on_number,this,&Dialog::newNumber); //signal on_number (in other h) , slot newNumber (in this cpp)
    //connect(this,&Dialog::on_Stop, &mJob, &MyJob::stop); //signal on_stop (in this h) , slo stop (in other cpp)

    connect(&mRds,&FM_rds::rds_out,this,&Dialog::rds_stream);
    connect(this,&Dialog::on_Stop,&mRds,&FM_rds::stop);


    //QFuture<void> test = QtConcurrent::run(&this->mJob,&MyJob::start,QString("katzen"));

    QFuture<void> test_rds = QtConcurrent::run(&this->mRds,&FM_rds::rds);
}

void Dialog::on_btnStop_clicked()
{
    emit on_Stop();
    emit on_StopScan();
}

//void Dialog::newNumber(QString name, int number)
//{
//    qDebug() << "From Dialog: " << name << " " << number;
//    ui->lineEdit->setText(name + " " + QString::number(number));
//}

void Dialog::rds_stream(QString radio_program)
{
    ui->label->setText(radio_program);
}

void Dialog::prog_bar_dab_valueChanged(int prog_bar_value)
{
    ui->prog_bar_dab->setValue(prog_bar_value);
}
/*
void Dialog::fill_dab_list(){

    ui->list_dab->clear();

    for(int i = 0; i < g_dab_vec_vec.size(); i++){

        ui->list_dab->addItem(g_dab_vec_vec[i][0]);
    }

    for(int i = 0; i < g_dab_vec_vec.size(); i++){

        ui->list_dab->addItem(g_dab_vec_vec[i][0]);
    }
}
*/
void Dialog::dab_fill_list()
{
    //clean up g_dab_vec_vec(doubles)

    for(int i = 1; i < g_dab_vec_vec.size(); i++){
        QVector<QString> tmp_0 = g_dab_vec_vec.at(i-1);

        if(tmp_0[2] == g_dab_vec_vec[i][2]){  //is sid the same?

            if(g_dab_vec_vec[i][3] == "fav"){
                g_dab_vec_vec.remove(i-1);
            }
        }
    }

    qDebug() << "g_dab_vec_vec cleanded?: " << g_dab_vec_vec;

    for(int i = 0; i < g_dab_vec_vec.size(); i++){
        //QString dab_to_list;
        //QVector<QString> tmp = g_dab_vec_vec.at(i-1); //holds -1

        ui->list_dab->addItem(g_dab_vec_vec[i][0]);
              //ui->list_dab->addItem(g_dab_vec_vec[i][0]);

        //mark fav green row
        if(g_dab_vec_vec[i][3].contains("fav")){
            ui->list_dab->setCurrentRow(i);
            ui->list_dab->currentItem()->setBackgroundColor(Qt::green);
        }
    }

    ui->list_dab->setCurrentRow(-1);
}

//void Dialog::disable_btn(){

//    ui->btnDabScanFreq->setEnabled(false);
//}

void Dialog::enable_disable_btn(bool btn_state){

    ui->btnDabScanFreq->setEnabled(btn_state);
    ui->btnDabTune->setEnabled(btn_state);
    ui->btnTune->setEnabled(btn_state);
}

//void Dialog::on_btnMute_clicked()
//{
//    fd = net_open("/dev/radio0", O_RDWR); // open device node

//    Dialog::set_mute(fd, "on");
//}

//void Dialog::on_btnUnmute_clicked()
//{
//    fd = net_open("/dev/radio0", O_RDWR); // open device node

//    Dialog::set_mute(fd, "off");
//}

//int Dialog::set_mute(int fd, const char *arg) {
//        //int type = 0;
//        struct v4l2_control control;
//        if (strcmp(arg, "off") == 0) {
//                control.id = V4L2_CID_AUDIO_MUTE;
//                control.value = 0;
//                fprintf(stdout, "Enabling audiostream\n");
//                net_ioctl(fd, VIDIOC_S_CTRL, &control);
//        } else if (strcmp(arg, "on") == 0) {
//                fprintf(stdout, "Disabling audiostream\n");
//                control.id = V4L2_CID_AUDIO_MUTE;
//                control.value = 1;
//                net_ioctl(fd, VIDIOC_S_CTRL, &control);
//        } else
//                fprintf(stdout, "Wrong argument [%s] choose between on|off\n", arg);

//        return 0;
//}

void Dialog::on_btnTune_clicked()
{
    //g_last_state_dab_fm = "FM";

    ui->label->clear();

    fd = net_open("/dev/radio0", O_RDWR);

    mMute.set_mute(fd,"off");
    qDebug() << "set mute: " << mMute.set_mute(fd, "off");
    int frequency;
    frequency = (ui->ln_freq->text()).toInt();
    //frequency = 94800000;
    int tuner = 0;
    //tuner = 1;


    //&mTune,Tune::set_radio_channel(fd,94800000,"RADIO");


    //connect(&rds,&MyJob::rds_out,this,&Dialog::rds_stream);
    //connect(&mTune,&Tune::set_radio_channel,)
    //connect(this,&Dialog::on_Stop,&rds,&MyJob::stop);
    //connect(this,&Dialog::on_btnTune_clicked,mTune.set_radio_channel(fd,frequency,tuner));
    mTune.set_radio_channel(fd,frequency,tuner);

}

void Dialog::on_btnDabTune_clicked()
{
    fd = net_open("/dev/dab0", O_RDWR);
    //tuner_state = "DAB";

    //g_last_state_dab_fm = "DAB";

    mMute.set_mute(fd, "off");
    qDebug() << "set mute: " << mMute.set_mute(fd, "off");

    int marked_row = (ui->list_dab->currentRow()); //marked row from dab list

    uint frequency = (g_dab_vec_vec[marked_row][1]).toUInt();

    QString sid_string = g_dab_vec_vec[marked_row][2];

//    uint frequency = (g_dab_vec_vec[marked_row][1]).toUInt();

//    QString sid_string = g_dab_vec_vec[marked_row][2];
    bool ok;
    uint sid = sid_string.toUInt(&ok, 16);
    qDebug() << "als int:" << sid;

    uint8_t sid_set = 1;
    uint8_t comp = 1;
    uint8_t comp_set = 1;


    //mTune.set_dab_channel_pretune(fd,frequency);
    //mTune.set_dab_freq(fd,frequency);
    //QThread::msleep(4000);
    mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);

    //QThread::msleep(200);
    //Dialog::set_mute(fd, "off");
}

void Dialog::on_btnDabScanFreq_clicked()
{
    mScan.mStop_dab_scan = false; //for new scan set false
    ui->list_dab->clear();

//    for(int i = 0;i < g_dab_vec_vec.size(); i++){

//        if(!g_dab_vec_vec[i][3].contains("fav")){
//            g_dab_vec_vec.remove(i);
//        }

//    }




    //Dialog::disable_btn();

    //ui->btnDabScanFreq->setEnabled(false);
    //QtConcurrent::run(&this->mScan,&Scan::dab_scan_wrapped);

    connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::prog_bar_dab_valueChanged); //feedback from scan to progressbar
    //connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::fill_dab_list); //feedback from scan to dab list
    connect(&mScan,&Scan::enable_buttons,this,&Dialog::enable_disable_btn);
    //connect(&mScan,&Scan::write_to_file,&mFile,&File::dab_write_file);

    QFuture<void> scan_dab = QtConcurrent::run(&this->mScan,&Scan::dab_scan_wrapped); //create new thread for scan

    //QFuture<void> stop_scan = QtConcurrent::

    connect(this,&Dialog::on_StopScan,&mScan,&Scan::stop_scan);

    //mFile.dab_write_file();

    //connect(&mScan,&Scan::finished_scan,this,&Dialog::dab_refresh_all);

    connect(&mScan,&Scan::finished_scan,this,&Dialog::dab_refresh_after_scan);


}



void Dialog::on_pushButton_clicked()
{
    //qDebug() << "dab_vev_vec: " << mScan.dab_vec_vec;
    qDebug() << "g_dab_vev_vec: " << g_dab_vec_vec;
}

void Dialog::on_pushButton_2_clicked()
{
    //connect(this,&Dialog::on_pushButton_2_clicked,&mFile,&File::dab_write_file);
    //mFile.dab_write_file();
}

void Dialog::on_btn_add_fav_clicked()
{
    int marked = ui->list_dab->currentRow();

    g_dab_vec_vec[marked].insert(3, "fav");

    Dialog::dab_refresh_all();
}

void Dialog::on_bnt_rem_fav_clicked()
{
    int marked = ui->list_dab->currentRow();

    g_dab_vec_vec[marked].insert(3, "");

    Dialog::dab_refresh_all();
}

void Dialog::dab_refresh_all()
{
    //Dialog::dab_write_file(); //write vecvec to file
    mFile.dab_write_file();
    mFile.dab_fav_write_file();
    ui->list_dab->clear();
    mFile.dab_read_file();
    Dialog::dab_fill_list(); //write file to vecvec
    Dialog::dab_show_fav_btn();
    //MainWindow::dab_disable_btn();
}

void Dialog::dab_refresh_after_scan()
{
    //Dialog::dab_write_file(); //write vecvec to file
    mFile.dab_write_file();
    //mFile.dab_fav_write_file();
    ui->list_dab->clear();
    mFile.dab_read_file();
    Dialog::dab_fill_list(); //write file to vecvec
    Dialog::dab_show_fav_btn();
    //MainWindow::dab_disable_btn();
}

void Dialog::on_btn_main_to_scan_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Dialog::on_btn_scan_to_main_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

bool Dialog::dab_logo_exists(QString sid) {

    QString dab_logo = path_dab_icons + sid +".png";

    QFileInfo check_dab_logo(dab_logo);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_dab_logo.exists() && check_dab_logo.isFile()) {
        return true;
    } else {
        return false;
    }
}

QPixmap Dialog::logo_dab(QString in)
{

    QPixmap dab_station_logo(in);
    QPixmap scaled_dab_station_logo = dab_station_logo.scaled(160, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QBitmap map(160, 120);
    map.fill(Qt::color0);

    QPainter painter(&map);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::color1);
    //painter.drawRoundedRect(3, 18, 153, 83, 11, 11);
    painter.drawRoundedRect(0, 15, 160, 90, 20, 20);


    scaled_dab_station_logo.setMask(map);

    return scaled_dab_station_logo;
}

void Dialog::dab_show_fav_btn()
{

    //QVector<int> fm_found_favs;

    dab_found_favs.clear();

    for(int i = 0; i < g_dab_vec_vec.size(); i++){
        if(g_dab_vec_vec[i][3] == "fav"){

            dab_found_favs.push_back(i);
        }
    }

//qDebug() << "found dab favs: " << dab_found_favs;



    if(dab_found_favs.size() >= 1){
        ui->btn_dab_st01->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(0)][2];
        //qDebug() << "btn_sid: " << btn_sid;
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st01->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            qDebug() << "dab_log: " << dab_logo;
            QPixmap back_from_scale {Dialog::logo_dab(dab_logo)};
            QIcon ButtonIcon(back_from_scale);
            ui->btn_dab_st01->setIcon(ButtonIcon);
            //ui->btn_dab_st01->setIconSize(scaled_dab_station_logo.rect().size());
            ui->btn_dab_st01->setIconSize(back_from_scale.rect().size());
            ui->btn_dab_st01->setStyleSheet("border: 0px");
        } else {
            ui->btn_dab_st01->setIcon(QIcon());
            ui->btn_dab_st01->setText(g_dab_vec_vec[dab_found_favs.at(0)][0]);
            ui->btn_dab_st01->setStyleSheet(btn_default_rounded);
        }
    } else if(dab_found_favs.size() < 1){
        ui->btn_dab_st01->setIcon(QIcon());
        ui->btn_dab_st01->setStyleSheet(btn_default_rounded);
        ui->btn_dab_st01->setEnabled(false);
        ui->btn_dab_st01->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 2){
        ui->btn_dab_st02->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(1)][2];
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st02->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap back_from_scale {Dialog::logo_dab(dab_logo)};
            QIcon ButtonIcon(back_from_scale);
            ui->btn_dab_st02->setIcon(ButtonIcon);
            ui->btn_dab_st02->setIconSize(back_from_scale.rect().size());
            ui->btn_dab_st02->setStyleSheet("border: 0px");
        } else {
            ui->btn_dab_st02->setIcon(QIcon());
            ui->btn_dab_st02->setText(g_dab_vec_vec[dab_found_favs.at(1)][0]);
            ui->btn_dab_st02->setStyleSheet(btn_default_rounded);
        }
    } else if(dab_found_favs.size() < 2){
        ui->btn_dab_st02->setIcon(QIcon());
        ui->btn_dab_st02->setStyleSheet(btn_default_rounded);
        ui->btn_dab_st02->setEnabled(false);
        ui->btn_dab_st02->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 3){
        ui->btn_dab_st03->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(2)][2];
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st03->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap back_from_scale {Dialog::logo_dab(dab_logo)};
            QIcon ButtonIcon(back_from_scale);
            ui->btn_dab_st03->setIcon(ButtonIcon);
            ui->btn_dab_st03->setIconSize(back_from_scale.rect().size());
            ui->btn_dab_st03->setStyleSheet("border: 0px");
        } else {
            ui->btn_dab_st03->setIcon(QIcon());
            ui->btn_dab_st03->setText(g_dab_vec_vec[dab_found_favs.at(2)][0]);
            ui->btn_dab_st03->setStyleSheet(btn_default_rounded);
        }
    } else if(dab_found_favs.size() < 3){
        ui->btn_dab_st03->setIcon(QIcon());
        ui->btn_dab_st03->setStyleSheet(btn_default_rounded);
        ui->btn_dab_st03->setEnabled(false);
        ui->btn_dab_st03->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 4){
        ui->btn_dab_st04->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(3)][2];
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st04->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap back_from_scale {Dialog::logo_dab(dab_logo)};
            QIcon ButtonIcon(back_from_scale);
            ui->btn_dab_st04->setIcon(ButtonIcon);
            ui->btn_dab_st04->setIconSize(back_from_scale.rect().size());
            ui->btn_dab_st04->setStyleSheet("border: 0px");
        } else {
            ui->btn_dab_st04->setIcon(QIcon());
            ui->btn_dab_st04->setText(g_dab_vec_vec[dab_found_favs.at(3)][0]);
            ui->btn_dab_st04->setStyleSheet(btn_default_rounded);
        }
    } else if(dab_found_favs.size() < 4){
        ui->btn_dab_st04->setIcon(QIcon());
        ui->btn_dab_st04->setStyleSheet(btn_default_rounded);
        ui->btn_dab_st04->setEnabled(false);
        ui->btn_dab_st04->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 5){
        ui->btn_dab_st05->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(4)][2];
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st05->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap back_from_scale {Dialog::logo_dab(dab_logo)};
            QIcon ButtonIcon(back_from_scale);
            ui->btn_dab_st05->setIcon(ButtonIcon);
            ui->btn_dab_st05->setIconSize(back_from_scale.rect().size());
            ui->btn_dab_st05->setStyleSheet("border: 0px");
        } else {
            ui->btn_dab_st05->setIcon(QIcon());
            ui->btn_dab_st05->setText(g_dab_vec_vec[dab_found_favs.at(4)][0]);
            ui->btn_dab_st05->setStyleSheet(btn_default_rounded);
        }
    } else if(dab_found_favs.size() < 5){
        ui->btn_dab_st05->setIcon(QIcon());
        ui->btn_dab_st05->setStyleSheet(btn_default_rounded);
        ui->btn_dab_st05->setEnabled(false);
        ui->btn_dab_st05->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 6){
        ui->btn_dab_st06->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(5)][2];
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st06->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap back_from_scale {Dialog::logo_dab(dab_logo)};
            QIcon ButtonIcon(back_from_scale);
            ui->btn_dab_st06->setIcon(ButtonIcon);
            ui->btn_dab_st06->setIconSize(back_from_scale.rect().size());
            ui->btn_dab_st06->setStyleSheet("border: 0px");
        } else {
            ui->btn_dab_st06->setIcon(QIcon());
            ui->btn_dab_st06->setText(g_dab_vec_vec[dab_found_favs.at(5)][0]);
            ui->btn_dab_st06->setStyleSheet(btn_default_rounded);
        }
    } else if(dab_found_favs.size() < 6){
        ui->btn_dab_st06->setIcon(QIcon());
        ui->btn_dab_st06->setStyleSheet(btn_default_rounded);
        ui->btn_dab_st06->setEnabled(false);
        ui->btn_dab_st06->setText("no favorite\navailable");
    }
}

void Dialog::tune_dab_wrapper(int btn_id)
{
    fd = net_open("/dev/dab0", O_RDWR);

    //g_last_state_dab_fm = "DAB";
    g_tuner_mode = "DAB";
    qDebug() << "g_tuner_mode: " << g_tuner_mode;
    g_last_state_mute_unmute = "unmuted";
    qDebug() << "g_last_state_mute_unmute: : " << g_last_state_mute_unmute;
    uint frequency = g_dab_vec_vec[dab_found_favs.at(btn_id)][1].toUInt();
    bool ok;
    uint sid = g_dab_vec_vec[dab_found_favs.at(btn_id)][2].toUInt(&ok, 16);
    uint8_t sid_set = 1;
    uint8_t comp = 1;
    uint8_t comp_set = 1;

    mMute.set_mute(fd, "off");
    mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
}

void Dialog::on_btn_dab_st01_clicked()
{    
    Dialog::tune_dab_wrapper(0);
}

void Dialog::on_btn_dab_st02_clicked()
{
    Dialog::tune_dab_wrapper(1);
}

void Dialog::on_btn_dab_st03_clicked()
{
    Dialog::tune_dab_wrapper(2);
}

void Dialog::on_btn_dab_st04_clicked()
{
    Dialog::tune_dab_wrapper(3);
}

void Dialog::on_btn_dab_st05_clicked()
{
    Dialog::tune_dab_wrapper(4);
}

void Dialog::on_btn_dab_st06_clicked()
{
    Dialog::tune_dab_wrapper(5);
}

void Dialog::on_btn_main_mute_clicked()
{
    QString tmp_mute_unmute_state;

    if(g_tuner_mode == "DAB"){
        fd = net_open("/dev/dab0", O_RDWR);
    } else{
        fd = net_open("/dev/radio0", O_RDWR);
    }

    if(g_last_state_mute_unmute == "unmuted"){

        //mute DAB
        if(g_tuner_mode == "DAB"){
            mMute.set_mute(fd, "on");
            //g_last_state_mute_unmute = "muted";
        }

    tmp_mute_unmute_state = "muted";
    }

    if(g_last_state_mute_unmute == "muted"){

        //unmute DAB
        if(g_tuner_mode == "DAB"){
            mMute.set_mute(fd, "off");
            //g_last_state_mute_unmute = "unmuted";
        }
    tmp_mute_unmute_state = "unmuted";
    }

    g_last_state_mute_unmute = tmp_mute_unmute_state;

}
