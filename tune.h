#ifndef TUNE_H
#define TUNE_H

#include <QObject>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
#include <globals.h>

class Tune : public QObject
{
    Q_OBJECT
public:
    explicit Tune(QObject *parent = nullptr);

//    QString last_played_tuner_type;
//    QString last_played_freq;
//    QString last_played_sid;

signals:

    void dab_hide_unhide(bool vis);

public slots:

    //int set_radio_channel(int fd);

    //int set_dab_channel(int fd, uint32_t frequency, uint32_t sid, uint8_t sid_set, uint32_t comp, uint8_t comp_set);
    int set_dab_channel(uint32_t frequency, uint32_t sid);
    //int set_dab_channel(int fd, uint32_t frequency);

    int set_dab_freq(int fd, uint32_t frequency);

    //int set_dab_channel(int fd, uint32_t frequency, char sid, uint8_t sid_set, uint32_t comp, uint8_t comp_set);


    //int set_radio_channel(int fd, int frequency, int tuner);
    //int set_radio_channel(int fd, uint32_t frequency);
    int set_radio_channel(uint32_t frequency);

private:

    //uint32_t last_tuned_freq_dab;

};

#endif // TUNE_H
