#include "file.h"


File::File(QObject *parent) : QObject(parent)
{

}

void File::dab_read_file(){

    //read lines from fm.txt to qVectorVector

    QFile file_dab(path_dab);
    if(!file_dab.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        //return;
    }

    QFile file_dab_fav(path_dab_fav);
    if(!file_dab_fav.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        //return;
    }

    g_dab_vec_vec.clear();

    QTextStream dab_stream_in_file(&file_dab);
    QString dab_stream_in_line;
    QTextStream dab_fav_stream_in_file(&file_dab_fav);
    QString dab_fav_stream_in_line;

    while (!dab_stream_in_file.atEnd()) {

       dab_stream_in_line = dab_stream_in_file.readLine();

       QStringList split_dab_stream_in_line = dab_stream_in_line.split(",");

       QVector<QString> dab_vec;

       dab_vec.push_back(split_dab_stream_in_line.at(0)); //== name of station
       dab_vec.push_back(split_dab_stream_in_line.at(1)); //== freq of station
       dab_vec.push_back(split_dab_stream_in_line.at(2)); //== sid of station
       dab_vec.push_back(split_dab_stream_in_line.at(3)); //== is favorit?
       g_dab_vec_vec.push_back(dab_vec);
    }

    //qDebug() << "g_dab_vec_vec before fav:" << g_dab_vec_vec;

    while (!dab_fav_stream_in_file.atEnd()) {

       dab_fav_stream_in_line = dab_fav_stream_in_file.readLine();

       QStringList split_dab_fav_stream_in_line = dab_fav_stream_in_line.split(",");

       QVector<QString> dab_vec;

       dab_vec.push_back(split_dab_fav_stream_in_line.at(0)); //== name of station
       dab_vec.push_back(split_dab_fav_stream_in_line.at(1)); //== freq of station
       dab_vec.push_back(split_dab_fav_stream_in_line.at(2)); //== sid of station
       dab_vec.push_back(split_dab_fav_stream_in_line.at(3)); //== is favorit?
       g_dab_vec_vec.push_back(dab_vec);
    }

    //qDebug() << "g_dab_vec_vec after fav:" << g_dab_vec_vec;

    QString dab_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < g_dab_vec_vec.size(); i++){

        //if(g_dab_vec_vec[i][3] == "fav"){

            dab_vec_vec_to_line = g_dab_vec_vec[i][0] + "," + g_dab_vec_vec[i][1] + "," + g_dab_vec_vec[i][2] + "," + g_dab_vec_vec[i][3];

            unsort_list.append(dab_vec_vec_to_line);
        //}
    }

    g_dab_vec_vec.clear(); //clear before reread

    //sort both in files together

//    QTextStream dab_fav_stream_in_file(&file_dab);
//    QString dab_fav_stream_in_line;

    QStringList list {File::sort_list(unsort_list)};

    foreach(QString dab_line_sort, list){

        QVector<QString> dab_vec;
        QStringList split_sort_list = dab_line_sort.split(",");
        //qDebug() << "split sort list" << split_sort_list;
        dab_vec.push_back(split_sort_list.at(0)); //== name of station
        dab_vec.push_back(split_sort_list.at(1)); //== freq of station
        dab_vec.push_back(split_sort_list.at(2)); //== sid of station
        dab_vec.push_back(split_sort_list.at(3)); //== is favorit?
        g_dab_vec_vec.push_back(dab_vec);
    }

    file_dab.close();
    file_dab_fav.close();
}

void File::dab_fav_write_file(){

    //writes lines from qVectorVector to file

    QFile file_dab_fav(path_dab_fav);
    if(!file_dab_fav.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    //file_dab_fav.resize(0); //erase content in file before write

    QString dab_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < g_dab_vec_vec.size(); i++){

        if(g_dab_vec_vec[i][3] == "fav"){

            dab_vec_vec_to_line = g_dab_vec_vec[i][0] + "," + g_dab_vec_vec[i][1] + "," + g_dab_vec_vec[i][2] + "," + g_dab_vec_vec[i][3];

            unsort_list.append(dab_vec_vec_to_line);
        }
    }

    QTextStream dab_fav_write_to_file(&file_dab_fav);
    QStringList list {File::sort_list(unsort_list)};

    foreach(QString dab_line_sort, list){
        dab_fav_write_to_file << dab_line_sort << "\n";
    }

    file_dab_fav.flush();
    file_dab_fav.close();
}

void File::dab_write_file(){

    //writes lines from qVectorVector to file

    QFile file_dab(path_dab);
    if(!file_dab.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    file_dab.resize(0); //erase content in file before write

    QString dab_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < g_dab_vec_vec.size(); i++){

        //if(g_dab_vec_vec[i][3] != "fav"){

//        if(g_dab_vec_vec[i][3] == "fav"){
//            g_dab_vec_vec[i].replace(3, "");
//        }

            dab_vec_vec_to_line = g_dab_vec_vec[i][0] + "," + g_dab_vec_vec[i][1] + "," + g_dab_vec_vec[i][2] + "," + g_dab_vec_vec[i][3];

            unsort_list.append(dab_vec_vec_to_line);
        //}
    }

    QTextStream dab_write_to_file(&file_dab);
    QStringList list {File::sort_list(unsort_list)};

    foreach(QString dab_line_sort, list){
        dab_write_to_file << dab_line_sort << "\n";
    }

    file_dab.flush();
    file_dab.close();
}

void File::fm_read_file(){

    //read lines from fm.txt to qVectorVector

    QFile file_fm(path_fm);
    if(!file_fm.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        //return;
    }

    QFile file_fm_fav(path_fm_fav);
    if(!file_fm_fav.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        //return;
    }

    g_fm_vec_vec.clear();

    QTextStream fm_stream_in_file(&file_fm);
    QString fm_stream_in_line;
    QTextStream fm_fav_stream_in_file(&file_fm_fav);
    QString fm_fav_stream_in_line;

    while (!fm_stream_in_file.atEnd()) {

       fm_stream_in_line = fm_stream_in_file.readLine();

       QStringList split_fm_stream_in_line = fm_stream_in_line.split(",");

       QVector<QString> fm_vec;

       fm_vec.push_back(split_fm_stream_in_line.at(0)); //== name of station
       fm_vec.push_back(split_fm_stream_in_line.at(1)); //== freq of station
       fm_vec.push_back(split_fm_stream_in_line.at(2)); //== is favorit?
       g_fm_vec_vec.push_back(fm_vec);
    }

    //qDebug() << "g_fm_vec_vec before fav:" << g_fm_vec_vec;

    while (!fm_fav_stream_in_file.atEnd()) {

       fm_fav_stream_in_line = fm_fav_stream_in_file.readLine();

       QStringList split_fm_fav_stream_in_line = fm_fav_stream_in_line.split(",");

       QVector<QString> fm_vec;

       fm_vec.push_back(split_fm_fav_stream_in_line.at(0)); //== name of station
       fm_vec.push_back(split_fm_fav_stream_in_line.at(1)); //== freq of station
       fm_vec.push_back(split_fm_fav_stream_in_line.at(2)); //== is favorit?
       g_fm_vec_vec.push_back(fm_vec);
    }

    //qDebug() << "g_fm_vec_vec after fav:" << g_fm_vec_vec;

    QString fm_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < g_fm_vec_vec.size(); i++){

        fm_vec_vec_to_line = g_fm_vec_vec[i][0] + "," + g_fm_vec_vec[i][1] + "," + g_fm_vec_vec[i][2];

        unsort_list.append(fm_vec_vec_to_line);
    }

    g_fm_vec_vec.clear(); //clear before reread

    //sort both in files together

    QStringList list {File::sort_list(unsort_list)};

    foreach(QString fm_line_sort, list){

        QVector<QString> fm_vec;
        QStringList split_sort_list = fm_line_sort.split(",");
        //qDebug() << "split sort list" << split_sort_list;
        fm_vec.push_back(split_sort_list.at(0)); //== name of station
        fm_vec.push_back(split_sort_list.at(1)); //== freq of station
        fm_vec.push_back(split_sort_list.at(2)); //== is favorit?
        g_fm_vec_vec.push_back(fm_vec);
    }

    file_fm.close();
    file_fm_fav.close();
}

void File::fm_fav_write_file(){

    //writes lines from qVectorVector to file

    QFile file_fm_fav(path_fm_fav);
    if(!file_fm_fav.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    QString fm_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < g_fm_vec_vec.size(); i++){

        if(g_fm_vec_vec[i][2] == "fav"){

            fm_vec_vec_to_line = g_fm_vec_vec[i][0] + "," + g_fm_vec_vec[i][1] + "," + g_fm_vec_vec[i][2];

            unsort_list.append(fm_vec_vec_to_line);
        }
    }

    QTextStream fm_fav_write_to_file(&file_fm_fav);
    QStringList list {File::sort_list(unsort_list)};

    foreach(QString fm_line_sort, list){
        fm_fav_write_to_file << fm_line_sort << "\n";
    }

    file_fm_fav.flush();
    file_fm_fav.close();
}

void File::fm_write_file(){

    //writes lines from qVectorVector to file

    QFile file_fm(path_fm);
    if(!file_fm.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    file_fm.resize(0); //erase content in file before write

    QString fm_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < g_fm_vec_vec.size(); i++){

        fm_vec_vec_to_line = g_fm_vec_vec[i][0] + "," + g_fm_vec_vec[i][1] + "," + g_fm_vec_vec[i][2];

        unsort_list.append(fm_vec_vec_to_line);
    }

    QTextStream fm_write_to_file(&file_fm);
    QStringList list {File::sort_list(unsort_list)};

    foreach(QString fm_line_sort, list){
        fm_write_to_file << fm_line_sort << "\n";
    }

    file_fm.flush();
    file_fm.close();
}

QStringList File::sort_list(QStringList list){

    QCollator coll;
    coll.setNumericMode(true);

    std::sort(list.begin(), list.end(), [&](const QString& s1, const QString& s2){ return coll.compare(s1, s2) < 0; });

    return list;
}
