#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>

class Globals : public QObject
{
    Q_OBJECT
public:
    explicit Globals(QObject *parent = nullptr);


signals:

public slots:
};

extern QVector<QVector<QString>> g_dab_vec_vec;
extern QString g_tuner_mode;
extern QString g_last_state_dab_fm;
extern QString g_last_state_mute_unmute;

#endif // GLOBALS_H
