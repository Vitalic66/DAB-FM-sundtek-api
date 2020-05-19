#include "dabdata.h"

DabData::DabData(QObject *parent) : QObject(parent)
{

}

void DabData::receive_dab_data()
{
    /*
    //sundtek added
    struct dab_ensemble_info ens;
    //self added
    struct dab_service_data data;

    struct xdata{

        int status;
        int type;
        int len;
        char data[512];

    } *xdata;

    //QObject *xdata;


    int rv;
    int radioFD = net_open("/dev/dab0", O_RDWR);

    memset(&ens, 0x0, sizeof(struct dab_ensemble_info));
    memset(&data, 0x0, sizeof(struct dab_service_data));
    data.status = 1;

    rv = net_ioctl(radioFD, DAB_GET_DIGITAL_SERVICE_DATA, &data);
    qDebug() << "rv: " << rv;
    qDebug() << "data.len: " << data.len;
    if (rv == 0) {
        if (data.len > 0) {
            xdata->status = 0;
            xdata->type = 0;
            xdata->len = data.len;



            rv = net_ioctl(radioFD, DAB_GET_DIGITAL_SERVICE_DATA, xdata);
            if (rv == 0) {
                if (xdata->type == MOT) {
                    //rv = parse_msc(xdata->data, xdata->len);
                    // please have a look at https://www.etsi.org/deliver/etsi_en/300400_300499/300401/02.01.01_60/en_300401v020101p.pdf how to parse the MSC data.
                    if (rv == 1)
                        emit finished();

                } else if (xdata->type == DLS) {
                    if (xdata->len>2) {
                        if (!(xdata->data[0] & 0x10)) {
                            QString tmp;
                            unsigned char *d=reinterpret_cast<unsigned char*>(&xdata->data[1]);
//                            for(int i=0;i<xdata->len-1;i++) {
//                                tmp+=QString::fromUtf16((ushort*)&ebuLatinToUnicode[d[i]], 1);
//                            }
//                            dls = tmp;
                            emit dlsUpdated();
                        }
                    }
                }
            }
        }
    }
    */
}
