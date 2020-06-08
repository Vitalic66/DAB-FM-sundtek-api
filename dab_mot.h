#ifndef DAB_MOT_H
#define DAB_MOT_H

#include <QObject>
#include <QDebug>
#include <QImage>
#include <QApplication>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
#include <charsets.h>
#include <mot_manager.h>
#include <tools.h>

class dab_mot : public QObject
{
    Q_OBJECT
public:
    explicit dab_mot(QObject *parent = nullptr);
    std::vector<uint8_t> mot_data;

signals:

    void new_mot(QImage MOTImage);

public slots:

    void dab_mot_start();
    void dab_mot_stop();
    //std::vector<uint8_t> GetMOTData();

private:

    void dab_read_mot();
    void dab_process_mot_data();

    //size_t mot_len;

    struct mot_xdata {

        uint8_t status;
        uint8_t type;
        uint16_t len;
        uint8_t data[512];
        uint8_t bufq;

    } *xdata;

    MOTManager mot_manager;
    QImage motImage;

    bool mStop_mot;
};

#endif // DAB_MOT_H
