#include "scan.h"
//#include <QVector>
#include <QDebug>
//#include <QStringLiteral>
#include <QMutexLocker>

QVector<QVector<QString>> g_dab_vec_vec;
QVector<QVector<QString>> g_fm_vec_vec;

Scan::Scan(QObject *parent) : QObject(parent)

{
    mStop_dab_scan = false;
    mStop_fm_scan = false;

}

void Scan::stop_scans()
{
    mStop_dab_scan = true;
    mStop_fm_scan = true;
}

void Scan::fm_scan_wrapper()
{
    int devfd = -1;

    media_scan_fm_frequencies(strdup("/dev/radio0"),devfd);
}

int Scan::media_scan_fm_frequencies(char *device, int devfd) {

        int fd;
        g_fm_vec_vec.clear();

        int prog_bar_fm = 0;


        if (devfd>=0)
                fd = devfd;
        else {
                printf("opening device: %s\n", device);
                fd = net_open(device, O_RDWR);
        }

        if (fd>=0) {
//                int i;
//                int e;
//                int current_scan_index=-1;
                //qDebug() << "current scan index: " << current_scan_index;
                struct fm_scan_setup setup;

                struct fm_scan_parameters parameters;
                memset(&parameters, 0x0, sizeof(struct fm_scan_parameters));
                memset(&setup, 0x0, sizeof(struct fm_scan_setup));
                //qDebug() << "memset: " << memset(&setup, 0x0, sizeof(struct fm_scan_setup));
                //printf("SCAN SETUP\n");
                net_ioctl(fd, FM_SCAN_SETUP, &setup);

                int station = 0;

                do {

                        //qDebug() << "READFREQ: " << parameters.READFREQ;
                        net_ioctl(fd, FM_SCAN_NEXT_FREQUENCY, &parameters);
        //              printf("LOCK STAT: %x - %d - %d\n", parameters.status, parameters.VALID, parameters.READFREQ);

                        switch(parameters.status) {
                            case FM_SCAN_LOCKED:
                            {
                                QString freq = QString::number(parameters.READFREQ*1000);

                                float mhz_float = freq.toFloat() / 1000000;
                                //qDebug() << "mhz_float: " << mhz_float;
                                QString mhz = QString::number(mhz_float);

                                //qDebug() << "mhz: " << mhz;
                                if(!mhz.contains(".")){
                                    mhz = mhz + ".0";
                                }


                                station++;
                                fm_vec.clear();
                                fm_vec.push_back("Station " + QString::number(station) + "@" + mhz + "MHz"); //Station x
                                fm_vec.push_back(freq); //frequency
                                fm_vec.push_back(""); //placeholder for fav

                                //qDebug() << "fm_vec: " << fm_vec;

                                g_fm_vec_vec.push_back(fm_vec);
                                //qDebug() << "g_fm_vec_vec: " << g_fm_vec_vec;

//                                }
                                break;
                            }
                            case FM_SCAN_SEARCHING:
                                    usleep(10000);
                                    break;
                            case FM_SCAN_COMPLETE:
                            {

                                //fprintf(stdout, "\nScan completed\n");
                                mStop_fm_scan = true;

                                break;
                            }

                        }

                        prog_bar_fm = (parameters.READFREQ - 87000) * 100 / 21000;

                        //qDebug() << "prog_bar_fm: " << prog_bar_fm;


                        emit progress_scan_fm(prog_bar_fm);
                        emit enable_buttons(false);
                        emit show_progbar_fm(true);

                        //qDebug() << "mStop_fm_scan: " << mStop_fm_scan;

                } while (parameters.status != FM_SCAN_COMPLETE && mStop_fm_scan == false);

                emit enable_buttons(true);
                emit show_progbar_fm(false);
                emit finished_scan_fm();

                mStop_fm_scan = false; //reset for next scan


                //qDebug() << "g_fm_vec_vec: " << g_fm_vec_vec;

                if (devfd == -1)
                        net_close(fd);
        }
        return 0;
}

void Scan::dab_scan_wrapper()
{
    int devfd = -1;
    int console = -1;
    int running = -1;

    media_scan_dabfrequencies(strdup("/dev/dab0"),devfd,console,running);
}

int Scan::media_scan_dabfrequencies(char *device, int devfd, int console, int running)
{
    int fd;
    int rv;
    int nlen;
    char tmp[30];
    dab_name_vec.clear();
    dab_sid_vec.clear();
    g_dab_vec_vec.clear();

    int prog_bar_dab = 0;

        if (devfd>=0)
                fd = devfd;
        else
                fd = net_open(device, O_RDWR);

        if (fd>=0) {
                int current_scan_index=-1;
                struct dab_scan_setup setup;
                struct dab_scan_parameters parameters;
                memset(&parameters, 0x0, sizeof(struct dab_scan_parameters));
                memset(&setup, 0x0, sizeof(struct dab_scan_setup));
                net_ioctl(fd, DAB_SCAN_SETUP, &setup);

                static struct {
                        char channel[4];
                        uint32_t freq;
                } dab_frequency_list[]={
                        {"5A", 174928},
                        {"5B",176640},
                        {"5C",178352},
                        {"5D",180064},
                        {"6A",181936},
                        {"6B",183648},
                        {"6C",185360},
                        {"6D",187072},
                        {"7A",188928},
                        {"7B",190640},
                        {"7C",192352},
                        {"7D",194064},
                        {"8A",195936},
                        {"8B",197648},
                        {"8C",199360},
                        {"8D",201072},
                        {"9A",202928},
                        {"9B",204640},
                        {"9C",206352},
                        {"9D",208064},
                        {"10A",209936},
                        {"10B",211648},
                        {"10C",213360},
                        {"10D",215072},
                        {"11A",216928},
                        {"11B",218640},
                        {"11C",220352},
                        {"11D",222064},
                        {"12A",223936},
                        {"12B",225648},
                        {"12C",227360},
                        {"12D",229072},
                        {"13A",230748},
                        {"13B",232496},
                        {"13C",234208},
                        {"13D",235776},
                        {"13E",237448},
                        {"13F",239200}
                };

                int dab_frequency_list_size =sizeof(dab_frequency_list)/sizeof(dab_frequency_list[0]);

                do {

                    net_ioctl(fd, DAB_SCAN_NEXT_FREQUENCY, &parameters);
                    if (current_scan_index != parameters.scan_index) {
                            if (console>=0) {
                                    //sprintf(tmp, "%s %d\n", dab_frequency_list[parameters.scan_index].channel, dab_frequency_list[parameters.scan_index].freq*1000);
                                    rv=write(console, tmp, nlen);
                            } else {
                                    //fprintf(stdout, "%s %d\n", dab_frequency_list[parameters.scan_index].channel, dab_frequency_list[parameters.scan_index].freq*1000);
                                    fflush(stdout);
                            }
                    }

                    switch(parameters.status) {
                        case DAB_SCAN_LOCKED:
                        {
                            if (console>=0) {
                                rv=write(console, "[LOCKED]\n", 9);
                            } else {
                                QString freq = QString::number(dab_frequency_list[parameters.scan_index].freq*1000);
                                uint freq_as_int = freq.toUInt();
                                //bool ok;

                                //tune to dab frequency locked
                                mTune.set_dab_freq(fd, freq_as_int);
                                //start scan dab services

                                //QString to char
                                QString string = "/dev/dab0";

                                QByteArray array = string.toLocal8Bit();
                                char* buffer = array.data();

                                media_scan_dabservices(buffer);

                                for(int j = 0; j < dab_sid_vec.size(); j++){

                                    dab_vec.clear();
                                    dab_vec.push_back(dab_name_vec.at(j)); //service_name
                                    dab_vec.push_back(freq); //freq
                                    dab_vec.push_back(dab_sid_vec.at(j)); //service_id
                                    dab_vec.push_back(""); //placeholder for fav
                                    g_dab_vec_vec.push_back(dab_vec);

                                }

                                dab_name_vec.clear();
                                dab_sid_vec.clear();
                            }
                            break;
                        }
                        case DAB_SCAN_SEARCHING:
                                usleep(10000);
                                break;
                        case DAB_SCAN_COMPLETE:

                        {
                                if (console>=0) {
                                        //rv=write(console, "[FINISHED]\n", 11);
                                } else {
                                        //fprintf(stdout, "\nScan completed\n");
                                }
                                break;
                        }
                    }

                    current_scan_index = parameters.scan_index;

                    prog_bar_dab = (current_scan_index * 100) / (dab_frequency_list_size -1);

                    emit progress_scan_dab(prog_bar_dab);
                    emit enable_buttons(false);
                    emit show_progbar_dab(true);

                    if (console>=0 && running == 0)
                            break;


                    if(prog_bar_dab == 100){

                        emit enable_buttons(true);
                        emit show_progbar_dab(false);
                        emit finished_scan_dab();


                        mStop_dab_scan = true;
                    }

                } while (parameters.status != DAB_SCAN_COMPLETE && mStop_dab_scan == false);
                mStop_dab_scan = false;

            if (devfd == -1)
                    net_close(fd);
        }

    return 0;

}

int Scan::media_scan_dabservices(char *device) {
        int fd;
        int rv;
        int i=0;

        fd = net_open(device, O_RDWR);
        if (fd>=0) {
                struct dab_service service;

                while(1) {

                        service.id=i++;
                        rv = net_ioctl(fd, DAB_GET_SERVICE, &service);
                        if (rv == -1)
                                break;

//                        QString service_name_conv;
                        std::vector<uint8_t> service_name_vec;
                        //char c;
                        for(int j = 0; j < 17; j++){

                            service_name_vec.push_back(service.service_name[j]); //append to uint8_t vector
                        }

                        QString qservice_name;

                        std::string service_name = toUtf8StringUsingCharset(service_name_vec.data(), (CharacterSet) 0, service_name_vec.size());
                        qservice_name = (QString::fromUtf8(service_name.c_str())).simplified();


                        if (qservice_name.length() == 0){

                            qservice_name = "NoServiceName";
                        }

                        QString dab_sid;

                        dab_sid = QString::number(service.sid, 16); //int to hex to string

                        if(dab_sid.length() == 4){  //filter non radio staions
                            dab_name_vec.push_back(qservice_name);
                            dab_sid_vec.push_back(dab_sid);
                        }
                }

        }
        net_close(fd);

        return 0;
}
