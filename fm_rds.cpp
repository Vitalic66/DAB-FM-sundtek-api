#include "fm_rds.h"

FM_rds::FM_rds(QObject *parent) : QObject(parent)
{
    mStop_rds = false;
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

void FM_rds::stop_rds_reading()
{
   mStop_rds = true;
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

    qDebug() << "rds thread started";

    qDebug() << "stop before while: " << mStop_rds;

    mStop_rds = false;

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
                    QString prog_chars;
                    QString rds_chars;
                    while(1 && !mStop_rds) {
                            net_ioctl(rdsfd, FM_RDS_STATUS, &data);

                            //QString rds_chars;


                            //char irgendwas;
                            //QStringList irgendwas_list;
                            //QString irgendwas_line;
QThread::msleep(workSpeed);

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
//                                            prog_chars = static_cast<char>(program[0]) + static_cast<char>(program[1]) + static_cast<char>(program[2])
//                                                    +    static_cast<char>(program[3]) + static_cast<char>(program[4]) + static_cast<char>(program[5])
//                                                    +    static_cast<char>(program[6]) + static_cast<char>(program[7]) + static_cast<char>(program[8])
//                                                    +    static_cast<char>(program[9]);
                                            //prog_chars = program[0]+program[1]+program[2];

                                            //if(prog_chars.length() < ){
                                                prog_chars.clear();
                                                for(int i = 0; i < 9; i++){
                                                prog_chars = prog_chars.append(static_cast<char>(program[i]));
                                                }
                                                if(prog_chars.contains('\0')){
                                                    prog_chars = prog_chars.replace('\0', "");
                                                }
                                            //}

//                                            + static_cast<char>(program[1]) + static_cast<char>(program[2])
//                                                                                                +    static_cast<char>(program[3]) + static_cast<char>(program[4]) + static_cast<char>(program[5])
//                                                                                                +    static_cast<char>(program[6]) + static_cast<char>(program[7]) + static_cast<char>(program[8])
//                                                                                                +    static_cast<char>(program[9]);
                                    }

//                                    QByteArray prog_chars;
//                                    for(int i = 0; i < 9; i++){
//                                        char rad_prog_single_char = 32;
//                                        rad_prog_single_char = static_cast<char>(program[i]);
//                                        prog_chars.append(rad_prog_single_char);
//                                    }



//                                    for(int i = 0; i < 9; i++){
//                                        char rad_prog_single_char = 32;
//                                        rad_prog_single_char = static_cast<char>(program[i]);

//                                    //qDebug() << "rdaio program: " << print_program;
//                                        qDebug() << "radio program single char: " << rad_prog_single_char;

//                                        prog_chars.append(rad_prog_single_char);
                                        qDebug() << "radio program: " << prog_chars;
//                                    }

                                    //QString print_prog = print_program;


                                    printf("PROGRAM: %s\n", print_program);

                                    //char irgendwas = static_cast<char>(print_program);

                                    //emit rds_out(print_program);

                                    //printf("RADIOTEXT: ");

                                    rds_chars.clear();

                                    //for (i=0;i<64;i++) {
for (i = 0; i < forsize; i++) {
                                        //if(mStop) return 0;

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
                                                            //qDebug() << i;
                                                            //QString buch = radiotext[i];
                                                            //ui->label_2->setText(buch);
                                                             //qDebug() << radiotext[i];

                                                            char rds_single_char = 32; //init with space
                                                            if(radiotext[i] != 127){ //remove white rectangles (ascii del)
//qDebug() << "single rds char: " << radiotext[i];
                                                                rds_single_char = static_cast<char>(radiotext[i]);

                                                            }
                                                            //QChar char(radiotext[i]);
                                                            //qDebug() << irgendwas;

                                                            rds_chars.append(rds_single_char);

                                                            if(rds_chars.contains('\0')){
                                                                rds_chars = rds_chars.replace('\0', "");
                                                            }

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
//                                           emit rds_out(rds_chars);
//                                           emit rds_prog_out(prog_chars);

                                    }
emit rds_out(rds_chars);
emit rds_prog_out(prog_chars);

                                    //printf("\n");

                                    //ui->label_2->setText(test);

                                    fflush(stdout);
                            }
                            //QThread::currentThread()->msleep(5000);
                    }
                    emit finished_rds_reading();
                    rds_chars.clear();
                    prog_chars.clear();
                    mStop_rds = false;
            }
    net_close(rdsfd);
    }
    return 0;
}
