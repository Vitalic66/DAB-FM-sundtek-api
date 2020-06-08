#ifndef DAB_MOT_PROCESS_H
#define DAB_MOT_PROCESS_H

#include <QObject>
#include <QDebug>
#include <QImage>
#include <QApplication>

#include <charsets.h>
#include <mot_manager.h>
#include <tools.h>
#include <dab_mot.h>

class Dab_MOT_Process : public QObject
{
    Q_OBJECT
public:
    explicit Dab_MOT_Process(QObject *parent = nullptr);

signals:

    void new_mot(QImage MOTImage);

public slots:
    void dab_mot_process_start();
    void dab_mot_process_stop();

private:

    //void dab_process_mot_data();

    MOTManager mot_manager;
    QImage motImage;
    dab_mot mdabmot;

    bool mStop_mot_process;



};

#endif // DAB_MOT_PROCESS_H
