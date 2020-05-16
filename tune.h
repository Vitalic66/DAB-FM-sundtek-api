#ifndef TUNE_H
#define TUNE_H

#include <QObject>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep

class Tune : public QObject
{
    Q_OBJECT
public:
    explicit Tune(QObject *parent = nullptr);

signals:

public slots:

    //int set_radio_channel(int fd);

    int set_dab_channel(int fd, uint32_t frequency, uint32_t sid, uint8_t sid_set, uint32_t comp, uint8_t comp_set);
    //int set_dab_channel(int fd, uint32_t frequency);

    int set_dab_freq(int fd, uint32_t frequency);

    //int set_dab_channel(int fd, uint32_t frequency, char sid, uint8_t sid_set, uint32_t comp, uint8_t comp_set);


    int set_radio_channel(int fd, int frequency, int tuner);

};

#endif // TUNE_H
