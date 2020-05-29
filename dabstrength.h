#ifndef DABSTRENGTH_H
#define DABSTRENGTH_H

#include <QObject>
#include <QDebug>
#include <QApplication>

#include </opt/include/mediaclient.h>
#include <fcntl.h>  //for O_RDWR
#include <unistd.h> //for usleep

#include <globals.h>

class DabStrength : public QObject
{
    Q_OBJECT
public:
    explicit DabStrength(QObject *parent = nullptr);

signals:

    //void dab_sig(QString rds_stream);
    //void sig_out(uint32_t, uint32_t, uint8_t, uint8_t);
    void sig_out(QString, QString, QString, QString);

public slots:

    void start_sig_reading();
    void stop_sig_reading();

private:

    int dab_sig();
    bool mStop_sig;
};

#endif // DABSTRENGTH_H
