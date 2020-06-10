#include "fm_rds.h"
#include "globals.h"
//#include <QMutexLocker>

//bool g_mStop_rds;
FM_rds::FM_rds(QObject *parent) : QObject(parent),
    mStop_rds(false)

{

}

void FM_rds::start_rds_reading()
{
    //QMutexLocker locker(&lock);

   mStop_rds = false;
   prog_chars = ""; //emit empty !!!
   rds_chars = ""; //emit empty !!!
   emit rds_out(rds_chars); //emit empty !!!
   emit rds_prog_out(prog_chars); //emit empty !!!
   QThread::msleep(500);

   rds();

   //qDebug() << "mstop from function start: " << mStop_rds;
}



void FM_rds::stop_rds_reading()
{
   mStop_rds = true;

   //qDebug() << "mstop from function stop ...: " << mStop_rds;
}

int FM_rds::rds(){

    int i;
    //struct rds_data *rdsd;
    struct fm_rds_data data;
    int rdsfd;

    if (strstr("/dev/radio0", "radio")) {
            rdsfd = net_open(strdup("/dev/radio0"), O_RDWR);
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

                        QApplication::processEvents();

                        if(mStop_rds) break;

                            net_ioctl(rdsfd, FM_RDS_STATUS, &data);

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

                                    prog_chars.clear();
                                    for(int i = 0; i < 9; i++){
                                    prog_chars = prog_chars.append(static_cast<char>(program[i]));

                                    }
                                    if(prog_chars.contains('\0')){
                                        prog_chars = prog_chars.replace('\0', "");
                                    }
                                }

                                rds_chars.clear();

                                for (i=0;i<64;i++) {

                                    if(radiotext[i] != 0){

                                        switch(radiotext[i]) {
                                            case 0x19:
                                                    //printf("ue");

                                                    break;
                                            default:

                                                    char rds_single_char = 32; //init with space
                                                    if(radiotext[i] != 127){ //remove white rectangles (ascii del)

                                                        rds_single_char = static_cast<char>(radiotext[i]);

                                                    }

                                            rds_chars.append(rds_single_char);

                                            if(rds_chars.contains('\0')){
                                                rds_chars = rds_chars.replace('\0', "");
                                            }
                                        } //end switch
                                    } //end if
                                } //end for

                                emit rds_out(rds_chars);
                                emit rds_prog_out(prog_chars);

                                fflush(stdout);
                            } //end if
                  } //end while
            }
    net_close(rdsfd);
    }
    return 0;
}
