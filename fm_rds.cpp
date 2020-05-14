#include "fm_rds.h"

FM_rds::FM_rds(QObject *parent) : QObject(parent)
{
    mStop = false;
}

//void MyJob::start(QString name)
//{
//    mStop = false;

//    for(int i = 0; i < 99999; i++)
//    {
//        if(mStop) return;
//        qDebug() << "From thread: " << i;
//        emit on_number(name, i);
//        QThread::currentThread()->msleep(100);
//    }
//}

void FM_rds::stop()
{
   mStop = true;
}

//int MyJob::rds(const char *device){
int FM_rds::rds(){

    mStop = false;

    int i;
    struct rds_data *rdsd;
    struct fm_rds_data data;
    int rdsfd;

    if (strstr("/dev/radio0", "radio")) {
            rdsfd = net_open("/dev/radio0", O_RDWR);
            if (rdsfd >= 0) {
                    uint8_t brkstat = 0;
                    uint8_t radiotext[150];
                    uint8_t program[9];
                    uint8_t print_program[9];
                    uint8_t rdsdata[8];
                    memset(radiotext, 0x0, 150);
                    memset(program, 0x0, 9);
                    memset(print_program, 0x0, 9);
                    int x=0;
                    while(1) {
                            net_ioctl(rdsfd, FM_RDS_STATUS, &data);

                            QString test;
                            //char irgendwas;
                            QStringList irgendwas_list;
                            QString irgendwas_line;


                            if (data.rdssync) {
                                    if (memcmp(rdsdata, data.data, 8)==0)
                                            continue;
                                    memcpy(rdsdata, data.data, 8);
                                    if ((data.data[2]>>4) == 0) {
                                            x = (data.data[3]&0x3)*2;
                                            program[x++] = data.data[6]&0x7f;
                                            program[x++] = data.data[7]&0x7f;
                                    }
                                    if ((data.data[2]>>4) == 2) {
                                            x = (data.data[3]&0x0f)*4;
                                            if ((data.data[3]&0x10) != brkstat) {
                                                    brkstat = data.data[3] & 0x10;
                                                    memset(radiotext, 0x0, 150);
                                            }
                                            radiotext[x++]=(data.data[4]&0x7f);
                                            radiotext[x++]=(data.data[5]&0x7f);
                                            radiotext[x++]=(data.data[6]&0x7f);
                                            radiotext[x++]=(data.data[7]&0x7f);
                                    }
                                    usleep(10000);

                                    if (isprint(program[0]) && isprint(program[1])) {
                                            memcpy(print_program, program, 9);
                                    }

                                    //printf("PROGRAM: %s\n", print_program);

                                    //char irgendwas = static_cast<char>(print_program);

                                    //emit rds_out(print_program);

                                    //printf("RADIOTEXT: ");

                                    for (i=0;i<64;i++) {

                                        if(mStop) return 0;

                                        if(radiotext[i] != 0){

                                        //char irgendwas = static_cast<char>(radiotext[i]);
                                        //test.append(irgendwas);
                                        //qDebug() << test;
                                                }
                                           switch(radiotext[i]) {
                                                    case 0x19:
                                                            printf("ue");
                                                            //qDebug() << radiotext[i];
                                                            break;
                                                    default:
                                                            //printf("%c", radiotext[i]);
                                                            qDebug() << i;
                                                            //QString buch = radiotext[i];
                                                            //ui->label_2->setText(buch);
                                                             //qDebug() << radiotext[i];

                                                            char irgendwas = static_cast<char>(radiotext[i]);
                                                            //QChar char(radiotext[i]);
                                                            //qDebug() << irgendwas;

                                                            test.append(irgendwas);

                                                            //qDebug() << test;


                                                             //if(irgendwas != "\u0000"){
//                                                                for(int j = 0; j < 20; j++){

//                                                                    irgendwas_line.append(irgendwas);
//                                                                }
                                                            //}
                                                            //irgendwas_list.append(irgendwas_line);
                                                            //ui->label_2->setText(test);
                                                            //qDebug() << irgendwas_list;


                                //QString buch = QString::fromLocal8Bit();
                                // const char* dst = reinterpret_cast<const char*>(radiotext[i]);

                                // test.append(dst);

                                 //qDebug() << test;

                                //QTextStream stream = dst;
                                 //QString buch;
                                 //buch = dst;
                                                            //ui->label_2->setText(dst);
                                                            //qDebug() << dst;
                                     //QString buch = dst;
                                            }
                                           emit rds_out(test);

                                    }
                                    //printf("\n");

                                    //ui->label_2->setText(test);

                                    fflush(stdout);
                            }
                            //QThread::currentThread()->msleep(5000);
                    }
            }
    net_close(rdsfd);
    }
    return 0;
}
