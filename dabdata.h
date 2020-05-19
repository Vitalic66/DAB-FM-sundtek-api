#ifndef DABDATA_H
#define DABDATA_H

#include <QObject>
#include <QDebug>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep

class DabData : public QObject
{
    Q_OBJECT
public:
    explicit DabData(QObject *parent = nullptr);

signals:

    void finished();
    void dlsUpdated();

public slots:

    void receive_dab_data();

private:
    //QObject xdata;
    //struct xdata;

};

#endif // DABDATA_H
