#include "dabstrength.h"

DabStrength::DabStrength(QObject *parent) : QObject(parent)
{

}

void DabStrength::start_sig_reading()
{
    mStop_sig = false;
    dab_sig();
}

void DabStrength::stop_sig_reading()
{
    mStop_sig = true;
}

int DabStrength::dab_sig()
{
    //qDebug()<<"sig strength started";
    //struct fm_scan_parameters data;
    //struct v4l2_radio_sigstat data;
    //struct dab_frequency data;
    struct dab_tuner data;
    fd = net_open(strdup("/dev/dab0"), O_RDWR);

    //memset(&data, 0x0, sizeof(struct dab_tuner));

    while(1) {

        QApplication::processEvents();

        if(mStop_sig) break;

        net_ioctl(fd, DAB_GET_TUNER, &data);

        //printf("strength %d\n", data.signalstrength);
        //qDebug() << "sigstrength: " << data.signalstrength;
        //qDebug() << "status: " << data.status;
        //qDebug() << "rssi: " << data.rssi;
        //qDebug() << "fic_quality: " << data.fic_quality;

        //data.signalstrength = signalstrength;


        QString dab_sig_strength = QString::number(data.signalstrength);
        QString dab_sig_status = QString::number(data.status);
        QString dab_sig_rssi = QString::number(data.rssi);
        QString dab_sig_fic = QString::number(data.fic_quality);


        emit sig_out(dab_sig_strength, dab_sig_status, dab_sig_rssi, dab_sig_fic);


        //emit sig_out(data.signalstrength, data.status, data.rssi, data.fic_quality);

        usleep(100000);
    }

return 0;

}
