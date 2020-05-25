#include "fm_rds.h"
#include "globals.h"
//#include <QMutexLocker>

//bool g_mStop_rds;
FM_rds::FM_rds(QObject *parent) :

    //QThread(parent) //m2

    QObject(parent)
  ,
    mStop_rds(false),
    workSpeed(0),
    forsize(64)/*,
    prog_chars(),
    rds_chars()*/


{
    //mStop_rds = false;
    //g_mStop_rds = false;

}

/*
void FM_rds::run()
{
    for(int i=0;i<1000;i++){
        QMutex mutex;
        mutex.lock();
        if(this->mStop_rds) break;

        mutex.unlock();

        //emit...
    }
}
*/






/*
Worker::Worker(QObject *parent) :
    QObject(parent),
    active(false),
    progressToggleTerminal(false),
    workAmount(0),
    workDone(0),
    workSpeed(10)
*/

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
//void FM_rds::doWork()
//{
//    if(mStop_rds){
//        rds();
//    }
//}


void FM_rds::start_rds_reading()
{
    //QMutexLocker locker(&lock);

   mStop_rds = false;
   prog_chars = "";
   qDebug() << "prog_chars from function start: " << prog_chars;
   rds_chars = "";
   qDebug() << "rds_chars from function start: " << rds_chars;
   emit rds_out(rds_chars);
   emit rds_prog_out(prog_chars);
   QThread::msleep(500);

   //if(mStop_rds){
       //QMutexLocker locker(&lock);
       rds();
   //}
   //g_mStop_rds = true;
   qDebug() << "mstop from function start: " << mStop_rds;

   //rds();

//   prog_chars.clear();
//   rds_chars.clear();

   //FM_rds::rds();
}



void FM_rds::stop_rds_reading()
{
    //QMutexLocker locker(&lock);
   //mStop_rds = true;

    //QMutexLocker locker(&lock);
   mStop_rds = true;
   //prog_chars = "";
   //rds_chars = "";

   qDebug() << "mstop from function stop ...: " << mStop_rds;
}


void FM_rds::receiveWorkSpeed(int _workSpeed)
{
    //QMutexLocker locker(&lock);
    workSpeed = _workSpeed;
}

void FM_rds::receiveForSize(int _forsize)
{
    //QMutexLocker locker(&lock);
    forsize = _forsize;
}

//int MyJob::rds(const char *device){



int FM_rds::rds(){
//void FM_rds::run(){ //m2

    //qDebug() << "rds thread started";

    //qDebug() << "stop before while: " << mStop_rds;

    //mStop_rds = false;
    //g_mStop_rds = false;

    //while(!mStop_rds){

    int i;
    struct rds_data *rdsd;
    struct fm_rds_data data;
    int rdsfd;
    //mStop_rds;

//QMutexLocker locker(&lock);
    //if (strstr("/dev/radio0", "radio") && !mStop_rds) {
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
                    //QString prog_chars;
                    //QString rds_chars;

                    while(1) {

                  // while(!mStop_rds){
                    //do{

                        QApplication::processEvents();

                        if(mStop_rds) break;

                            net_ioctl(rdsfd, FM_RDS_STATUS, &data);

//QThread::msleep(workSpeed);

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

                                        //prog_chars = program[0]+program[1]+program[2];

                                        //if(prog_chars.length() < ){
                                        prog_chars.clear();
                                        for(int i = 0; i < 9; i++){
                                        prog_chars = prog_chars.append(static_cast<char>(program[i]));

                                        if(mStop_rds){
                                            prog_chars.clear();
                                            //emit clear_lbl("");
                                        }
                                        //qDebug()<<"prog_chars in for loop: "<<prog_chars;

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
                                                        //QChar char(radiotext[i]);
                                                        //qDebug() << irgendwas;

                                                rds_chars.append(rds_single_char);

                                                if(rds_chars.contains('\0')){
                                                    rds_chars = rds_chars.replace('\0', "");
                                                }
                                            } //end switch

                                           //if(mStop_rds){
                                               //rds_chars.clear();
                                               //emit clear_lbl("");
                                           //}

                                       }
                                       //qDebug()<<"prog_chars for loop: " << prog_chars;
                                    } //end for loop

                                    //if(mStop_rds){
                                    //    rds_chars.clear();
                                    //    prog_chars.clear();
                                    //}


                                    //if(!mStop_rds){
                                    emit rds_out(rds_chars);
                                    emit rds_prog_out(prog_chars);
                                    //} else {

                                    //rds_single_char = ' ';
                                    //emit rds_out("");
                                    //emit rds_out("");
//                                    emit clear_lbl("");
                                    //}

                                    fflush(stdout);


                                    //qDebug() << "mStop after end of for loop: " << mStop_rds;
                            } //end if loop
                            //QThread::currentThread()->msleep(5000);
                            //qDebug() << "mStop before end of while loop: " << mStop_rds;
//qDebug()<<"ping after if loop";
                            //mutex.unlock(); //m2


//if(mStop_rds) break;
                  } //end while loop

                    //rds_chars="";
                    //prog_chars="";

//                    emit rds_out(rds_chars);
//                    qDebug()<<"rds_chars after while: " <<rds_chars;
//                    emit rds_prog_out(prog_chars);
//                    qDebug()<<"prog_chars after while: " <<prog_chars;

//qDebug()<<"ping after while";

//                            emit rds_out(rds_chars);
//                            emit rds_prog_out(prog_chars);
                   // }while(!mStop_rds);
                    //emit finished_rds_reading();
//                    rds_chars.clear();
//                    prog_chars.clear();
                    //rds_chars = "";
                    //qDebug()<<"rds_chars after while: "<<rds_chars;
                   // prog_chars = "";
                    //qDebug()<<"prog_chars after while: "<<prog_chars;
                    //emit clear_lbl("");
                    //mStop_rds = false;
                    //mStop_rds = true;

                    //qDebug() << "mStop after end of while loop: " << mStop_rds;
            }
            //emit finished_rds_reading();
            //g_mStop_rds = false;
            //qDebug() << "thread rds stopped";
    net_close(rdsfd);
    //}
    }
    return 0; //m1
}
