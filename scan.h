#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QVector>


#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep
#include <tune.h>

class Scan : public QObject
{
    Q_OBJECT
public:
    explicit Scan(QObject *parent = nullptr);
    //dab_frequency_list[200];



signals:

public slots:

    int media_scan_dabfrequencies(char *device, int devfd, int console, int running);

    int media_scan_dabservices(char *device);

/*public:

    uint32_t dab_frequency_list[]*/

private:
    //QVector<QVector<uint32_t>> dab_vec_vec;

    //QVector<uint32_t> dab_sid_vec;
    QVector<QString> dab_name_vec;
    QVector<QString> dab_sid_vec;
    Tune mTune;

public:
    QVector<QVector<QString>> dab_vec_vec;
};

#endif // SCAN_H
