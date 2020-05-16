#include "mute.h"

Mute::Mute(QObject *parent) : QObject(parent)
{

}

int Mute::set_mute(int fd, const char *arg) {
        int type = 0;
        struct v4l2_control control;
        if (strcmp(arg, "off") == 0) {
                control.id = V4L2_CID_AUDIO_MUTE;
                control.value = 0;
                //fprintf(stdout, "Enabling audiostream\n");
                net_ioctl(fd, VIDIOC_S_CTRL, &control);
        } else if (strcmp(arg, "on") == 0) {
                //fprintf(stdout, "Disabling audiostream\n");
                control.id = V4L2_CID_AUDIO_MUTE;
                control.value = 1;
                net_ioctl(fd, VIDIOC_S_CTRL, &control);
        } else
                fprintf(stdout, "Wrong argument [%s] choose between on|off\n", arg);

        return 0;
}
