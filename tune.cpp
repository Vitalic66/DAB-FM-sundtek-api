#include "tune.h"
//#include "mute.h"

#include <QDebug>
#include <QProcess>

uint32_t g_last_tuned_freq_dab;

Tune::Tune(QObject *parent) : QObject(parent)
{

}

int Tune::set_dab_freq(int fd, uint32_t frequency)
{

        struct dab_frequency dabf;
        memset(&dabf, 0x0, sizeof(struct dab_frequency));

        dabf.frequency = frequency;

        net_ioctl(fd, DAB_SET_FREQUENCY, &dabf);
        return 0;
}

int Tune::set_dab_channel(uint32_t frequency, uint32_t sid)
{

    emit dab_hide_unhide(true);

    fd = net_open(strdup("/dev/dab0"), O_RDWR);
    QString freq_as_string = QString::number(frequency);

    if(g_last_tuned_freq_dab != frequency){ //tune to dab freq first if not done yet
        g_last_tuned_freq_dab = frequency;

        QProcess::execute("/opt/bin/mediaclient -m DAB -f " + freq_as_string);
    }

    struct dab_frequency dabf;
    memset(&dabf, 0x0, sizeof(struct dab_frequency));

    dabf.frequency = frequency;

    dabf.sid = sid;

    net_ioctl(fd, DAB_SET_FREQUENCY, &dabf);
    return 0;
}

int Tune::set_radio_channel(uint32_t frequency)
{

    emit dab_hide_unhide(false);

    fd = net_open(strdup("/dev/radio0"), O_RDWR);

    uint32_t tuner = 0;

    struct v4l2_frequency freq;
    memset(&freq, 0x0, sizeof(struct v4l2_frequency));
    freq.frequency = frequency/1000*16;
    freq.type = V4L2_TUNER_RADIO;
    freq.tuner = tuner;

    net_ioctl(fd, VIDIOC_S_FREQUENCY, &freq);
    return 0;
}
