﻿#include "dabdata.h"

DabData::DabData(QObject *parent) : QObject(parent), padDecoder(this, true)
  , superframefilter(this, false, false)
  //, public SubchannelSinkObserver

{
    padDecoder.SetMOTAppType(12);
}

//static const unsigned short ebuLatinToUcs2[] = {
///* 0x00 - 0x07 */   0x00,  0x118,  0x12e,  0x172,  0x102,  0x116,  0x10e,  0x218,
///*  0x8 -  0xf */  0x21a,  0x10a,    0xa,    0xb,  0x120,  0x139,  0x17b,  0x143,
///* 0x10 - 0x17 */  0x105,  0x119,  0x12f,  0x173,  0x103,  0x117,  0x10f,  0x219,
///* 0x18 - 0x1f */  0x21b,  0x10b,  0x147,  0x11a,  0x121,  0x13a,  0x17c,   0x82,
///* 0x20 - 0x27 */   0x20,   0x21,   0x22,   0x23,  0x142,   0x25,   0x26,   0x27,
///* 0x28 - 0x2f */   0x28,   0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
///* 0x30 - 0x37 */   0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,
///* 0x38 - 0x3f */   0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
///* 0x40 - 0x47 */   0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,
///* 0x48 - 0x4f */   0x48,   0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
///* 0x50 - 0x57 */   0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,
///* 0x58 - 0x5f */   0x58,   0x59,   0x5a,   0x5b,  0x16e,   0x5d,  0x141,   0x5f,
///* 0x60 - 0x67 */  0x104,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,
///* 0x68 - 0x6f */   0x68,   0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
///* 0x70 - 0x77 */   0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,
///* 0x78 - 0x7f */   0x78,   0x79,   0x7a,   0xab,  0x16f,   0xbb,  0x13d,  0x126,
///* 0x80 - 0x87 */   0xe1,   0xe0,   0xe9,   0xe8,   0xed,   0xec,   0xf3,   0xf2,
///* 0x88 - 0x8f */   0xfa,   0xf9,   0xd1,   0xc7,  0x15e,   0xdf,   0xa1,  0x178,
///* 0x90 - 0x97 */   0xe2,   0xe4,   0xea,   0xeb,   0xee,   0xef,   0xf4,   0xf6,
///* 0x98 - 0x9f */   0xfb,   0xfc,   0xf1,   0xe7,  0x15f,  0x11f,  0x131,   0xff,
///* 0xa0 - 0xa7 */  0x136,  0x145,   0xa9,  0x122,  0x11e,  0x11b,  0x148,  0x151,
///* 0xa8 - 0xaf */  0x150, 0x20ac,   0xa3,   0x24,  0x100,  0x112,  0x12a,  0x16a,
///* 0xb0 - 0xb7 */  0x137,  0x146,  0x13b,  0x123,  0x13c,  0x130,  0x144,  0x171,
///* 0xb8 - 0xbf */  0x170,   0xbf,  0x13e,   0xb0,  0x101,  0x113,  0x12b,  0x16b,
///* 0xc0 - 0xc7 */   0xc1,   0xc0,   0xc9,   0xc8,   0xcd,   0xcc,   0xd3,   0xd2,
///* 0xc8 - 0xcf */   0xda,   0xd9,  0x158,  0x10c,  0x160,  0x17d,   0xd0,  0x13f,
///* 0xd0 - 0xd7 */   0xc2,   0xc4,   0xca,   0xcb,   0xce,   0xcf,   0xd4,   0xd6,
///* 0xd8 - 0xdf */   0xdb,   0xdc,  0x159,  0x10d,  0x161,  0x17e,  0x111,  0x140,
///* 0xe0 - 0xe7 */   0xc3,   0xc5,   0xc6,  0x152,  0x177,   0xdd,   0xd5,   0xd8,
///* 0xe8 - 0xef */   0xde,  0x14a,  0x154,  0x106,  0x15a,  0x179,  0x164,   0xf0,
///* 0xf0 - 0xf7 */   0xe3,   0xe5,   0xe6,  0x153,  0x175,   0xfd,   0xf5,   0xf8,
///* 0xf8 - 0xff */   0xfe,  0x14b,  0x155,  0x107,  0x15b,  0x17a,  0x165,  0x127
//};

//static const char* utf8_encoded_EBU_Latin[] = {
//"\0", "Ę", "Į", "Ų", "Ă", "Ė", "Ď", "Ș", "Ț", "Ċ", "\n","\v","Ġ", "Ĺ", "Ż", "Ń",
//"ą", "ę", "į", "ų", "ă", "ė", "ď", "ș", "ț", "ċ", "Ň", "Ě", "ġ", "ĺ", "ż", "\u0082",
//" ", "!", "\"","#", "ł", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
//"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
//"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
//"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "Ů", "]", "Ł", "_",
//"Ą", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
//"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "«", "ů", "»", "Ľ", "Ħ",
//"á", "à", "é", "è", "í", "ì", "ó", "ò", "ú", "ù", "Ñ", "Ç", "Ş", "ß", "¡", "Ÿ",
//"â", "ä", "ê", "ë", "î", "ï", "ô", "ö", "û", "ü", "ñ", "ç", "ş", "ğ", "ı", "ÿ",
//"Ķ", "Ņ", "©", "Ģ", "Ğ", "ě", "ň", "ő", "Ő", "€", "£", "$", "Ā", "Ē", "Ī", "Ū",
//"ķ", "ņ", "Ļ", "ģ", "ļ", "İ", "ń", "ű", "Ű", "¿", "ľ", "°", "ā", "ē", "ī", "ū",
//"Á", "À", "É", "È", "Í", "Ì", "Ó", "Ò", "Ú", "Ù", "Ř", "Č", "Š", "Ž", "Ð", "Ŀ",
//"Â", "Ä", "Ê", "Ë", "Î", "Ï", "Ô", "Ö", "Û", "Ü", "ř", "č", "š", "ž", "đ", "ŀ",
//"Ã", "Å", "Æ", "Œ", "ŷ", "Ý", "Õ", "Ø", "Þ", "Ŋ", "Ŕ", "Ć", "Ś", "Ź", "Ť", "ð",
//"ã", "å", "æ", "œ", "ŵ", "ý", "õ", "ø", "þ", "ŋ", "ŕ", "ć", "ś", "ź", "ť", "ħ"};

//static const unsigned short ebuLatinToUcs2_2[] = {
///* 0x00 - 0x07 */ 0x00,   0x01,   0x02,   0x03,   0x04,   0x05,   0x06,   0x07,
///* 0x08 - 0x0f */ 0x08,   0x09,   0x0a,   0x0b,   0x0c,   0x0d,   0x0e,   0x0f,
///* 0x10 - 0x17 */ 0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,
///* 0x18 - 0x1f */ 0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,   0x1e,   0x2d,

///* 0x20 - 0x27 */ 0x20,   0x21,   0x22,   0x23,   0xa4,   0x25,   0x26,   0x27,
///* 0x28 - 0x2f */ 0x28,   0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
///* 0x30 - 0x37 */ 0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,
///* 0x38 - 0x3f */ 0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
///* 0x40 - 0x47 */ 0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,
///* 0x48 - 0x4f */ 0x48,   0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
///* 0x50 - 0x57 */ 0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,
///* 0x58 - 0x5f */ 0x58,   0x59,   0x5a,   0x5b,   0x5c,   0x5d,   0x2015, 0x5f,
///* 0x60 - 0x67 */ 0x2551, 0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,
///* 0x68 - 0x6f */ 0x68,   0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
///* 0x70 - 0x77 */ 0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,
///* 0x78 - 0x7f */ 0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0xaf,   0x7f,

///* 0x80 - 0x87 */ 0xe1,   0xe0,   0xe9,   0xe8,   0xed,   0xec,   0xf3,   0xf2,
///* 0x88 - 0x8f */ 0xfa,   0xf9,   0xd1,   0xc7,   0x015e, 0xdf,   0xa1,   0x0132,
///* 0x90 - 0x97 */ 0xe2,   0xe4,   0xea,   0xeb,   0xee,   0xef,   0xf4,   0xf6,
///* 0x98 - 0x9f */ 0xfb,   0xfc,   0xf1,   0xe7,   0x015f, 0x011f, 0x0131, 0x0133,
///* 0xa0 - 0xa7 */ 0xaa,   0x03b1, 0xa9,   0x2030, 0x011e, 0x011b, 0x0148, 0x0151,
///* 0xa8 - 0xaf */ 0x03c0, 0x20ac, 0xa3,   0x24,   0x2190, 0x2191, 0x2192, 0x2193,
///* 0xb0 - 0xb7 */ 0xba,   0xb9,   0xb2,   0xb3,   0xb1,   0x0130, 0x0144, 0x0171,
///* 0xb8 - 0xbf */ 0xb5,   0xbf,   0xf7,   0xb0,   0xbc,   0xbd,   0xbe,   0xa7,
///* 0xc0 - 0xc7 */ 0xc1,   0xc0,   0xc9,   0xc8,   0xcd,   0xcc,   0xd3,   0xd2,
///* 0xc8 - 0xcf */ 0xda,   0xd9,   0x0158, 0x010c, 0x0160, 0x017d, 0xd0,   0x13f,
///* 0xd0 - 0xd7 */ 0xc2,   0xc4,   0xca,   0xcb,   0xce,   0xcf,   0xd4,   0xd6,
///* 0xd8 - 0xdf */ 0xdb,   0xdc,   0x0159, 0x010d, 0x0161, 0x017e, 0x0111, 0x0140,
///* 0xe0 - 0xe7 */ 0x00c3, 0x00c5, 0x00c6, 0x0152, 0x0177, 0xdd,   0xd5,   0xd8,
///* 0xe8 - 0xef */ 0xde,   0x014a, 0x0154, 0x0106, 0x015a, 0x0179, 0x0166, 0xf0,
///* 0xf0 - 0xf7 */ 0xe3,   0xe5,   0xe6,   0x0153, 0x0175, 0xfd,   0xf5,   0xf8,
///* 0xf8 - 0xff */ 0xfe,   0x014b, 0x0155, 0x0107, 0x015b, 0x017a, 0x0167, 0xff
//};

//void DabData::MOTProcessData(const std::vector<uint8_t>& mot_data)
void DabData::MOTProcessData()
{
    //std::vector<uint8_t> data_ar;
    bool mot_stop = false;
    do{
        receive_dab_data();

        if(mot_manager.HandleMOTDataGroup(data_ar)) mot_stop = true;
        usleep(250000);

    }while(!mot_stop);

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
        int subtype = new_slide.content_sub_type;                       qDebug()<<"main subtype"<<subtype;
        const std::vector<uint8_t>& Data = new_slide.data;              qDebug()<<"main data"<<new_slide.data;

        QByteArray qdata(reinterpret_cast<const char*>(Data.data()), static_cast<int>(Data.size()));

                                                                    qDebug()<<"main qdata"<<qdata;

        motImage.loadFromData(qdata, subtype == 0 ? "GIF" : subtype == 1 ? "JPEG" : subtype == 2 ? "BMP" : "PNG");

        if (motImage.isNull()) {
            motImage = QImage(320, 240, QImage::Format_Alpha8);
            motImage.fill(Qt::transparent);
        }

        emit new_mot(motImage);

    }

        //emit mot_finished(new_slide);

}





void DabData::receive_dab_data()
{
    struct dab_service_data data;
    data_ar.clear();
    //std::vector<uint8_t> data_ar;

    struct test {

        uint8_t status;
        uint8_t type;
        uint16_t len;
        //size_t len;
        //uint8_t data[512];
        //std::vector<uint8_t> data[0];
        //const uint8_t data[0];
        uint8_t data[0];
        uint8_t bufq;

    } *xdata;

    int rv;
    int radioFD;
            radioFD = net_open(strdup("/dev/dab0"), O_RDWR);

    //memset(&ens, 0x0, sizeof(struct dab_ensemble_info));
    memset(&data, 0x0, sizeof(struct dab_service_data));

    data.status = 1;

    //xdata = (struct test *) malloc(16384); //fixed 16mb else prog crashes
    //xdata = (struct test *) malloc(sizeof (test)); //crash double free
    xdata = (struct test *) malloc(65536);

    qDebug() << "xdata.status" << xdata->status << "data.status" << data.status;
    //qDebug() << "xdata.type" << xdata->type << "data.type" << data.type;

//    std::vector<uint8_t> bufq_ar;

//    for(int i = 0; i < xdata->bufq; i++){
//        bufq_ar.push_back(xdata->data[i]);
//    }
//    qDebug()<<"bufq_ar"<<bufq_ar;




    rv = net_ioctl(radioFD, DAB_GET_DIGITAL_SERVICE_DATA, &data);
//    qDebug() <<"rv"<<rv;
//    qDebug()<<"data.len"<<data.len;
    if (rv == 0) {
        if (data.len > 0) {
            xdata->status = 0;
            xdata->type = 0;
            //xdata->type = data.type;
            xdata->len = data.len;
            //xdata->data = data.data;
            xdata->bufq = data.bufq;

            rv = net_ioctl(radioFD, DAB_GET_DIGITAL_SERVICE_DATA, xdata);
            //qDebug() <<"rv"<<rv;
            if (rv == 0) {
                if (xdata->type == MOT) {
                    qDebug() << "MOT";
                    //std::vector<uint8_t> data_ar;

                    //do{
                        //data_ar.clear();


                        for(int i = 1; i < xdata->len+1; i++){ //shift offset +1 in sundtek

                            data_ar.push_back(xdata->data[i]);
                        }

                        //mot_manager.HandleMOTDataGroup(data_ar);
                    //}while(!mot_manager.HandleMOTDataGroup(data_ar));

                }
            }
        }
}


                    /*
                    if(!mot_manager.HandleMOTDataGroup(data_ar)){   qDebug()<<mot_manager.HandleMOTDataGroup(data_ar);
//receive_dab_data();
mot_manager.HandleMOTDataGroup(data_ar);
usleep(1000000);
                    }
                    */
                    //rv = parse_msc(xdata->data, xdata->len);
                    // please have a look at https://www.etsi.org/deliver/etsi_en/300400_300499/300401/02.01.01_60/en_300401v020101p.pdf how to parse the MSC data.
                    //if (rv == 1)
                        //emit finished();

                }
                /*
                else if (xdata->type == DLS) {
                    qDebug() << "DLS";
                    //if (xdata->len>2) {
                        //if (!(xdata->data[0] & 0x10)) {
//                            qDebug() << "data.status" << data.status;
//                            qDebug() << "xdata->status" << xdata->status;
//                            qDebug() << "data.type" << data.type;
//                            qDebug() << "xdata->type" << xdata->type;
//                            qDebug() << "data.len" << data.len;
//                            qDebug() << "xdata->len" << xdata->len;
//                            qDebug() << "data.bufq" << data.bufq;
//                            qDebug() << "xdata->bufq" << xdata->bufq;


                            std::vector<uint8_t> data_ar;
                            //QByteArray xdata_ar;
                            for(int i = 0; i < xdata->len; i++){

                                //printf("blub %d", i);
                                //qDebug() << xdata->data[i];
                                //xdata_ar.push_back(xdata->data[i]);
                                data_ar.push_back(xdata->data[i]);

                            }

                            mot_manager.HandleMOTDataGroup(data_ar);




//                            QString tmp;
//                            unsigned char *d=reinterpret_cast<unsigned char*>(&xdata->data[1]);
                            //for(int i=0;i<xdata->len-1;i++) {
//                                tmp+=QString::fromUtf16((ushort*)&ebuLatinToUnicode[d[i]], 1);
//                            }
//                            dls = tmp;
//                            emit dlsUpdated();
                        //}
                    //}

                }
*/


/*
    struct au {

        uint8_t status;
        uint8_t type;

        ssize_t ka;
        //void buffer;
        size_t bufferlen;

    } *au_data;

    au_data = (struct au *) malloc(sizeof(struct au));

    memset(&data, 0x0, sizeof(struct dab_service_data));
*/
    //qDebug() << "rv: " << rv;
/*
    int fd;
    fd = net_open(strdup("/dev/dab0"), O_RDWR);
    //net_close(fd);

    //ssize_t net_read(int fd, void *buf, size_t nbytes);

    ssize_t blub;
    //void *buffer = 0;
    //size_t buflen = 0;

    blub = __net_read(fd, &buffer, buflen);


    //au_data->ka = __net_read(fd)

//    qDebug() << "ssize_t blub" << blub;
//    qDebug() << "buffer" << buffer;
//    qDebug() << "buflen" << buflen;

//    xdata->type = data.type;
//    xdata->len = data.len;

*/

/*
rv = net_ioctl(radioFD, DAB_GET_DIGITAL_SERVICE_DATA, &data);

    if (rv == 0) {
        if (data.len > 0) {
            xdata->status = 0;
            //xdata->type = 0;
            xdata->type = data.type;
            xdata->len = data.len;


//qDebug() << "xdata->type: " << xdata->type;
//qDebug() << "data.type: " << data.type;
//qDebug() << "data.len: " << data.len;
//qDebug() << "xdata->len: " << xdata->len;


            rv = net_ioctl(radioFD, DAB_GET_DIGITAL_SERVICE_DATA, xdata);
            //qDebug()<<"rv: " << rv;
            if (rv == 0){
                if (xdata->type == MOT) {
                    //void DecoderAdapter::ProcessPAD(const uint8_t *xpad_data, size_t xpad_len, bool exact_xpad_len, const uint8_t *fpad_data)
                    //padDecoder.Process(xpad_data, xpad_len, exact_xpad_len, fpad_data);
                    //padDecoder.Process(xdata->data, xdata->len, true, data.data);

                    //observer->ProcessPAD(data + pad_start, pad_len - FPAD_LEN, true, data + pad_start + pad_len - FPAD_LEN);







                    qDebug() << "MOT started";

                    qDebug() << "xdata.len" << xdata->len;
                    qDebug() << "xdata.bufq" << xdata->bufq;
                    qDebug() << "xdata.data" << xdata->data;
                    qDebug() << "xdata.status" << xdata->status;
                    qDebug() << "xdata.type" << xdata->type;
                    //rv = parse_msc(xdata->data, xdata->len);
                    // please have a look at https://www.etsi.org/deliver/etsi_en/300400_300499/300401/02.01.01_60/en_300401v020101p.pdf how to parse the MSC data.

                     //mMOTEntity.GetData();

                    //if (!(xdata->data[0] & 0x10)) {
                        QString tmp;
                        QString tmp_utf;
                        QString dls;
                        QString dls_utf;
                        QString tmp_ucs2;
                        QString tmp_ucs2_2;
                        QString tmp_utf8;
                        int gr = 0;

                        std::vector<uint8_t> tmp_vec;

                        //unsigned char *d=reinterpret_cast<unsigned char*>(&xdata->data[1]);
                        //qDebug() << " char *d: " << *d;
                        //qDebug()<< "len: " << xdata->len;
                        //for(int i=0;i<xdata->len-1;i++) { //?????????? warum -1
                        //for(int i=0;i<xdata->len;i++) {
                            //tmp+=QString::fromUtf16((ushort*)&ebuLatinToUnicode[d[i]], 1);
                            //tmp_ucs2 =QString::fromUtf16((ushort*)&ebuLatinToUcs2[d[i]], 1);
                            //tmp_utf8 =QString::fromUtf16((ushort*)&utf8_encoded_EBU_Latin[d[i]], 1);
                            //tmp_ucs2_2 =QString::fromUtf16((ushort*)&ebuLatinToUcs2_2[d[i]], 1);
                            //tmp_utf =QString::fromUtf16((ushort*)d[i], 1);
                            //tmp+=d[i];
                            //unsigned char *d=reinterpret_cast<unsigned char*>(&xdata->data[i]);
                            //qDebug() << *d;

                            //tmp_vec.push_back(*d);

                            //tmp+=&xdata->data[i];
                            //qDebug()<< "i d[i] ucs2 utf8 ucs2_2" << i << d[i] << tmp_ucs2 << tmp_utf8 << tmp_ucs2_2;
                        //}

                        //qDebug() << "size data" << tmp_vec.size();
                        //qDebug() <<"mot d" << tmp_vec;


                    //}




//                    std::vector<uint8_t> result(size);
//                    size_t offset = 0;

                    // concatenate all segments
//                    for(int i = 0; i <= last_seg_number; i++) {
//                        seg_t& seg = segs[i];
//                        memcpy(&result[offset], &seg[0], seg.size());
//                        offset += seg.size();
//                    }

                    //return result;


                    //if (rv == 1)
                        //emit finished();

                } else if (xdata->type == DLS) {
                    qDebug() << "DLS started";
                    if (xdata->len>2) {
                        qDebug()<< "len: " << xdata->len;
                        if (!(xdata->data[0] & 0x10)) {
                            QString tmp;
                            QString tmp_utf;
                            QString dls;
                            QString dls_utf;
                            QString tmp_ucs2;
                            QString tmp_ucs2_2;
                            QString tmp_utf8;

                            //unsigned char *d=reinterpret_cast<unsigned char*>(&xdata->data[1]);
                            //qDebug() << " char *d: " << *d;
                            //for(int i=0;i<xdata->len-1;i++) {
                                //tmp+=QString::fromUtf16((ushort*)&ebuLatinToUnicode[d[i]], 1);
                                //tmp_ucs2 =QString::fromUtf16((ushort*)&ebuLatinToUcs2[d[i]], 1);
                                //tmp_utf8 =QString::fromUtf16((ushort*)&utf8_encoded_EBU_Latin[d[i]], 1);
                                //tmp_ucs2_2 =QString::fromUtf16((ushort*)&ebuLatinToUcs2_2[d[i]], 1);
                                //tmp_utf =QString::fromUtf16((ushort*)d[i], 1);
                                //tmp+=d[i];
                                //qDebug()<< "i d[i] ucs2 utf8 ucs2_2" << i << d[i] << tmp_ucs2 << tmp_utf8 << tmp_ucs2_2;
                            //}
                            //dls = tmp;
                            //dls_utf = tmp_utf;
                            //qDebug() << "dls string: " << dls;
                            //qDebug() << "dls utf string: " << dls_utf;
                            //emit dlsUpdated();
                        }
                    }
                }


            }
        }
    }
*/
//}
