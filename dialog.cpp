#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //Dialog::dab_read_file(); //read file to dab_vec_vec
    mFile.dab_read_file();
    Dialog::dab_fill_list(); //dab_vec_vec to fm_list
}

Dialog::~Dialog()
{
    emit on_Stop();
    emit on_StopScan();
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

    for(int i = 0; i < g_dab_vec_vec.size(); i++){
        //QString dab_to_list;

        //dab_to_list = g_dab_vec_vec[i][0];

        ui->list_dab->addItem(g_dab_vec_vec[i][0]);

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
    ui->label->clear();

    fd = net_open("/dev/radio0", O_RDWR);

    mMute.set_mute(fd,"off");

    int frequency;
    frequency = (ui->ln_freq->text()).toInt();
    //frequency = 94800000;
    int tuner = 1;
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

    mMute.set_mute(fd, "off");
    qDebug() << "set mute: " << mMute.set_mute(fd, "off");

    int marked_row = (ui->list_dab->currentRow()); //marked row from dab list

    uint frequency = (mScan.dab_vec_vec[marked_row][1]).toUInt();

    QString sid_string = mScan.dab_vec_vec[marked_row][2];

//    uint frequency = (g_dab_vec_vec[marked_row][1]).toUInt();

//    QString sid_string = g_dab_vec_vec[marked_row][2];
    bool ok;
    uint sid = sid_string.toUInt(&ok, 16);
    //qDebug() << "als int:" << sid;

    uint8_t sid_set = 1;
    uint8_t comp = 1;
    uint8_t comp_set = 1;

    mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
    //QThread::msleep(200);
    //Dialog::set_mute(fd, "off");
}

void Dialog::on_btnDabScanFreq_clicked()
{
    mScan.mStop_dab_scan = false; //for new scan set false
    ui->list_dab->clear();
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

    connect(&mScan,&Scan::finished_scan,this,&Dialog::dab_refresh_all);

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
    //MainWindow::dab_show_fav_btn();
    //MainWindow::dab_disable_btn();
}
