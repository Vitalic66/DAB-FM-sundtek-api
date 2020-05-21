#include "scan.h"
//#include <QVector>
#include <QDebug>
//#include <QStringLiteral>
#include <QMutexLocker>

QVector<QVector<QString>> g_dab_vec_vec;
QVector<QVector<QString>> g_fm_vec_vec;

Scan::Scan(QObject *parent) : QObject(parent)
  //Scan::Scan(QObject *parent) : QThread(parent)
{
    //mStop_dab_scan = false;
    //mStop_fm_scan = false;

}

  //new2

//void Scan::run()
void Scan::fm_scan_wrapper()
{
/*
      for(int i = 0; i < 10000; i++){

          //QMutex mutex;
          //mutex.lock();
          //if(this->Stop) break;
          //mutex.unlock();

          emit NumberChanged(i);

          this->msleep(100);
      }
*/

    //qDebug() << "fm scan wrapper called";
        int devfd = -1;
    //    int console = -1;
    //    int running = -1;

        Scan::media_scan_fm_frequencies("/dev/radio0",devfd);
}

int Scan::media_scan_fm_frequencies(char *device, int devfd) {
//qDebug() << "media scan fm called";
        int fd;
        int rv;
        int nlen;
        char tmp[30];
        g_fm_vec_vec.clear();

        int prog_bar_fm = 0;
        //int prog_bar_act_freq = 87000;

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
    //                                if (console>=0) {
    //                                        rv=write(console, "[FINISHED]\n", 11);
    //                                } else {
                                            fprintf(stdout, "\nScan completed\n");

                                            //test8
                                            mStop_fm_scan = true;
                                    //}
                                    break;
                            }



                        //station = station + 1;
                        //qDebug() << "i in fm scan: " << station;

                        }
//                        if (console>=0 && running == 0)
//                                break;


                        //act freq - 108 + 21
                        //divisor 21

                        //test8
                        prog_bar_fm = (parameters.READFREQ - 87000) * 100 / 21000;

                        //prog_bar_fm = (parameters.READFREQ *100) / 10800 ;
                        qDebug() << "prog_bar_fm: " << prog_bar_fm;

                        //test8
                        emit progress_scan_fm(prog_bar_fm);
                        workDone = prog_bar_fm;
                        emit sendProgress(workDone);
                        //emit enable_buttons(false);
                        //emit show_progbar_fm(true);

                        qDebug() << "mStop_fm_scan: " << mStop_fm_scan;

                //test6
                } while (parameters.status != FM_SCAN_COMPLETE && mStop_fm_scan == false);
                //} while (parameters.status != FM_SCAN_COMPLETE);

                //test5
                //emit enable_buttons(true);
                //emit show_progbar_fm(false);

                //emit write_to_file();

                //test5
                emit finished_scan_fm();

                mStop_fm_scan = false;


                qDebug() << "g_fm_vec_vec: " << g_fm_vec_vec;

                //test4
                if (devfd == -1)
                        net_close(fd);
        }
        //test6
        //return 0;

        //test7 (after test 6 return deactivated -> invalid pointer
        return 0;
}



//new threading ################################################################################################################################


/*
void Scan::DoSetup(QThread &cThread)
{
    connect(&cThread,SIGNAL(started()),this,SLOT(DoWork()));
}

void Scan::DoWork()
{
    for (int i = 0; i < 10000; i++){
        qDebug() << i;
    }
}
*/



/*
void Scan::stop_scan_dab()
{
   mStop_dab_scan = true;
}

void Scan::stop_scan_fm()
{
   mStop_fm_scan = true;
}

int Scan::dab_scan_wrapped(){

    int devfd = -1;
    int console = -1;
    int running = -1;

    Scan::media_scan_dabfrequencies("/dev/dab0",devfd,console,running);
}

int Scan::fm_scan_wrapped(){
//qDebug() << "fm scan wrapper called";
    int devfd = -1;
//    int console = -1;
//    int running = -1;

    Scan::media_scan_fm_frequencies("/dev/radio0",devfd);
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
                struct dab_frequency dabf;
                struct dab_tuner dabt;
                int i;
                int e;
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
                //qDebug() << "foo: " << foo;
                //qDebug() << "size struct" << dab_frequency_list->channel.s

                do {

                    net_ioctl(fd, DAB_SCAN_NEXT_FREQUENCY, &parameters);
                    if (current_scan_index != parameters.scan_index) {
                            if (console>=0) {
                                    sprintf(tmp, "%s %d\n", dab_frequency_list[parameters.scan_index].channel, dab_frequency_list[parameters.scan_index].freq*1000);
                                    rv=write(console, tmp, nlen);
                            } else {
                                    fprintf(stdout, "%s %d\n", dab_frequency_list[parameters.scan_index].channel, dab_frequency_list[parameters.scan_index].freq*1000);
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
                                //qDebug() << "char buffer: " << buffer;

                                //QVector<int> dab_sid_vec_back {Scan::media_scan_dabservices(buffer)};

                                Scan::media_scan_dabservices(buffer);

                                for(int j = 0; j < dab_sid_vec.size(); j++){

                                    //dab_vec.push_back(dab_sid_vec.at(j)); //service_name
                                    //dab_vec.push_back(dab_frequency_list[parameters.scan_index].freq*1000); //freq

                                    dab_vec.clear();
                                    dab_vec.push_back(dab_name_vec.at(j)); //service_name
                                    dab_vec.push_back(freq); //freq
                                    dab_vec.push_back(dab_sid_vec.at(j)); //service_id
                                    dab_vec.push_back(""); //placeholder for fav
                                    //dab_vec_vec.push_back(dab_vec);
                                    g_dab_vec_vec.push_back(dab_vec);

                                }

                                dab_name_vec.clear();
                                dab_sid_vec.clear();


                                //qDebug() << "dab_vec_after_for" << dab_vec;
                                //qDebug() << "dab_vec_vec_after_for" << dab_vec_vec;
                                //qDebug() << "g_dab_vec_vec_after_for" << g_dab_vec_vec;
                            }
                            break;
                        }
                        case DAB_SCAN_SEARCHING:
                                usleep(10000);
                                break;
                        case DAB_SCAN_COMPLETE:

                        {
                                if (console>=0) {
                                        rv=write(console, "[FINISHED]\n", 11);
                                } else {
                                        fprintf(stdout, "\nScan completed\n");
                                }
                                break;
                        }
                    }

                    current_scan_index = parameters.scan_index;
                    //qDebug() << "status: " << parameters.status;
                    //qDebug() << "size of dab list: " << dab_frequency_list_size;
                    //qDebug() << "scan index: " << current_scan_index;

                    //test8
                    //prog_bar_dab = (current_scan_index * 100) / (dab_frequency_list_size -1);

                    //qDebug() << "prog bar value: " << prog_bar_dab;

                    //test8
//                    emit progress_scan_dab(prog_bar_dab);
//                    emit enable_buttons(false);
//                    emit show_progbar_dab(true);

                    if (console>=0 && running == 0)
                            break;

                    //test8
                    //if(prog_bar_dab == 100){

                        //test5
                        //emit enable_buttons(true);
                        //emit show_progbar_dab(false);
                        //emit write_to_file();
                        //emit finished_scan();

                        //test8
                        //mStop_dab_scan = true;
                    //}

                    //qDebug() << "mStop_scan_dab: " << mStop_dab_scan;

                //test6
                //} while (parameters.status != DAB_SCAN_COMPLETE && mStop_dab_scan == false);
                } while (parameters.status != DAB_SCAN_COMPLETE);

            //qDebug() << "g_dabvecvec after scan: " << g_dab_vec_vec;

            //test4
//            if (devfd == -1)
//                    net_close(fd);
        }
    //test6
    //return 0;
    //test7 (after test 6 return deactivated -> invalid pointer
    return 0;

}

int Scan::media_scan_dabservices(char *device) {
        int fd;
        int rv;
        int i=0;

        //QVector<QString> dab_trans_vec;
        fd = net_open(device, O_RDWR);
        if (fd>=0) {
                struct dab_service service;
                printf("Service Name, Service ID, Component ID\n");
                while(1) {

                        service.id=i++;
                        rv = net_ioctl(fd, DAB_GET_SERVICE, &service);
                        if (rv == -1)
                                break;
                        printf("%16s\t0x%x\t0x%x\n", service.service_name, service.sid, service.comp[0]);

                        qDebug() << "service_name: " << service.service_name;

                        QString service_name_conv;
                        //char c;
                        for(int j = 0; j < 17; j++){
                            const char c = static_cast<char>(service.service_name[j]);

                            service_name_conv.append(c);
                        }

                        //service_name_conv = service_name_conv.toLocal8Bit();

                        if(service_name_conv.contains('\0')){
                            service_name_conv = service_name_conv.remove('\0');
                        }

                        //remove whitespaces
                        service_name_conv = service_name_conv.simplified();

                        if(service_name_conv.contains("\uFFD9")){
                            service_name_conv = service_name_conv.replace("\uFFD9", "_");
                        }

                        qDebug() << "service_name_conv: " << service_name_conv;


                        //uint8_t char to QString
                        //QString dab_service_name(QChar(service.service_name[16]));
                        //QString dab_service_name = QString::number(service.service_name);
                        //qDebug() << "dab_service_name conv: " << dab_service_name;
                        //QString dab_service_name(QChar());

                        //char irgendwas = static_cast<char>(service.service_name);
                        //QString dab_sid_str(QChar(service.sid)); //chinesische zeichen
                        //qDebug() << "dab_sid_str: " << dab_sid_str;

//                        QString dab_sid_hex = QString("0x%1").arg(service.id, 8, 16, QLatin1Char( '0' ));
//                        qDebug()<< "dab sid hex: " << dab_sid_hex;

                        QString dab_sid;
                        //if(service.sid < 10000){
                        dab_sid = QString::number(service.sid, 16); //int to hex to string
                        qDebug() << "dab_sid conv: " << dab_sid;
                        //}
                        //uint32_t sid = service.sid;
                        //QString dab_service_name = service.service_name;
                        //dab_sid_vec.push_back(service.service_name);

                        if(dab_sid.length() == 4){  //filter non radio staions
                        dab_name_vec.push_back(service_name_conv);
                        dab_sid_vec.push_back(dab_sid);
                        }
                        qDebug() << "dab_sid_vec: " << dab_sid_vec;


                        //return sid;
                }
                net_close(fd);
        }

        //return dab_sid_vec;

        return 0;
}

//int Scan::media_scan_fm_frequencies(char *device, int devfd, int console, int running) {
int Scan::media_scan_fm_frequencies(char *device, int devfd) {
//qDebug() << "media scan fm called";
        int fd;
        int rv;
        int nlen;
        char tmp[30];
        g_fm_vec_vec.clear();

        int prog_bar_fm = 0;
        //int prog_bar_act_freq = 87000;

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
    //                                if (console>=0) {
    //                                        rv=write(console, "[FINISHED]\n", 11);
    //                                } else {
                                            fprintf(stdout, "\nScan completed\n");

                                            //test8
                                            //mStop_fm_scan = true;
                                    //}
                                    break;
                            }



                        //station = station + 1;
                        //qDebug() << "i in fm scan: " << station;

                        }
//                        if (console>=0 && running == 0)
//                                break;


                        //act freq - 108 + 21
                        //divisor 21

                        //test8
                        //prog_bar_fm = (parameters.READFREQ - 87000) * 100 / 21000;

                        //prog_bar_fm = (parameters.READFREQ *100) / 10800 ;
                        //qDebug() << "prog_bar_fm: " << prog_bar_fm;

                        //test8
                        //emit progress_scan_fm(prog_bar_fm);
                        //emit enable_buttons(false);
                        //emit show_progbar_fm(true);

                        //qDebug() << "mStop_fm_scan: " << mStop_fm_scan;

                //test6
                //} while (parameters.status != FM_SCAN_COMPLETE && mStop_fm_scan == false);
                } while (parameters.status != FM_SCAN_COMPLETE);

                //test5
                //emit enable_buttons(true);
                //emit show_progbar_fm(false);

                //emit write_to_file();

                //test5
                //emit finished_scan_fm();


                qDebug() << "g_fm_vec_vec: " << g_fm_vec_vec;

                //test4
//                if (devfd == -1)
//                        net_close(fd);
        }
        //test6
        //return 0;

        //test7 (after test 6 return deactivated -> invalid pointer
        return 0;
}
*/
