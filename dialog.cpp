#include "dialog.h"
#include "ui_dialog.h"

//#include <QThread> //new
//#include <scan.h> //new

QString g_tuner_mode = "DAB";

QString g_last_state_mute_unmute;

int fd;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);



    //mScan = new Scan(this);
    //connect(mScan,SIGNAL(NumberChanged(int)),this,SLOT(onNumberChanged(int)));

    //connect(mScan,SIGNAL(progress_scan_fm(int)),this,SLOT(prog_bar_fm_valueChanged(int)));
    //connect(mScan,SIGNAL(progress_scan_fm(int)),this,&Dialog::prog_bar_dab_valueChanged(prog_bar_value));






    //start mediaclient in case it did not...
    QProcess::execute("/opt/bin/mediaclient --shutdown");
    QThread::msleep(600);
    QProcess::execute("/opt/bin/mediaclient --start");
    QThread::msleep(300);

    mFile.dab_read_file();
    mFile.fm_read_file();

    Dialog::dab_fill_list(); //dab_vec_vec to dab_list
    Dialog::fm_fill_list(); //fm_vec_vec to fm_list

//    net_open("/dev/dab0", O_RDWR);
//    net_close(fd);

//    fd = net_open("/dev/dab0", O_RDWR);
//    mMute.set_mute(fd, "off");

    Dialog::dab_show_fav_btn();
    Dialog::fm_show_fav_btn();

    // init some GUI buttons
    if(g_tuner_mode == "DAB"){
        ui->list_dab->setVisible(true);
        ui->list_fm->setVisible(false);
        ui->prog_bar_dab->setVisible(true);
        ui->prog_bar_fm->setTextVisible(false);
        ui->btn_tuner_mode->setText("to FM\nMODE");
        ui->btn_add->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->ln_add_station->setEnabled(false);
        ui->btn_tune->setEnabled(false);
    }

    if(g_tuner_mode == "FM"){
        ui->list_dab->setVisible(false);
        ui->list_fm->setVisible(true);
        ui->prog_bar_dab->setVisible(false);
        ui->prog_bar_fm->setTextVisible(true);
        ui->btn_tuner_mode->setText("to DAB\nMODE");
        ui->btn_tune->setEnabled(false);
    }

    ui->prog_bar_dab->setVisible(false);
    ui->prog_bar_fm->setVisible(false);

    qDebug() << "g_tuner_mode: " << g_tuner_mode;



    setup_connections_fm_scan();






}

Dialog::~Dialog()
{
    // Quit thread
    thread_fm_scan->quit();
    // Wait for it to be closed properly
    while(!thread_fm_scan->isFinished());
    // Delete thread and UI
    delete thread_fm_scan;


    //emit on_Stop();
    //emit on_StopScan();
    QProcess::execute("/opt/bin/mediaclient --shutdown");
    delete ui;
}
void Dialog::setup_connections_fm_scan()
{
    // Slots and signals
    // Connect buttons to slots
    //connect(ui->btn_scan, SIGNAL(clicked()), this, SLOT(setThreadState()));
    //connect(ui->buttonToggleWorker, SIGNAL(clicked()), this, SLOT(setThreadState()));
    //connect(ui->spinBoxWorkAmount, SIGNAL(valueChanged(int)), this, SLOT(setWorkAmount(int)));
    //connect(ui->sliderWorkSpeed, SIGNAL(valueChanged(int)), this, SLOT(setWorkSpeed(int)));

    // Create thread, worker and timer
    thread_fm_scan = new QThread();
    // Important that both the worker and timer are NOT members of this widget class otherwise thread affinity will not change at all!
    //Worker *worker = new Worker();
    Scan *scan_fm = new Scan();

    //QTimer *timer = new QTimer();
    //timer->setInterval(0);  // Timer's inteveral set to 0 means that timer will trigger an event as soon as there are no other events to be processed

    // Connect worker to widget and vice verser (buttons, progressBarWork)

    connect(scan_fm, SIGNAL(enable_buttons(bool)), this, SLOT(enable_disable_btn(bool))); //enable/disable some buttons during scan
    connect(scan_fm, SIGNAL(show_progbar_fm(bool)), this, SLOT(show_progbars(bool))); //hide/unhide progbars
    connect(scan_fm, SIGNAL(finished_scan_fm()), this, SLOT(fm_refresh_after_scan()));  //when finished do save and refresh
    connect(scan_fm, SIGNAL(progress_scan_fm(int)), this, SLOT(prog_bar_fm_valueChanged(int)));  //progress of scan fm for progressbar
    connect(this, SIGNAL(start_scan_fm()), scan_fm, SLOT(fm_scan_wrapper())); //start scanning

    // Mark timer and worker for deletion ones the thread is stopped
    connect(thread_fm_scan, SIGNAL(finished()), scan_fm, SLOT(deleteLater()));
    connect(thread_fm_scan, SIGNAL(finished()), thread_fm_scan, SLOT(deleteLater()));

    // Move worker to thread
    scan_fm->moveToThread(thread_fm_scan);

    // Start main event loop of thread
    thread_fm_scan->start();
}
/*
void Dialog::receiveProgress(int workDone)
{
    ui->prog_bar_fm->setValue(workDone);
    //ui->progressBar->setValue(workDone);
    ui->label->setText(QString::number(workDone));
    //ui->progressBarWork->setFormat(QString::number(workDone) + " out of " + QString::number(ui->progressBarWork->maximum()) + " | [time per chunk : " + QString::number(workSpeed) + "ms]");
}
*/

/*
void Dialog::on_btnStart_clicked()  //start rds streaming
{


    //connect(&mJob,&MyJob::on_number,this,&Dialog::newNumber); //signal on_number (in other h) , slot newNumber (in this cpp)
    //connect(this,&Dialog::on_Stop, &mJob, &MyJob::stop); //signal on_stop (in this h) , slo stop (in other cpp)

    connect(&mRds,&FM_rds::rds_out,this,&Dialog::rds_stream);
    connect(this,&Dialog::on_Stop,&mRds,&FM_rds::stop);


    //QFuture<void> test = QtConcurrent::run(&this->mJob,&MyJob::start,QString("katzen"));

    QFuture<void> test_rds = QtConcurrent::run(&this->mRds,&FM_rds::rds);
}
*/

void Dialog::start_rds_stream(){

    connect(&mRds,&FM_rds::rds_out,this,&Dialog::rds_stream);
    connect(this,&Dialog::on_Stop,&mRds,&FM_rds::stop);
    QFuture<void> test_rds = QtConcurrent::run(&this->mRds,&FM_rds::rds);
}

void Dialog::rds_stream(QString radio_program)
{
    ui->lbl_rds_stream->setText(radio_program);
}

/*
void Dialog::on_btnStop_clicked()  //stop rds streaming
{
    emit on_Stop(); //rds stop
    emit on_StopScan(); //dab fm scan progressbar stop
}
*/

//void Dialog::newNumber(QString name, int number)
//{
//    qDebug() << "From Dialog: " << name << " " << number;
//    ui->lineEdit->setText(name + " " + QString::number(number));
//}



void Dialog::prog_bar_dab_valueChanged(int prog_bar_value)
{
    ui->prog_bar_dab->setValue(prog_bar_value);
}

//void Dialog::prog_bar_fm_valueChanged(int prog_bar_value)
void Dialog::prog_bar_fm_valueChanged(int prog_bar_value)
{
    ui->prog_bar_fm->setValue(prog_bar_value);
    //ui->label->setText(QString::number(prog_bar_value));
}

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

    //qDebug() << "g_dab_vec_vec cleaned?: " << g_dab_vec_vec;

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

void Dialog::fm_fill_list()
{
    //clean up g_dab_vec_vec(doubles)

    for(int i = 1; i < g_fm_vec_vec.size(); i++){
        QVector<QString> tmp_0 = g_fm_vec_vec.at(i-1);

        if(tmp_0[1] == g_fm_vec_vec[i][1]){  //is freq the same?

            if(g_fm_vec_vec[i][2] == "fav"){
                g_fm_vec_vec.remove(i-1);
            }
        }
    }

    //qDebug() << "g_fm_vec_vec cleaned?: " << g_fm_vec_vec;

    for(int i = 0; i < g_fm_vec_vec.size(); i++){
        //QString dab_to_list;
        //QVector<QString> tmp = g_dab_vec_vec.at(i-1); //holds -1

        ui->list_fm->addItem(g_fm_vec_vec[i][0]);
              //ui->list_dab->addItem(g_dab_vec_vec[i][0]);

        //mark fav green row
        if(g_fm_vec_vec[i][2].contains("fav")){
            ui->list_fm->setCurrentRow(i);
            ui->list_fm->currentItem()->setBackgroundColor(Qt::green);
        }
    }

    ui->list_fm->setCurrentRow(-1);
}

//void Dialog::disable_btn(){

//    ui->btnDabScanFreq->setEnabled(false);
//}

void Dialog::enable_disable_btn(bool btn_state){

    ui->btn_scan->setEnabled(btn_state);
    ui->btn_tune->setEnabled(btn_state);
    ui->btn_tune->setEnabled(btn_state);
}

void Dialog::show_progbars(bool visibility){

    if(g_tuner_mode == "DAB"){
        ui->prog_bar_dab->setVisible(visibility);
    }

    if(g_tuner_mode == "FM"){
        ui->prog_bar_fm->setVisible(visibility);
    }
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



//void Dialog::on_btnDabTune_clicked()

void Dialog::on_btn_scan_clicked()
{

//mScan.mStop_fm_scan = false;

    //emit toggleThread();
    emit start_scan_fm();

    //mScan->mStop_fm_scan = false;

    //mScan->start();

    //connect(mScan,SIGNAL(progress_scan_fm(int)),this,SLOT(prog_bar_fm_valueChanged(int)));







    //new
    /*
    QThread cThread;
    Scan cObject;

    cObject.DoSetup(cThread);
    cObject.moveToThread(&cThread);

    cThread.start();
*/
    //new2



    /*
    if(g_tuner_mode == "DAB"){

        mScan.mStop_dab_scan = false; //for new scan set false
        ui->list_dab->clear();

        //test3
        //connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::prog_bar_dab_valueChanged); //feedback from scan to progressbar
        //connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::fill_dab_list); //feedback from scan to dab list
        //test3
        //connect(&mScan,&Scan::enable_buttons,this,&Dialog::enable_disable_btn);
        //test1
        //connect(&mScan,&Scan::show_progbar_dab,this,&Dialog::show_progbars);
        //connect(&mScan,&Scan::write_to_file,&mFile,&File::dab_write_file);

        QFuture<void> scan_dab = QtConcurrent::run(&this->mScan,&Scan::dab_scan_wrapped); //create new thread for scan

        //QFuture<void> stop_scan = QtConcurrent::
        //test1
        //connect(this,&Dialog::on_StopScan,&mScan,&Scan::stop_scan_dab);

        //mFile.dab_write_file();

        //connect(&mScan,&Scan::finished_scan,this,&Dialog::dab_refresh_all);
        //test2
        //connect(&mScan,&Scan::finished_scan,this,&Dialog::dab_refresh_after_scan);
    }

    if(g_tuner_mode == "FM"){

        mScan.mStop_fm_scan = false; //for new scan set false
        ui->list_fm->clear();
        //test3
        //connect(&mScan,&Scan::progress_scan_fm,this,&Dialog::prog_bar_fm_valueChanged); //feedback from scan to progressbar

        //connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::fill_dab_list); //feedback from scan to dab list
        //test3
        //connect(&mScan,&Scan::enable_buttons,this,&Dialog::enable_disable_btn);
        //test1
        //connect(&mScan,&Scan::show_progbar_fm,this,&Dialog::show_progbars);
        //connect(&mScan,&Scan::write_to_file,&mFile,&File::dab_write_file);

        QFuture<void> scan_fm = QtConcurrent::run(&this->mScan,&Scan::fm_scan_wrapped); //create new thread for scan

        //QFuture<void> stop_scan = QtConcurrent::
        //test1
        //connect(this,&Dialog::on_StopScan,&mScan,&Scan::stop_scan_fm);

        //mFile.dab_write_file();

        //connect(&mScan,&Scan::finished_scan,this,&Dialog::dab_refresh_all);
        //test2
        //connect(&mScan,&Scan::finished_scan_fm,this,&Dialog::fm_refresh_after_scan);
    }
*/

}

void Dialog::on_btn_add_fav_clicked()
{
    if(g_tuner_mode == "DAB"){
        int marked = ui->list_dab->currentRow();

        g_dab_vec_vec[marked].insert(3, "fav");

        Dialog::dab_refresh_all();
    }

    if(g_tuner_mode == "FM"){
        int marked = ui->list_fm->currentRow();

        g_fm_vec_vec[marked].insert(2, "fav");

        Dialog::fm_refresh_all();
    }
}

void Dialog::on_bnt_rem_fav_clicked()
{
    if(g_tuner_mode == "DAB"){
        int marked = ui->list_dab->currentRow();

        g_dab_vec_vec[marked].insert(3, "");

        Dialog::dab_refresh_all();
    }

    if(g_tuner_mode == "FM"){
        int marked = ui->list_fm->currentRow();

        g_fm_vec_vec[marked].insert(2, "");

        Dialog::fm_refresh_all();
    }
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

void Dialog::fm_refresh_all()
{
    //Dialog::dab_write_file(); //write vecvec to file
    mFile.fm_write_file();
    mFile.fm_fav_write_file();
    ui->list_fm->clear();
    mFile.fm_read_file();
    Dialog::fm_fill_list(); //write file to vecvec
    Dialog::fm_show_fav_btn();
    //MainWindow::dab_disable_btn();
}

void Dialog::fm_refresh_after_scan()
{
    //Dialog::dab_write_file(); //write vecvec to file
    mFile.fm_write_file();
    //mFile.dab_fav_write_file();
    ui->list_fm->clear();
    mFile.fm_read_file();
    Dialog::fm_fill_list(); //write file to vecvec
    Dialog::fm_show_fav_btn();
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
    //painter.setRenderHint(QPainter::Antialiasing, false);
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

    if(dab_found_favs.size() >= 1){
        ui->btn_dab_st01->setEnabled(true);
        QString btn_sid = "0x" + g_dab_vec_vec[dab_found_favs.at(0)][2];
        //qDebug() << "btn_sid: " << btn_sid;
        bool dab_logo_exist {Dialog::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st01->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            //qDebug() << "dab_log: " << dab_logo;
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

void Dialog::fm_show_fav_btn()
{

    //QVector<int> fm_found_favs;

    fm_found_favs.clear();

    for(int i = 0; i < g_fm_vec_vec.size(); i++){
        if(g_fm_vec_vec[i][2] == "fav"){

            fm_found_favs.push_back(i);
        }
    }

    ui->btn_fm_st01->setStyleSheet(btn_default_rounded);
    ui->btn_fm_st02->setStyleSheet(btn_default_rounded);
    ui->btn_fm_st03->setStyleSheet(btn_default_rounded);
    ui->btn_fm_st04->setStyleSheet(btn_default_rounded);
    ui->btn_fm_st05->setStyleSheet(btn_default_rounded);
    ui->btn_fm_st06->setStyleSheet(btn_default_rounded);

    if(fm_found_favs.size() >= 1){
        ui->btn_fm_st01->setEnabled(true);
        ui->btn_fm_st01->setText(g_fm_vec_vec[fm_found_favs.at(0)][0]);
    } else if(fm_found_favs.size() < 1){
        ui->btn_fm_st01->setEnabled(false);
        ui->btn_fm_st01->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 2){
        ui->btn_fm_st02->setEnabled(true);
        ui->btn_fm_st02->setText(g_fm_vec_vec[fm_found_favs.at(1)][0]);
    } else if(fm_found_favs.size() < 2){
        ui->btn_fm_st02->setEnabled(false);
        ui->btn_fm_st02->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 3){
        ui->btn_fm_st03->setEnabled(true);
        ui->btn_fm_st03->setText(g_fm_vec_vec[fm_found_favs.at(2)][0]);
    } else if(fm_found_favs.size() < 3){
        ui->btn_fm_st03->setEnabled(false);
        ui->btn_fm_st03->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 4){
        ui->btn_fm_st04->setEnabled(true);
        ui->btn_fm_st04->setText(g_fm_vec_vec[fm_found_favs.at(3)][0]);
    } else if(fm_found_favs.size() < 4){
        ui->btn_fm_st04->setEnabled(false);
        ui->btn_fm_st04->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 5){
        ui->btn_fm_st05->setEnabled(true);
        ui->btn_fm_st05->setText(g_fm_vec_vec[fm_found_favs.at(4)][0]);
    } else if(fm_found_favs.size() < 5){
        ui->btn_fm_st05->setEnabled(false);
        ui->btn_fm_st05->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 6){
        ui->btn_fm_st06->setEnabled(true);
        ui->btn_fm_st06->setText(g_fm_vec_vec[fm_found_favs.at(5)][0]);
    } else if(fm_found_favs.size() < 6){
        ui->btn_fm_st06->setEnabled(false);
        ui->btn_fm_st06->setText("no favorite\navailable");
    }
}

void Dialog::on_btn_tune_clicked()
{
    if(g_tuner_mode == "FM"){

        g_last_tuned_freq_dab = 0;

        emit on_Stop(); //stop rds stream

        ui->lbl_rds_stream->clear(); //clear rds output

        Dialog::start_rds_stream(); //start rds streaming

        fd = net_open("/dev/radio0", O_RDWR);

        g_last_state_mute_unmute = "muted";
        g_tuner_mode = "FM";

        int marked_row = (ui->list_fm->currentRow()); //marked row from fm list
        uint32_t frequency = (g_fm_vec_vec[marked_row][1]).toUInt();

        mMute.set_mute();
        mTune.set_radio_channel(fd, frequency);
    }

    if(g_tuner_mode == "DAB"){

        g_last_state_mute_unmute = "muted";
        g_tuner_mode = "DAB";


        emit on_Stop(); //stop rds stream
        ui->lbl_rds_stream->clear(); //clear rds output



        int marked_row = (ui->list_dab->currentRow()); //marked row from dab list

        uint frequency = (g_dab_vec_vec[marked_row][1]).toUInt();

        QString sid_string = g_dab_vec_vec[marked_row][2];

    //    uint frequency = (g_dab_vec_vec[marked_row][1]).toUInt();

    //    QString sid_string = g_dab_vec_vec[marked_row][2];
        bool ok;
        uint sid = sid_string.toUInt(&ok, 16);
        //qDebug() << "als int:" << sid;

        uint8_t sid_set = 1;
        uint8_t comp = 1;
        uint8_t comp_set = 1;


        //mTune.set_dab_channel_pretune(fd,frequency);
        //mTune.set_dab_freq(fd,frequency);
        //QThread::msleep(4000);
        mMute.set_mute();
        mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);

        //QThread::msleep(200);
        //Dialog::set_mute(fd, "off");
    }

}

void Dialog::tune_dab_wrapper(int btn_id)
{ 
    fd = net_open("/dev/dab0", O_RDWR);

    emit on_Stop(); //stop rds_stream

    ui->lbl_rds_stream->clear();




    //g_last_state_dab_fm = "DAB";
    g_tuner_mode = "DAB";
    //qDebug() << "g_tuner_mode: " << g_tuner_mode;
    g_last_state_mute_unmute = "muted";
    //qDebug() << "g_last_state_mute_unmute: : " << g_last_state_mute_unmute;
    uint frequency = g_dab_vec_vec[dab_found_favs.at(btn_id)][1].toUInt();
    bool ok;
    uint sid = g_dab_vec_vec[dab_found_favs.at(btn_id)][2].toUInt(&ok, 16);
    uint8_t sid_set = 1;
    uint8_t comp = 1;
    uint8_t comp_set = 1;

    //mMute.set_mute(fd, "off");
    mMute.set_mute();
    mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
}

void Dialog::tune_fm_wrapper(int btn_id)
{
    g_last_tuned_freq_dab = 0; //reset in case dab is chosen again

    fd = net_open("/dev/radio0", O_RDWR);

    emit on_Stop(); //stop rds_stream

    ui->lbl_rds_stream->clear();

    //start rds stream
    Dialog::start_rds_stream(); //start rds streaming

    //g_last_state_dab_fm = "DAB";
    g_tuner_mode = "FM";
    qDebug() << "g_tuner_mode: " << g_tuner_mode;
    g_last_state_mute_unmute = "muted";
    qDebug() << "g_last_state_mute_unmute: : " << g_last_state_mute_unmute;
    uint frequency = g_fm_vec_vec[fm_found_favs.at(btn_id)][1].toUInt();


    //mMute.set_mute(fd, "off");
    mMute.set_mute();
    mTune.set_radio_channel(fd,frequency);
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

void Dialog::on_btn_fm_st01_clicked()
{
    Dialog::tune_fm_wrapper(0);
}

void Dialog::on_btn_fm_st02_clicked()
{
    Dialog::tune_fm_wrapper(1);
}

void Dialog::on_btn_fm_st03_clicked()
{
    Dialog::tune_fm_wrapper(2);
}

void Dialog::on_btn_fm_st04_clicked()
{
    Dialog::tune_fm_wrapper(3);
}

void Dialog::on_btn_fm_st05_clicked()
{
    Dialog::tune_fm_wrapper(4);
}

void Dialog::on_btn_fm_st06_clicked()
{
    Dialog::tune_fm_wrapper(5);
}

void Dialog::on_btn_main_mute_clicked()
{
    mMute.set_mute();
}

void Dialog::on_btn_scan_mute_clicked()
{
    mMute.set_mute();
}

void Dialog::on_btn_tuner_mode_clicked()
{
    QString tmp_tuner_mode;

    if(g_tuner_mode == "DAB"){ //activating FM mode

        ui->btn_tuner_mode->setText("to DAB\nMODE");
        ui->list_dab->setVisible(false);
        ui->list_fm->setVisible(true);
        ui->list_dab->setCurrentRow(-1);
        //ui->prog_bar_dab->setVisible(false);
        //ui->prog_bar_fm->setVisible(true);
        ui->btn_add->setEnabled(true);
        ui->btn_rename_station->setEnabled(true);
        ui->ln_add_station->setEnabled(true);
        ui->list_fm->setCurrentRow(-1);


        tmp_tuner_mode = "FM";
    }

    if(g_tuner_mode == "FM"){ //activating DAB mode


        ui->btn_tuner_mode->setText("to FM\nMODE");
        ui->list_dab->setVisible(true);
        ui->list_fm->setVisible(false);
        ui->list_fm->setCurrentRow(-1);
        //ui->prog_bar_dab->setVisible(true);
        //ui->prog_bar_fm->setVisible(false);
        ui->btn_add->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->ln_add_station->setEnabled(false);
        ui->list_dab->setCurrentRow(-1);

        tmp_tuner_mode = "DAB";
    }

    g_tuner_mode = tmp_tuner_mode;

    qDebug() << "g_tuner_mode: " << g_tuner_mode;

    //enable/disable tune button depending on station selected or not
    int init_fm_list = ui->list_fm->currentRow();
    int init_dab_list = ui->list_dab->currentRow();

    if(init_fm_list == -1 && g_tuner_mode == "FM"){
        ui->btn_tune->setDisabled(true);
    }

    if(init_fm_list != -1 && g_tuner_mode == "FM"){
        ui->btn_tune->setDisabled(false);
    }

    if(init_dab_list == -1 && g_tuner_mode == "DAB"){
        ui->btn_tune->setDisabled(true);
    }

    if(init_dab_list != -1 && g_tuner_mode == "DAB"){
        ui->btn_tune->setDisabled(false);
    }

 /*
    if(ui->btn_tuner_mode->text() == "FM\nMODE"){
        ui->btn_tuner_mode->setText("DAB\nMODE");
        tuner_mode = "FM";
        ui->lst_dab->setVisible(false);
        ui->lst_fm->setVisible(true);
        //ui->btn_rename->setVisible(true);
        //ui->ln_man_tune->setVisible(true);
        //ui->label->setVisible(true);
        ui->btn_add->setEnabled(true);
        //ui->btn_man_tune->setVisible(true);
        ui->btn_add->setEnabled(true);
        ui->btn_rename_station->setEnabled(true);
        ui->ln_add_station->setEnabled(true);
        ui->lst_fm->setCurrentRow(-1);

        MainWindow::fm_refresh_all();
    } else {
        ui->btn_tuner_mode->setText("FM\nMODE");
        tuner_mode = "DAB";
        ui->lst_dab->setVisible(true);
        ui->lst_fm->setVisible(false);
        //ui->btn_rename->setVisible(false);
        //ui->ln_man_tune->setVisible(false);
        //ui->label->setVisible(false);
        ui->btn_add->setEnabled(false);
        //ui->btn_man_tune->setVisible(false);
        ui->btn_add->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->ln_add_station->setEnabled(false);
        ui->lst_dab->setCurrentRow(-1);

        MainWindow::dab_refresh_all();
    }
*/
}

void Dialog::on_btn_testbutton_clicked()
{
    mDabData.receive_dab_data();
}

void Dialog::on_btn_rename_station_clicked()
{
    int marked = ui->list_fm->currentRow();
    QString name_marked = ui->list_fm->currentItem()->text();

    bool ok;

    QString new_name = QInputDialog::getText(this, "enter new name", "new name: ",QLineEdit::Normal,name_marked, &ok);

    if(new_name == ""){
        new_name = name_marked;
    }

    if(new_name.contains(",")){
        new_name = new_name.replace(",", ".");
    }

    g_fm_vec_vec[marked].replace(0, new_name);

    Dialog::fm_refresh_all();
}

void Dialog::on_btn_add_clicked() //FM only
{
    QString add_station = ui->ln_add_station->text();

    if(!add_station.isEmpty()){

        if(add_station.contains(",")){
            add_station = add_station.replace(",", ".");
        }

        float add_station_float = add_station.toFloat();
        int to_mhz = add_station_float * 1000000;
        QString station_conv_string = (QString::number(to_mhz));

        if(!add_station.contains(".")){
            add_station = add_station.append(".0");
        }

        QVector<QString> fm_vec;

        fm_vec.push_back("man Station@" + add_station + "MHz");
        fm_vec.push_back(station_conv_string);
        fm_vec.push_back("");

        g_fm_vec_vec.push_back(fm_vec);

        //qDebug() << " add station fm vecvec:" << fm_vec_vec;
    }

    ui->ln_add_station->setText(""); //empty line for new entrie

    Dialog::fm_refresh_all();
}

void Dialog::on_btn_delete_clicked()
{
    if(g_tuner_mode == "FM"){
        int fm_marked_line = ui->list_fm->currentRow();
        //qDebug() << "line_remove fm list" << fm_marked_line;

        g_fm_vec_vec.remove(fm_marked_line);

        Dialog::fm_refresh_all();
    }

    if(g_tuner_mode == "DAB"){
        int dab_marked_line = ui->list_dab->currentRow();
        //qDebug() << "line_remove dab list" << dab_marked_line;

        g_dab_vec_vec.remove(dab_marked_line);

        Dialog::dab_refresh_all();
    }
}

void Dialog::on_list_fm_itemSelectionChanged()
{
    ui->btn_tune->setEnabled(true);
}

void Dialog::on_list_dab_itemSelectionChanged()
{
    ui->btn_tune->setEnabled(true);
}
/*
void Dialog::onNumberChanged(int Number)
{
    ui->label->setText(QString::number(Number));
}
*/
