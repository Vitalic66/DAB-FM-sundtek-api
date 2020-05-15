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

#endif // GLOBALS_H
