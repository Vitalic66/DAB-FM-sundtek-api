#include "scan.h"
//#include <QVector>
#include <QDebug>
#include <QStringLiteral>

QVector<QVector<QString>> g_dab_vec_vec;

Scan::Scan(QObject *parent) : QObject(parent)
{
mStop_dab_scan = false;
QVector<QVector<QString>> dab_vec_vec;

}

void Scan::stop_scan()
{
   mStop_dab_scan = true;
}

int Scan::dab_scan_wrapped(){

    //mStop_dab_scan = false;
//while(!mStop_dab_scan){
    //forever{

        //if(mStop_dab_scan) return 0;

    //if(mStop_dab_scan){
    //    return 0;
    //} else {
    int devfd = -1;
    int console = -1;
    int running = -1;

    Scan::media_scan_dabfrequencies("/dev/dab0",devfd,console,running);
    //}
//}

//return dab_vec_vec;
}

int Scan::media_scan_dabfrequencies(char *device, int devfd, int console, int running) {

//    forever{

//        if(mStop_dab_scan) break;

        int fd;
        int rv;
        int nlen;
        char tmp[30];
        //clear vectors before new scan
        dab_vec_vec.clear();
        dab_name_vec.clear();
        dab_sid_vec.clear();

        g_dab_vec_vec.clear();

//        for(int i = 0;i < g_dab_vec_vec.size(); i++){

//            if(!g_dab_vec_vec[i][3].contains("fav")){
//                g_dab_vec_vec.remove(i);
//            }

//        }

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
//while(!mStop_dab_scan){
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

                                            //QString freq;
                                            //char freq_char = static_cast<char>(dab_frequency_list[parameters.scan_index].freq*1000);
                                            //qDebug() << "scan_index: " << dab_frequency_list[parameters.scan_index].freq;
                                            //qDebug() << "freq_char: " << freq_char;
                                            //freq.append(freq_char);
                                            //qDebug() << "freq: " << freq;
                                            //QString freq = ;
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
                                            qDebug() << "char buffer: " << buffer;

                                            //QVector<int> dab_sid_vec_back {Scan::media_scan_dabservices(buffer)};

                                            Scan::media_scan_dabservices(buffer);
//dab_vec_vec.clear();
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


                                            qDebug() << "dab_vec_after_for" << dab_vec;
                                            qDebug() << "dab_vec_vec_after_for" << dab_vec_vec;
                                            qDebug() << "g_dab_vec_vec_after_for" << g_dab_vec_vec;




                                            //qDebug() << "conv: " << conv;
                                            //dab_vec.push_back(freq);
                                            //qDebug() << "dab_vec: " << dab_vec;
                                            //dab_vec_vec.push_back(dab_vec);
                                            //qDebug() << "dab_vec_vec: " << dab_vec_vec;
                                            //fprintf(stdout, "[LOCKED]\n");





                                    }
                                    break;
                            }
                            case DAB_SCAN_SEARCHING:
                                    usleep(10000);
                                    break;
                            case DAB_SCAN_COMPLETE:
                            //case 8:
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
                            qDebug() << "size of dab list: " << dab_frequency_list_size;
                            qDebug() << "scan index: " << current_scan_index;

                            prog_bar_dab = (current_scan_index *100) / (dab_frequency_list_size -1);
                            qDebug() << "prog bar value: " << prog_bar_dab;

                            emit progress_scan_dab(prog_bar_dab);
                            emit enable_buttons(false);

                            //while(prog_bar_dab != 100){
                            //if(prog_bar_dab == )

                            //}


                            if (console>=0 && running == 0)
                                    break;

                            if(prog_bar_dab == 100){
                                emit enable_buttons(true);
                                emit write_to_file();
                                emit finished_scan();
                                mStop_dab_scan = true;
                            }


                            qDebug() << "mStop_scan_dab: " << mStop_dab_scan;

//}
                    //} while (parameters.status != DAB_SCAN_COMPLETE);
                    } while (parameters.status != DAB_SCAN_COMPLETE && mStop_dab_scan == false);
//}
                    qDebug() << "dabvecvec after scan: " << dab_vec_vec;
                    qDebug() << "g_dabvecvec after scan: " << g_dab_vec_vec;

                    if (devfd == -1)
                            net_close(fd);
            }





//        for(int i = 0; i < dab_vec_vec.size();i++){
//            QVector<QString> dab_trans_vec_vec {Scan::media_scan_dabservices("dev/dab0")};

//        }

// }
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
