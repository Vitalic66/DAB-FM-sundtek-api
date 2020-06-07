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
    struct dab_tuner data;
    fd = net_open(strdup("/dev/dab0"), O_RDWR);

    while(1) {

        QApplication::processEvents();

        if(mStop_sig) break;

        net_ioctl(fd, DAB_GET_TUNER, &data);

        QString dab_sig_strength = QString::number(data.signalstrength);
        QString dab_sig_status = QString::number(data.status);
        QString dab_sig_rssi = QString::number(data.rssi);
        QString dab_sig_fic = QString::number(data.fic_quality);

        emit sig_out(dab_sig_strength, dab_sig_status, dab_sig_rssi, dab_sig_fic);

        usleep(100000);
    }

net_close(fd);

return 0;

}
