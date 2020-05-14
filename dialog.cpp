#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    emit on_Stop();
    delete ui;
}

void Dialog::on_btnStart_clicked()
{


    //connect(&mJob,&MyJob::on_number,this,&Dialog::newNumber); //signal on_number (in other h) , slot newNumber (in this cpp)
    //connect(this,&Dialog::on_Stop, &mJob, &MyJob::stop); //signal on_stop (in this h) , slo stop (in other cpp)

    connect(&rds,&MyJob::rds_out,this,&Dialog::rds_stream);
    connect(this,&Dialog::on_Stop,&rds,&MyJob::stop);


    //QFuture<void> test = QtConcurrent::run(&this->mJob,&MyJob::start,QString("katzen"));

    QFuture<void> test_rds = QtConcurrent::run(&this->rds,&MyJob::rds);
}

void Dialog::on_btnStop_clicked()
{
    emit on_Stop();
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

void Dialog::fill_dab_list(){

    ui->list_dab->clear();

    for(int i = 0; i < mScan.dab_vec_vec.size(); i++){

        ui->list_dab->addItem(mScan.dab_vec_vec[i][0]);
    }
}

void Dialog::on_btnMute_clicked()
{
    fd = net_open("/dev/radio0", O_RDWR); // open device node

    Dialog::set_mute(fd, "on");
}

void Dialog::on_btnUnmute_clicked()
{
    fd = net_open("/dev/radio0", O_RDWR); // open device node

    Dialog::set_mute(fd, "off");
}

int Dialog::set_mute(int fd, const char *arg) {
        //int type = 0;
        struct v4l2_control control;
        if (strcmp(arg, "off") == 0) {
                control.id = V4L2_CID_AUDIO_MUTE;
                control.value = 0;
                fprintf(stdout, "Enabling audiostream\n");
                net_ioctl(fd, VIDIOC_S_CTRL, &control);
        } else if (strcmp(arg, "on") == 0) {
                fprintf(stdout, "Disabling audiostream\n");
                control.id = V4L2_CID_AUDIO_MUTE;
                control.value = 1;
                net_ioctl(fd, VIDIOC_S_CTRL, &control);
        } else
                fprintf(stdout, "Wrong argument [%s] choose between on|off\n", arg);

        return 0;
}

void Dialog::on_btnTune_clicked()
{
    ui->label->clear();

    fd = net_open("/dev/radio0", O_RDWR);
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

    int marked_row = (ui->list_dab->currentRow()); //marked row from dab list

    int frequency = (mScan.dab_vec_vec[marked_row][1]).toInt();

    QString sid_string = mScan.dab_vec_vec[marked_row][2];
    bool ok;
    int sid = sid_string.toInt(&ok, 16);
    //qDebug() << "als int:" << sid;

    int sid_set = 1;
    int comp = 1;

    int comp_set;
    mTune.set_dab_channel(fd,frequency,sid,sid_set,comp,comp_set);
    //QThread::msleep(200);
    Dialog::set_mute(fd, "off");
}

void Dialog::on_btnDabScanFreq_clicked()
{
    mScan.mStop_dab_scan = false; //for new scan set false

    connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::prog_bar_dab_valueChanged); //feedback from scan to progressbar
    connect(&mScan,&Scan::progress_scan_dab,this,&Dialog::fill_dab_list); //feedback from scan to dab list

    QFuture<void> scan_dab = QtConcurrent::run(&this->mScan,&Scan::dab_scan_wrapped); //create new thread for scan
}


