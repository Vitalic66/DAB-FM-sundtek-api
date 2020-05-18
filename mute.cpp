#include "mute.h"


Mute::Mute(QObject *parent) : QObject(parent)
{

}

//if(g_tuner_mode == "DAB"){
//    fd = net_open("/dev/dab0", O_RDWR);
//} else{
//    fd = net_open("/dev/radio0", O_RDWR);
//}

//if(g_last_state_mute_unmute == "unmuted"){

//    //mute DAB
//    if(g_tuner_mode == "DAB"){
//        mMute.set_mute(fd, "on");
//        //g_last_state_mute_unmute = "muted";
//    }

//tmp_mute_unmute_state = "muted";
//}

//if(g_last_state_mute_unmute == "muted"){

//    //unmute DAB
//    if(g_tuner_mode == "DAB"){
//        mMute.set_mute(fd, "off");
//        //g_last_state_mute_unmute = "unmuted";
//    }
//tmp_mute_unmute_state = "unmuted";
//}

//g_last_state_mute_unmute = tmp_mute_unmute_state;

//int Mute::set_mute(int fd, const char *arg) {
int Mute::set_mute() {

        int type = 0;
        QString tmp_mute_unmute_state;

        if(g_tuner_mode == "DAB"){
            fd = net_open("/dev/dab0", O_RDWR);
        } else{
            fd = net_open("/dev/radio0", O_RDWR);
        }

        if(g_last_state_mute_unmute == "unmuted"){

            //mute DAB
            if(g_tuner_mode == "DAB"){
                //mMute.set_mute(fd, "on");
                //g_last_state_mute_unmute = "muted";
            }

        tmp_mute_unmute_state = "muted";
        }

        if(g_last_state_mute_unmute == "muted"){

            //unmute DAB
            if(g_tuner_mode == "DAB"){
                //mMute.set_mute(fd, "off");
                //g_last_state_mute_unmute = "unmuted";
            }
        tmp_mute_unmute_state = "unmuted";
        }

        g_last_state_mute_unmute = tmp_mute_unmute_state;

        struct v4l2_control control;
        //if (strcmp(arg, "off") == 0) {
        if (g_last_state_mute_unmute == "unmuted") {
                control.id = V4L2_CID_AUDIO_MUTE;
                control.value = 0;
                //fprintf(stdout, "Enabling audiostream\n");
                net_ioctl(fd, VIDIOC_S_CTRL, &control);
        //} else if (strcmp(arg, "on") == 0) {
        } else if (g_last_state_mute_unmute == "muted") {
                //fprintf(stdout, "Disabling audiostream\n");
                control.id = V4L2_CID_AUDIO_MUTE;
                control.value = 1;
                net_ioctl(fd, VIDIOC_S_CTRL, &control);
        } /*else
                fprintf(stdout, "Wrong argument [%s] choose between on|off\n", arg);*/

        return 0;
}
