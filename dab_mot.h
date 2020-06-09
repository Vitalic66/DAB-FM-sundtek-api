#ifndef DAB_MOT_H
#define DAB_MOT_H

#include <QObject>
#include <QDebug>
#include <QImage>
#include <QApplication>
#include <QPixmap>

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
    std::vector<uint8_t> dls_data;
    size_t len;
    int charset;

signals:

    void new_mot(QImage MOTImage);
    void new_label(QString);

public slots:

    void dab_mot_start();
    void dab_mot_stop();

private:

    void dab_read_mot();
    void dab_process_mot_data();
    void dab_process_dls_data();

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

    //int width = 320;
    //int height = 240;
    QImage empty_image = QPixmap().toImage();
};

#endif // DAB_MOT_H
