#include "dab_mot.h"

dab_mot::dab_mot(QObject *parent) : QObject(parent),
    mStop_mot(false)
{

}

void dab_mot::dab_mot_start()
{
    mStop_mot = false;
    mot_data.clear();
    dls_data.clear();
    emit new_label("");
    emit new_mot(empty_image);
    dab_read_mot();
}

void dab_mot::dab_mot_stop()
{
    mStop_mot = true;
    mot_data.clear();
    dls_data.clear();
    emit new_label("");
    emit new_mot(empty_image);
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

        if (rv == 0) {

            if (data.len > 0) {

                xdata->status = 0;
                xdata->type = 0;
                xdata->len = data.len;
                //xdata->bufq = data.bufq;

                rv = net_ioctl(radioFD_xdata, DAB_GET_DIGITAL_SERVICE_DATA, xdata);

                if (rv == 0) {

                    if (xdata->type == MOT) {

                        mot_data.clear();

                        for(int i = 1; i < xdata->len+1; i++){ //shift offset +1 in sundtek

                            mot_data.push_back(xdata->data[i]);
                        }

                    } else if(xdata->type == DLS){

                        if(!(xdata->data[0] & 0x10)) {
                            if(xdata->len > 10){ //length of xdata must be bigger 10

                                dls_data.clear();
                                len = xdata->len;

                                for(int i = 3; i < xdata->len; i++){ //shift offset + 3 because sundtek api decodes complete message from 4th field

                                    dls_data.push_back(xdata->data[i]);
                                    charset = 0; //unknown if field 2 is char indicator at sundteks api
                                    /* if ever necessary (charsets.h)
                                     * EbuLatin = 0x00 // Complete EBU Latin based repertoire - see annex C
                                     * UnicodeUcs2 = 0x06
                                     * UnicodeUtf8 = 0x0F
                                     * calculate charset this way:
                                     * xdata->data[2] >> 4; (move field 2 (should be char indicator) bitwise 4 times right
                                    */
                                }
                            }
                        }
                    }
                }
            }
        }

    dab_process_mot_data(); //image
    dab_process_dls_data(); //text

    } //end while
}

void dab_mot::dab_process_mot_data()
{
    if(mot_manager.HandleMOTDataGroup(mot_data)){

        const MOT_FILE new_slide = mot_manager.GetFile();

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
            int subtype = new_slide.content_sub_type;
            const std::vector<uint8_t>& Data = new_slide.data;

            QByteArray qdata(reinterpret_cast<const char*>(Data.data()), static_cast<int>(Data.size()));

            motImage.loadFromData(qdata, subtype == 0 ? "GIF" : subtype == 1 ? "JPEG" : subtype == 2 ? "BMP" : "PNG");

            if (motImage.isNull()) {
                motImage = QImage(320, 240, QImage::Format_Alpha8);
                motImage.fill(Qt::transparent);
            }
        emit new_mot(motImage); //emit new_mot(small);
        }
    }
}

void dab_mot::dab_process_dls_data()
{
    if(dls_data.size() > 0){

        std::string label = toUtf8StringUsingCharset(dls_data.data(), (CharacterSet) 0, dls_data.size());
        QString qlabel = QString::fromUtf8(label.c_str());
        emit new_label(qlabel);
    }
}
