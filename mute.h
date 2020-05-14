#ifndef MUTE_H
#define MUTE_H

#include <QObject>

//#include <dialog.h>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep

class Mute : public QObject
{
    Q_OBJECT
public:
    explicit Mute(QObject *parent = nullptr);

    int set_mute(int fd, const char *arg);

signals:

public slots:



};

#endif // MUTE_H
