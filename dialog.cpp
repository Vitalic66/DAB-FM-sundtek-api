#include "dialog.h"
#include "ui_dialog.h"

//QString g_tuner_mode = "FM";
QString g_last_state_mute_unmute = "unmuted";

int fd;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //start mediaclient in case it did not...
    QProcess::execute("/opt/bin/mediaclient --shutdown");
    QTimer::singleShot(600, [] {QProcess::execute("/opt/bin/mediaclient --start");});
    //QThread::msleep(600);
    //QProcess::execute("/opt/bin/mediaclient --start");
    //QThread::msleep(300);

    mFile.read_settings_file();

    //setup ############################################################################################################
    //tuner mode
    if(g_tuner_mode == "DAB"){
        ui->set_tuner_mode_dab->setChecked(true);
    } else {
        ui->set_tuner_mode_fm->setChecked(true);
    }
    //autoplay
    if(mFile.set_autoplay == "on"){
        ui->chkbx_settings_autoplay->setChecked(true);
        if(!mFile.last_played_tuner_type.isEmpty() && !mFile.last_played_freq.isEmpty()){
            //QThread::msleep(5000);
            if(mFile.last_played_tuner_type == "DAB"){
                QTimer::singleShot(mFile.delay_autoplay_dab.toInt(), this, &Dialog::tune_autoplay);
            } else {
                QTimer::singleShot(mFile.delay_autoplay_fm.toInt(), this, &Dialog::tune_autoplay);
            }

            //Dialog::tune_autoplay();
        }
    }

    mFile.dab_read_file();
    mFile.fm_read_file();

    Dialog::dab_fill_list(); //dab_vec_vec to dab_list
    Dialog::fm_fill_list(); //fm_vec_vec to fm_list

    Dialog::dab_show_fav_btn();
    Dialog::fm_show_fav_btn();

    connect(&mTune, &Tune::dab_hide_unhide, this, &Dialog::gui_dab_hide_unhide);
    connect(this, &Dialog::dab_hide_unhide, this, &Dialog::gui_dab_hide_unhide);

    //connect(&mDabData, &DabData::MOTProcessData, this, &Dialog::dab_show_mot);



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
        ui->btn_add_fav->setEnabled(false);
        ui->btn_rem_fav->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->btn_delete->setEnabled(false);
        //ui->lbl_rds->setVisible(false);
        //ui->lbl_rds_stream->setVisible(false);
        //ui->lbl_rds_station->setVisible(false);
        //ui->lbl_rds_station_stream->setVisible(false);
        ui->tabWidget->setCurrentIndex(0);

        emit dab_hide_unhide(true);
    }

    if(g_tuner_mode == "FM"){
        ui->list_dab->setVisible(false);
        ui->list_fm->setVisible(true);
        ui->prog_bar_dab->setVisible(false);
        ui->prog_bar_fm->setTextVisible(true);
        ui->btn_tuner_mode->setText("to DAB\nMODE");
        ui->btn_tune->setEnabled(false);
        ui->btn_add_fav->setEnabled(false);
        ui->btn_rem_fav->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->btn_delete->setEnabled(false);
//        ui->lbl_rds_station->setVisible(false);   //label dab station not implemented yet
//        ui->lbl_rds_station_stream->setVisible(false); //label dab station stream not implemented yet
        ui->tabWidget->setCurrentIndex(1);
        //ui->frame_dab_strength->setVisible(false);

        emit dab_hide_unhide(false);
    }

    ui->prog_bar_dab->setVisible(false);
    ui->prog_bar_fm->setVisible(false);

    ui->ho_sl_settings_delay_autoplay_fm->setValue(mFile.delay_autoplay_fm.toInt());
    ui->lbl_delay_value_fm->setText(mFile.delay_autoplay_fm);
    ui->ho_sl_settings_delay_autoplay_dab->setValue(mFile.delay_autoplay_dab.toInt());
    ui->lbl_delay_value_dab->setText(mFile.delay_autoplay_dab);

    setup_connections_mot();
    setup_connections_scan();
    setup_connections_fm_rds();
    setup_connections_dab_sig();
    setup_connections_btn_mute();
    setup_connections_btn_exit();



    //setup_connections_btn_tuner_mode();

    //setup_connections_btn_scan();


    //connect(this, &Dialog::tuner_mode_changed, &Dialog::btn_tuner_mode, click)

}

Dialog::~Dialog()
{
    //write to settings file
    //tuner mode
    if(ui->set_tuner_mode_dab->isChecked()){
        g_tuner_mode = "DAB";
    } else {
        g_tuner_mode ="FM";
    }
    //autostart
    if(ui->chkbx_settings_autoplay->isChecked()){
        mFile.set_autoplay = "on";
    } else {
        mFile.set_autoplay = "off";
    }
    //last played station
    //already in variables


    mFile.write_settings_file();

    emit stop_rds();
    emit stop_mot();
    emit stop_dab_sig();
    emit stop_scan();


    //qDebug() "status scan thread: " << thread_scan->isRunning()

    /*
    // Quit thread
    thread_fm_scan->quit();
    // Wait for it to be closed properly
    while(!thread_fm_scan->isFinished());
    // Delete thread and UI
    delete thread_fm_scan;

    // Quit thread
    thread_dab_scan->quit();
    // Wait for it to be closed properly
    while(!thread_dab_scan->isFinished());
    // Delete thread and UI
    delete thread_dab_scan;
    */
    /*
    // Quit thread
    thread_scan->quit();
    // Wait for it to be closed properly
    while(!thread_scan->isFinished());
    // Delete thread and UI
    delete thread_scan;

    // Quit thread
    thread_rds->quit();
    // Wait for it to be closed properly
    while(!thread_rds->isFinished());
    // Delete thread and UI
    delete thread_rds;
*/
    QProcess::execute("/opt/bin/mediaclient --shutdown");
    delete ui;
}

void Dialog::setup_connections_mot()
{
    thread_dab_mot = new QThread();

    dab_mot *dabmot = new dab_mot();

    connect(this, SIGNAL(start_mot()), dabmot, SLOT(dab_mot_start())); //start
    connect(this, SIGNAL(stop_mot()), dabmot, SLOT(dab_mot_stop())); //stop

    //connect(dabdata, SIGNAL(new_mot(QImage)), this, SLOT(dab_show_mot(QImage)));
    connect(dabmot, SIGNAL(new_mot(QImage)), this, SLOT(dab_show_mot(QImage)));
    connect(dabmot, SIGNAL(new_label(QString)), this, SLOT(rds_stream(QString)));

    dabmot->moveToThread(thread_dab_mot);
    thread_dab_mot->start();

/*
    thread_mot = new QThread();

    DabData *dabdata = new DabData();

    //connect(ui->btn_main_mute, &QPushButton::clicked, &mMute, &Mute::set_mute);

    //connect(ui->pushButton, &QPushButton::clicked, dabdata, &DabData::MOTProcessData);

    connect(this, SIGNAL(start_mot()), dabdata, SLOT(MOTProcessData())); //start

    //connect(dabdata, SIGNAL(mot_finished(const MOT_FILE)), this, SLOT(dab_show_mot(const MOT_FILE)));
    connect(dabdata, SIGNAL(new_mot(QImage)), this, SLOT(dab_show_mot(QImage)));

    dabdata->moveToThread(thread_mot);
    thread_mot->start();
*/

}
/*
void Dialog::setup_connections_mot_process()
{
    thread_dab_mot_process = new QThread();

    Dab_MOT_Process *dabmotprocess = new Dab_MOT_Process();

    connect(this, SIGNAL(start_mot()), dabmotprocess, SLOT(dab_mot_process_start())); //start
    connect(this, SIGNAL(stop_mot()), dabmotprocess, SLOT(dab_mot_process_stop())); //stop

    //connect(dabdata, SIGNAL(new_mot(QImage)), this, SLOT(dab_show_mot(QImage)));
    connect(dabmotprocess, SIGNAL(new_mot(QImage)), this, SLOT(dab_show_mot(QImage)));

    dabmotprocess->moveToThread(thread_dab_mot_process);
    thread_dab_mot_process->start();
}
*/
void Dialog::setup_connections_scan()
{
    // Create thread, worker
    thread_scan = new QThread();

    Scan *scan = new Scan();

    connect(scan, SIGNAL(enable_buttons(bool)), this, SLOT(enable_disable_btn(bool))); //enable/disable some buttons during scan

    connect(scan, SIGNAL(show_progbar_dab(bool)), this, SLOT(show_progbars(bool))); //hide/unhide progbars
    connect(scan, SIGNAL(finished_scan_dab()), this, SLOT(dab_refresh_after_scan()));  //when finished do save and refresh
    connect(scan, SIGNAL(progress_scan_dab(int)), this, SLOT(prog_bar_dab_valueChanged(int)));  //progress of scan fm for progressbar
    connect(this, SIGNAL(start_scan_dab()), scan, SLOT(dab_scan_wrapper())); //start scanning

    connect(scan, SIGNAL(show_progbar_fm(bool)), this, SLOT(show_progbars(bool))); //hide/unhide progbars
    connect(scan, SIGNAL(finished_scan_fm()), this, SLOT(fm_refresh_after_scan()));  //when finished do save and refresh
    connect(scan, SIGNAL(progress_scan_fm(int)), this, SLOT(prog_bar_fm_valueChanged(int)));  //progress of scan fm for progressbar
    connect(this, SIGNAL(start_scan_fm()), scan, SLOT(fm_scan_wrapper())); //start scanning


    // Mark timer and worker for deletion ones the thread is stopped
    //connect(thread_scan, SIGNAL(finished()), scan, SLOT(deleteLater()));
    //connect(thread_scan, SIGNAL(finished()), thread_scan, SLOT(deleteLater()));

    // Move worker to thread
    scan->moveToThread(thread_scan);
    thread_scan->start();
}

void Dialog::setup_connections_fm_rds()
{
    // Create thread, worker
    thread_rds = new QThread();

    FM_rds *rds = new FM_rds();

    connect(rds, SIGNAL(rds_out(QString)), this, SLOT(rds_stream(QString)));  //rds data stream
    //connect(&rds, &FM_rds::rds_prog_out(QString), this, Dialog::rds_prog(QString));
    connect(rds, SIGNAL(rds_prog_out(QString)), this, SLOT(rds_prog(QString)));  //rds data stream

    connect(this, SIGNAL(start_rds()), rds, SLOT(start_rds_reading())); //start rds streaming
    connect(this, SIGNAL(stop_rds()), rds, SLOT(stop_rds_reading())); //stop rds streaming

    // Mark timer and worker for deletion ones the thread is stopped
    //connect(thread_rds, SIGNAL(finished()), rds, SLOT(deleteLater()));
    //connect(thread_rds, SIGNAL(finished()), thread_rds, SLOT(deleteLater()));

    // Move worker to thread
    rds->moveToThread(thread_rds);
    thread_rds->start();
}

void Dialog::setup_connections_dab_sig()
{
    // Create thread, worker
    thread_dab_sig = new QThread();

    DabStrength *dab_sig = new DabStrength();

    //connect(dab_sig, SIGNAL(sig_out(uint32_t, uint32_t, uint8_t, uint8_t)), this, SLOT(dab_sig_stream(uint32_t, uint32_t, uint8_t, uint8_t)));
    connect(dab_sig, SIGNAL(sig_out(QString, QString, QString, QString)), this, SLOT(dab_sig_stream(QString, QString, QString, QString)));
    //connect(&rds, &FM_rds::rds_prog_out(QString), this, Dialog::rds_prog(QString));
    //connect(rds, SIGNAL(rds_prog_out(QString)), this, SLOT(rds_prog(QString)));

    connect(this, SIGNAL(start_dab_sig()), dab_sig, SLOT(start_sig_reading())); //start
    connect(this, SIGNAL(stop_dab_sig()), dab_sig, SLOT(stop_sig_reading())); //stop

    // Mark timer and worker for deletion ones the thread is stopped
    //connect(thread_rds, SIGNAL(finished()), rds, SLOT(deleteLater()));
    //connect(thread_rds, SIGNAL(finished()), thread_rds, SLOT(deleteLater()));

    // Move worker to thread
    dab_sig->moveToThread(thread_dab_sig);
    thread_dab_sig->start();
}

void Dialog::rds_stream(QString data)
{
    ui->lbl_rds_stream->setWordWrap(true);
    ui->lbl_rds_stream->setText(data);

    //ui->lbl_rds_stream_2->setWordWrap(true);
    //ui->lbl_rds_stream_"->setText(data);
}

void Dialog::rds_prog(QString prog)
{
    ui->lbl_rds_station_stream->setText(prog);
    ui->lbl_rds_station_stream_2->setText(prog);
}

//void Dialog::dab_sig_stream(uint32_t dab_strength, uint32_t dab_status, uint8_t dab_rssi, uint8_t dab_fic)
void Dialog::dab_sig_stream(QString dab_strength, QString dab_status, QString dab_rssi, QString dab_fic)
{
    /*
    ui->lbl_dab_strength->setText(QString::number(dab_strength));
    qDebug() <<"strength: " << dab_strength;
    ui->lbl_dab_status->setText(QString::number(dab_status));
    ui->lbl_dab_rssi->setText(QString::number(dab_rssi));
    ui->lbl_dab_fic->setText(QString::number(dab_fic));
    */
    //ui->lbl_dab_strength->setText(dab_strength);
    //qDebug() <<"strength: " << dab_strength;
    //ui->lbl_dab_status->setText(dab_status);
    //ui->lbl_dab_rssi->setText(dab_rssi);
    //ui->lbl_dab_fic->setText(dab_fic);


    //QString green = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}";
    //QString green = "QProgressBar::chunk {background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 green, stop: 1 green)}";
    //QString yellow = "QProgressBar::chunk {background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 yellow, stop: 1 yellow)}";
    //QString red = "QProgressBar::chunk {background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 red, stop: 1 red)}";

    if(dab_fic.toInt() > 97){

        //QPalette p = palette();
        //p.setColor(QPalette::Highlight, Qt::green);
        //setPalette(p);
        //ui->prog_bar_dab_fic->setStyleSheet("background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5, stop: 0 green, stop: 1 white)");
        //ui->prog_bar_dab_fic->setStyleSheet(green);
        ui->frame_dab_strength->setStyleSheet("background-color: rgb(0,255,0); border-radius: 5px;");
        ui->frame_dab_strength_2->setStyleSheet("background-color: rgb(0,255,0); border-radius: 5px;");
//    } else if (dab_fic.toInt() <= 97 && dab_fic.toInt() > 80) {
//        ui->prog_bar_dab_fic->setStyleSheet(yellow);
    } else {
        //ui->prog_bar_dab_fic->setStyleSheet(red);
        ui->frame_dab_strength->setStyleSheet("background-color: rgb(255,0,0); border-radius: 5px;");
        ui->frame_dab_strength_2->setStyleSheet("background-color: rgb(255,0,0); border-radius: 5px;");
    }

    //ui->prog_bar_dab_fic->setValue(dab_fic.toInt());
    //ui->prog_bar_dab_fic->setValue(100);

}

void Dialog::prog_bar_dab_valueChanged(int prog_bar_value)
{
    ui->prog_bar_dab->setValue(prog_bar_value);
}

void Dialog::prog_bar_fm_valueChanged(int prog_bar_value)
{
    ui->prog_bar_fm->setValue(prog_bar_value);
    //ui->label->setText(QString::number(prog_bar_value));
}

void Dialog::gui_dab_hide_unhide(bool vis)
{
    ui->frame_dab_strength->setVisible(vis);
    ui->lbl_dab_strength->setVisible(vis);
    ui->frame_dab_strength_2->setVisible(vis);
    ui->lbl_dab_strength_2->setVisible(vis);
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
    ui->btn_add_fav->setEnabled(btn_state);
    ui->btn_rem_fav->setEnabled(btn_state);
    ui->btn_rename_station->setEnabled(btn_state);
    ui->btn_delete->setEnabled(btn_state);
    ui->btn_tuner_mode->setEnabled(btn_state);
    ui->btn_add->setEnabled(btn_state);
    ui->btn_delete->setEnabled(btn_state);
    ui->btn_rename_station->setEnabled(btn_state);
    ui->ln_add_station->setEnabled(btn_state);
    ui->btn_scan_to_main->setEnabled(btn_state);
    ui->btn_scan_to_settings->setEnabled(btn_state);
    ui->btn_scan_mute->setEnabled(btn_state);
    ui->btn_add_fav->setEnabled(btn_state);
    ui->btn_rem_fav->setEnabled(btn_state);
}

void Dialog::show_progbars(bool visibility){

    if(g_tuner_mode == "DAB"){
        ui->prog_bar_dab->setVisible(visibility);
    }

    if(g_tuner_mode == "FM"){
        ui->prog_bar_fm->setVisible(visibility);
    }
}

void Dialog::on_btn_scan_clicked()
{
    if(g_tuner_mode == "DAB"){
        emit start_scan_dab();
    }

    if(g_tuner_mode == "FM"){
        g_last_state_mute_unmute = "unmuted";
        mMute.set_mute();
        emit start_scan_fm();
    }
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

void Dialog::on_btn_rem_fav_clicked()
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

void Dialog::on_btn_scan_to_main_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Dialog::on_btn_settings_to_main_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Dialog::on_btn_settings_to_scan_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Dialog::on_btn_main_to_scan_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    //Dialog::on_btn_tuner_mode_clicked();
}

void Dialog::on_btn_main_to_settings_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Dialog::on_btn_scan_to_settings_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
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
    emit stop_rds(); //stop rds stream
    emit stop_dab_sig();
    emit stop_mot();

    if(g_tuner_mode == "FM"){

        g_last_tuned_freq_dab = 0;

        emit start_rds();

        g_last_state_mute_unmute = "muted";
        g_tuner_mode = "FM";


        //fd = net_open("/dev/radio0", O_RDWR);
        int marked_row = (ui->list_fm->currentRow()); //marked row from fm list


        uint32_t frequency = (g_fm_vec_vec[marked_row][1]).toUInt();

        mMute.set_mute();
        //mTune.set_radio_channel(fd, frequency);
        mTune.set_radio_channel(frequency);

        mFile.last_played_freq = g_fm_vec_vec[marked_row][1];
        mFile.last_played_sid = "";
    }

    if(g_tuner_mode == "DAB"){

        //emit stop_mot();
        //emit stop_rds();

        emit start_dab_sig();

        emit start_mot();

        g_last_state_mute_unmute = "muted";
        g_tuner_mode = "DAB";

        int marked_row = (ui->list_dab->currentRow()); //marked row from dab list

        uint frequency = (g_dab_vec_vec[marked_row][1]).toUInt();

        QString sid_string = g_dab_vec_vec[marked_row][2];

        //fd = net_open("/dev/dab0", O_RDWR);
        bool ok;
        uint sid = sid_string.toUInt(&ok, 16);
        //qDebug() << "als int:" << sid;

        //uint8_t sid_set = 1;
        //uint8_t comp = 1;
        //uint8_t comp_set = 1;

        mMute.set_mute();
        //mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
        mTune.set_dab_channel(frequency,sid);

        //ui->lbl_rds_station_stream->setText("");
        ui->lbl_rds_station_stream->setText(g_dab_vec_vec[marked_row][0]);
        ui->lbl_rds_station_stream_2->setText(g_dab_vec_vec[marked_row][0]);
        //qDebug()<<g_dab_vec_vec[marked_row][0];
        ui->lbl_rds_stream->setText("");

        mFile.last_played_freq = g_dab_vec_vec[marked_row][1];
        mFile.last_played_sid = sid_string;
    }

    mFile.last_played_tuner_type = g_tuner_mode;


}

void Dialog::tune_dab_wrapper(int btn_id)
{ 
    if(g_tuner_mode != g_tuner_mode){
        emit tuner_mode_changed();
    }

    emit stop_rds();
    emit stop_dab_sig();
    emit stop_mot();

    //if(ui->tabWidget->currentIndex() != 1){

        Dialog::dab_btn_changer();
    //}
    emit start_dab_sig();
    emit start_mot();

    g_tuner_mode = "DAB";
    //qDebug() << "g_tuner_mode: " << g_tuner_mode;



    g_last_state_mute_unmute = "muted";
    //qDebug() << "g_last_state_mute_unmute: : " << g_last_state_mute_unmute;


//btn_mute_change_text();





    //fd = net_open("/dev/dab0", O_RDWR);
    uint frequency = g_dab_vec_vec[dab_found_favs.at(btn_id)][1].toUInt();
    bool ok;
    uint sid = g_dab_vec_vec[dab_found_favs.at(btn_id)][2].toUInt(&ok, 16);
    //uint8_t sid_set = 1;
    //uint8_t comp = 1;
    //uint8_t comp_set = 1;

    //mMute.set_mute(fd, "off");
    mMute.set_mute();
    //mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
    mTune.set_dab_channel(frequency, sid);

    ui->lbl_rds_station_stream->setText(g_dab_vec_vec[dab_found_favs.at(btn_id)][0]); //station name
    ui->lbl_rds_station_stream_2->setText(g_dab_vec_vec[dab_found_favs.at(btn_id)][0]); //station name
    ui->lbl_rds_stream->setText(""); //data stream (rds from fm or dls from dab

    mFile.last_played_tuner_type = g_tuner_mode;
    mFile.last_played_freq = g_dab_vec_vec[dab_found_favs.at(btn_id)][1];
    mFile.last_played_sid = g_dab_vec_vec[dab_found_favs.at(btn_id)][2];
}

void Dialog::tune_fm_wrapper(int btn_id)
{
    if(g_tuner_mode != g_tuner_mode){
        emit tuner_mode_changed();
    }

    emit stop_rds();
    emit stop_dab_sig();
    emit stop_mot();

    g_last_tuned_freq_dab = 0; //reset in case dab is chosen again

    //if(ui->tabWidget->currentIndex() != 0){

        Dialog::fm_btn_changer();
    //}

    emit start_rds();

    //g_last_state_dab_fm = "DAB";
    g_tuner_mode = "FM";
    //qDebug() << "g_tuner_mode: " << g_tuner_mode;
    g_last_state_mute_unmute = "muted";
    //qDebug() << "g_last_state_mute_unmute: : " << g_last_state_mute_unmute;
    //fd = net_open("/dev/radio0", O_RDWR);
    uint frequency = g_fm_vec_vec[fm_found_favs.at(btn_id)][1].toUInt();

    mMute.set_mute();
    //mTune.set_radio_channel(fd,frequency);
    mTune.set_radio_channel(frequency);

    mFile.last_played_tuner_type = g_tuner_mode;
    mFile.last_played_freq = g_fm_vec_vec[fm_found_favs.at(btn_id)][1];
    mFile.last_played_sid = "";
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

void Dialog::setup_connections_btn_mute()
{
    connect(ui->btn_main_mute, &QPushButton::clicked, &mMute, &Mute::set_mute);
    connect(ui->btn_scan_mute, &QPushButton::clicked, &mMute, &Mute::set_mute);
    connect(ui->btn_settings_mute, &QPushButton::clicked, &mMute, &Mute::set_mute);

    //connect(ui->btn_main_mute, &QPushButton::clicked, this, &Dialog::btn_mute_change_text);
    //connect(ui->btn_scan_mute, &QPushButton::clicked, this, &Dialog::btn_mute_change_text);
    //connect(ui->btn_settings_mute, &QPushButton::clicked, this, &Dialog::btn_mute_change_text);

    //connect(ui->btn_main_mute, SIGNAL(clicked()), &mMute, &mMute.set_mute()));

    connect(&mMute, &Mute::mute_state_changed, this, & Dialog::btn_mute_change_text);
}

void Dialog::setup_connections_btn_exit()
{
    connect(ui->btn_main_exit, &QPushButton::clicked, this, &Dialog::close);
    connect(ui->btn_scan_exit, &QPushButton::clicked, this, &Dialog::close);
    connect(ui->btn_settings_exit, &QPushButton::clicked, this, &Dialog::close);
}

/*
void Dialog::setup_connections_btn_tuner_mode()
{
    //qDebug()<<connect(ui->btn_tuner_mode, &QPushButton::clicked, ui->tabWidget, &QTabWidget::setCurrentIndex);
    connect(ui->btn_tuner_mode, &QPushButton::clicked, this, &Dialog::change_tuner_mode);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &Dialog::change_tuner_mode);
}
*/

void Dialog::btn_mute_change_text()
{
    if(g_last_state_mute_unmute == "muted"){
        //ui->btn_main_mute->setText("UNMUTE");
        ui->btn_main_mute->setIcon(QIcon(":/icons/mute.png"));
        ui->btn_main_mute->setStyleSheet(btn_rect_red);
        //ui->btn_scan_mute->setText("UNMUTE");
        ui->btn_scan_mute->setIcon(QIcon(":/icons/mute.png"));
        ui->btn_scan_mute->setStyleSheet(btn_rect_red);
        //ui->btn_settings_mute->setText("UNMUTE");
        ui->btn_settings_mute->setIcon(QIcon(":/icons/mute.png"));
        ui->btn_settings_mute->setStyleSheet(btn_rect_red);
    } else {
        //ui->btn_main_mute->setText("MUTE");
        ui->btn_main_mute->setIcon(QIcon(":/icons/unmute.png"));
        ui->btn_main_mute->setStyleSheet(btn_rect_green);
        //ui->btn_scan_mute->setText("MUTE");
        ui->btn_scan_mute->setIcon(QIcon(":/icons/unmute.png"));
        ui->btn_scan_mute->setStyleSheet(btn_rect_green);
        //ui->btn_settings_mute->setText("MUTE");
        ui->btn_settings_mute->setIcon(QIcon(":/icons/unmute.png"));
        ui->btn_settings_mute->setStyleSheet(btn_rect_green);
    }
}

void Dialog::on_btn_tuner_mode_clicked()
{

    QString tmp_tuner_mode;

    if(g_tuner_mode == "DAB"){ //activating FM mode

        Dialog::fm_btn_changer();
        ui->tabWidget->setCurrentIndex(1);
//qDebug()<<g_tuner_mode;
        tmp_tuner_mode = "FM";
    }

    if(g_tuner_mode == "FM"){ //activating DAB mode

        Dialog::dab_btn_changer();
        ui->tabWidget->setCurrentIndex(0);
//qDebug()<<g_tuner_mode;
        tmp_tuner_mode = "DAB";
    }

    g_tuner_mode = tmp_tuner_mode;
//qDebug()<<g_tuner_mode;
    //qDebug() << "g_tuner_mode: " << g_tuner_mode;

    //enable/disable tune button depending on station selected or not
    int init_fm_list = ui->list_fm->currentRow();
    int init_dab_list = ui->list_dab->currentRow();

    if(init_fm_list == -1 && g_tuner_mode == "FM"){
        ui->btn_tune->setDisabled(true);
        ui->btn_add_fav->setDisabled(true);
        ui->btn_rem_fav->setDisabled(true);
        ui->btn_rename_station->setDisabled(true);
        ui->btn_delete->setDisabled(true);
    }

    if(init_fm_list != -1 && g_tuner_mode == "FM"){
        ui->btn_tune->setDisabled(false);
        ui->btn_add_fav->setDisabled(false);
        ui->btn_rem_fav->setDisabled(false);
        ui->btn_rename_station->setDisabled(false);
        ui->btn_delete->setDisabled(false);
    }

    if(init_dab_list == -1 && g_tuner_mode == "DAB"){
        ui->btn_tune->setDisabled(true);
        ui->btn_add_fav->setDisabled(true);
        ui->btn_rem_fav->setDisabled(true);
        ui->btn_rename_station->setDisabled(true);
        ui->btn_delete->setDisabled(true);
    }

    if(init_dab_list != -1 && g_tuner_mode == "DAB"){
        ui->btn_tune->setDisabled(false);
        ui->btn_add_fav->setDisabled(false);
        ui->btn_rem_fav->setDisabled(false);
        ui->btn_rename_station->setDisabled(false);
        ui->btn_delete->setDisabled(false);
    }

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
    ui->btn_add_fav->setEnabled(true);
    ui->btn_rem_fav->setEnabled(true);
}

void Dialog::on_list_dab_itemSelectionChanged()
{
    ui->btn_tune->setEnabled(true);
    ui->btn_add_fav->setEnabled(true);
    ui->btn_rem_fav->setEnabled(true);
}
/*
void Dialog::onNumberChanged(int Number)
{
    ui->label->setText(QString::number(Number));
}
*/

void Dialog::dab_btn_changer()
{
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
    //ui->lbl_rds_stream->setVisible(false);
    //ui->lbl_rds->setVisible(false);
    //ui->lbl_rds_station->setVisible(false);
    //ui->lbl_rds_station_stream->setVisible(false);
}

void Dialog::fm_btn_changer()
{
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
    //ui->lbl_rds_stream->setVisible(true);
//    ui->lbl_rds->setVisible(true);
//    ui->lbl_rds_station->setVisible(true);
//    ui->lbl_rds_station_stream->setVisible(true);

}

void Dialog::on_tabWidget_currentChanged(int index)
{
    //if(index != index){
    emit tuner_mode_changed();
    //}


    //qDebug()<<index;
    //some bugfixes needed...
    /*
    if(ui->tabWidget->currentIndex() != 1){

        g_tuner_mode = "DAB";
        qDebug()<<g_tuner_mode;
        qDebug()<<"tab changed to dab";
    }
    if(ui->tabWidget->currentIndex() != 0){

        g_tuner_mode = "FM";
        qDebug()<<g_tuner_mode;
        qDebug()<<"tab changed to fm";
    }
*/
}

void Dialog::tune_autoplay()
{
    //QTimer *timer = new QTimer();

    if(mFile.last_played_tuner_type == "DAB"){

        emit start_mot();
        emit start_dab_sig();
        //QThread::msleep(3000);
        //usleep(3000000);
        //timer->start(3000);
        g_tuner_mode = "DAB";
        g_last_state_mute_unmute = "muted";
        //fd = net_open("/dev/dab0", O_RDWR);

        uint frequency = mFile.last_played_freq.toUInt();
//qDebug()<<frequency;
        bool ok;
        uint sid = mFile.last_played_sid.toUInt(&ok, 16);
//qDebug()<<sid;
        //uint8_t sid_set = 1;
        //uint8_t comp = 1;
        //uint8_t comp_set = 1;
//todo station name at autostart
        //ui->lbl_rds_station_stream->setText(g_dab_vec_vec[dab_found_favs.at(btn_id)][0]);
        mMute.set_mute();
        //mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
        mTune.set_dab_channel(frequency,sid);


    }

    if(mFile.last_played_tuner_type == "FM"){

        emit start_rds();


        g_last_state_mute_unmute = "muted";

        g_tuner_mode = "FM";
        //fd = net_open("/dev/radio0", O_RDWR);


        uint frequency = mFile.last_played_freq.toUInt();
//qDebug()<<frequency;

        mMute.set_mute();
        //mTune.set_radio_channel(fd,frequency);
        mTune.set_radio_channel(frequency);

    }

}

void Dialog::on_ho_sl_settings_delay_autoplay_fm_valueChanged(int value)
{
    mFile.delay_autoplay_fm = QString::number(value);
    ui->lbl_delay_value_fm->setText(mFile.delay_autoplay_fm);
}


void Dialog::on_ho_sl_settings_delay_autoplay_dab_valueChanged(int value)
{
    mFile.delay_autoplay_dab = QString::number(value);
    ui->lbl_delay_value_dab->setText(mFile.delay_autoplay_dab);
}

void Dialog::gui_mode_dab()
{
    //disable
//    ui->lbl_rds->setVisible(false);
//    ui->lbl_rds_stream->setVisible(false);
//    ui->lbl_rds_station->setVisible(false);
//    ui->lbl_rds_station_stream->setVisible(false);


    //enable
    ui->frame_dab_strength->setVisible(true);
    ui->frame_dab_strength_2->setVisible(true);
}

void Dialog::gui_mode_fm()
{
    //disable
    //ui->frame_dab_strength->setVisible(false);


    //enable

//    ui->lbl_rds->setVisible(true);
//    ui->lbl_rds_stream->setVisible(true);
//    ui->lbl_rds_station->setVisible(true);
//    ui->lbl_rds_station_stream->setVisible(true);


}

//void Dialog::on_btn_start_dab_sig_clicked()
//{
//    mDabStrength.start_sig_reading();
//}


void Dialog::dab_show_mot(QImage motImage)
{
    ui->lbl_dab_mot->setPixmap(QPixmap::fromImage(motImage));
}
