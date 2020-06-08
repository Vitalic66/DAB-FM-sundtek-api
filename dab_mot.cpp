#include "dab_mot.h"

dab_mot::dab_mot(QObject *parent) : QObject(parent),
    mStop_mot(false)
{

}

void dab_mot::dab_mot_start()
{
    mStop_mot = false;
    dab_read_mot();
}

void dab_mot::dab_mot_stop()
{
    mStop_mot = true;
}

void dab_mot::dab_read_mot()
{
    struct dab_service_data data;
    mot_data.clear();

    int rv;
    int radioFD_data = net_open(strdup("/dev/dab0"), O_RDWR);
    int radioFD_xdata = net_open(strdup("/dev/dab0"), O_RDWR);

    memset(&data, 0x0, sizeof(struct dab_service_data));

    data.status = 1;

    xdata = (struct mot_xdata *) malloc(1024);

    while(1){

        QApplication::processEvents();

        if(mStop_mot) break;



    rv = net_ioctl(radioFD_data, DAB_GET_DIGITAL_SERVICE_DATA, &data);
//    qDebug() <<"rv"<<rv;
//    qDebug()<<"data.len"<<data.len;
    //qDebug() <<"rv data"<<rv;
    if (rv == 0) {

        if (data.len > 0) {
            xdata->status = 0;
            xdata->type = 0;
            //xdata->type = data.type;
            xdata->len = data.len;
            //xdata->data = data.data;
            xdata->bufq = data.bufq;

            rv = net_ioctl(radioFD_xdata, DAB_GET_DIGITAL_SERVICE_DATA, xdata);
            //qDebug() <<"rv"<<rv;
            //qDebug() <<"rv xdata"<<rv;
                if (rv == 0) {

                    if (xdata->type == MOT) {
                        //qDebug() << "MOT";
                        mot_data.clear();

                        for(int i = 1; i < xdata->len+1; i++){ //shift offset +1 in sundtek

                            mot_data.push_back(xdata->data[i]);
                        }
                      //qDebug()<<mot_data;

                    }
                }
            }
            //net_close(radioFD_xdata);
        }

    //usleep(250000);
    dab_process_mot_data();
    //net_close(radioFD_data);
    //qDebug()<<mot_data;

    } //end while

}

void dab_mot::dab_process_mot_data()
{

    if(mot_manager.HandleMOTDataGroup(mot_data)){

        const MOT_FILE new_slide = mot_manager.GetFile();
        // check file type
        bool show_slide = true;
        if(new_slide.content_type != MOT_FILE::CONTENT_TYPE_IMAGE)
            show_slide = false;
        switch(new_slide.content_sub_type) {
        case MOT_FILE::CONTENT_SUB_TYPE_JFIF:
        case MOT_FILE::CONTENT_SUB_TYPE_PNG:
            break;
        default:
            show_slide = false;
        }

        if(show_slide){
            int subtype = new_slide.content_sub_type;                       //qDebug()<<"main subtype"<<subtype;
            const std::vector<uint8_t>& Data = new_slide.data;              //qDebug()<<"main data"<<new_slide.data;

            QByteArray qdata(reinterpret_cast<const char*>(Data.data()), static_cast<int>(Data.size()));

                                                                        //qDebug()<<"main qdata"<<qdata;

            motImage.loadFromData(qdata, subtype == 0 ? "GIF" : subtype == 1 ? "JPEG" : subtype == 2 ? "BMP" : "PNG");
//QImage small;
            if (motImage.isNull()) {
                motImage = QImage(320, 240, QImage::Format_Alpha8);
                motImage.fill(Qt::transparent);
                //small = motImage.scaled(240, 180, Qt::KeepAspectRatio);
                //
            }

            emit new_mot(motImage);
            //emit new_mot(small);

        }
    }
}

