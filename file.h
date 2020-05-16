#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QCollator>
#include <QDebug>


#include <scan.h>
#include <globals.h>


using namespace std;

class File : public QObject
{
    Q_OBJECT
public:
    explicit File(QObject *parent = nullptr);

    void dab_read_file();

    Scan mScan;

signals:

public slots:

    void dab_write_file();

    void dab_fav_write_file();

private:

    QStringList sort_list(QStringList list);

    QString path_dab = "../.dab.txt";
    QString path_dab_fav = "../.dab_fav.txt";

    QString path_fm = "../.fm.txt";
    QString path_fm_fav = "../.fm_fav.txt";



};

#endif // FILE_H
