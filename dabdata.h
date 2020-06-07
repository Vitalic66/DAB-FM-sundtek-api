#ifndef DABDATA_H
#define DABDATA_H

#include <QObject>
#include <QDebug>
#include <QImage>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
//#include <iostream>
#include <charsets.h>
#include <mot_manager.h>
#include <tools.h>
#include <pad_decoder.h>
#include <dabplus_decoder.h>

#define FPAD_LEN 2;

class DabData : public QObject, public PADDecoderObserver, public SubchannelSinkObserver
{
    Q_OBJECT
public:
    explicit DabData(QObject *parent = nullptr);
    //virtual void PADChangeSlide(const MOT_FILE& slide);


signals:

    void finished();
    void dlsUpdated();
    //void mot_finished(const MOT_FILE &slide);
    void mot_finished(const MOT_FILE &slide);
    void new_mot(QImage MOTImage);

public slots:

    void receive_dab_data();
    //void MOTProcessData(const std::vector<uint8_t>& dg);
    void MOTProcessData();

private:
    //QObject xdata;
    //struct xdata;
//    MOTEntity mMOTEntity;
//    MOTObject mMOTObject;
//    MOTManager mMOTManager;
    PADDecoder padDecoder;
    MOTManager mot_manager;
    SuperframeFilter superframefilter;
    //PADDecoderObserver *observer;
    QImage motImage;

    segs_t segs;
    int last_seg_number;
    size_t size;

    //ssize_t blub;
    void *buffer;
//    const uint8_t *buffer;
    size_t buflen;

    std::vector<uint8_t> data_ar;


};

#endif // DABDATA_H
