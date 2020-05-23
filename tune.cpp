#include "tune.h"
//#include "mute.h"

#include <QDebug>
#include <QProcess>

uint32_t g_last_tuned_freq_dab;

Tune::Tune(QObject *parent) : QObject(parent)
{

}

int Tune::set_dab_freq(int fd, uint32_t frequency) {

        struct dab_frequency dabf;
        memset(&dabf, 0x0, sizeof(struct dab_frequency));

        printf("Tuning: %d\n", frequency);

        dabf.frequency = frequency;
        //qDebug() << "dabf.frequency: " << dabf.frequency;

        net_ioctl(fd, DAB_SET_FREQUENCY, &dabf);
        return 0;
}

int Tune::set_dab_channel(int fd, uint32_t frequency, uint32_t sid, uint8_t sid_set, uint32_t comp, uint8_t comp_set) {
    //int Tune::set_dab_channel(int fd, uint32_t frequency) {

    QString freq_as_string = QString::number(frequency);

    //qDebug() << "last_tuned_freq before compare: " << last_tuned_freq_dab;

    if(g_last_tuned_freq_dab != frequency){ //tune to dab freq first if not done yet
        g_last_tuned_freq_dab = frequency;

        QProcess::execute("/opt/bin/mediaclient -m DAB -f " + freq_as_string);
    }

    //qDebug() << "last_tuned_freq: " << g_last_tuned_freq_dab;

       //sid_set = -1;

//        qDebug() << "sid: " << sid;
//        qDebug() << "sid_set: " << sid_set;
//        qDebug() << "comp: " << comp;
//        qDebug() << "comp_set: " << comp_set;

        struct dab_frequency dabf;
        memset(&dabf, 0x0, sizeof(struct dab_frequency));
/*
        if (sid_set && comp_set)
                printf("Tuning: %d, 0x%x, 0x%x\n", frequency, sid, comp);
        else
*/
        if (sid_set)
                printf("Tuning: %d, 0x%x\n", frequency, sid);
        else
                printf("Tuning: %d\n", frequency);

        dabf.frequency = frequency;
        //qDebug() << "dabf.frequency: " << dabf.frequency;

        if (sid_set) {
                dabf.sid_set = 1;
                dabf.sid = sid;
        }
        if (comp_set) {
                dabf.comp = comp;
                dabf.comp_set = 1;
        }
        net_ioctl(fd, DAB_SET_FREQUENCY, &dabf);
        return 0;
}

int Tune::set_radio_channel(int fd, uint32_t frequency) {


        uint32_t tuner = 0;
    //last_tuned_freq_dab = 0;

//int Tune::set_radio_channel(int fd) {
        struct v4l2_frequency freq;
        memset(&freq, 0x0, sizeof(struct v4l2_frequency));
        freq.frequency = frequency/1000*16;
        //freq.frequency = frequency/1000;
        //qDebug() << "freq.frequency: " << freq.frequency;
        freq.type = V4L2_TUNER_RADIO;
        //qDebug() << "freq.type: " << freq.type;
        freq.tuner = tuner;
        //qDebug() << "freq.tuner: " << freq.tuner;
        //qDebug() <<"FD: " << fd;

        net_ioctl(fd, VIDIOC_S_FREQUENCY, &freq);
        return 0;
}
