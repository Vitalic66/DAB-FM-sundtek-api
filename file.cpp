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
        return;
    }

    QFile file_dab_fav(path_dab_fav);
    if(!file_dab_fav.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
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

    qDebug() << "g_dab_vec_vec before fav:" << g_dab_vec_vec;

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

    qDebug() << "g_dab_vec_vec after fav:" << g_dab_vec_vec;

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
        qDebug() << "split sort list" << split_sort_list;
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

        if(g_dab_vec_vec[i][3] != "fav"){
            dab_vec_vec_to_line = g_dab_vec_vec[i][0] + "," + g_dab_vec_vec[i][1] + "," + g_dab_vec_vec[i][2] + "," + g_dab_vec_vec[i][3];

            unsort_list.append(dab_vec_vec_to_line);
        }
    }

    QTextStream dab_write_to_file(&file_dab);
    QStringList list {File::sort_list(unsort_list)};

    foreach(QString dab_line_sort, list){
        dab_write_to_file << dab_line_sort << "\n";
    }

    file_dab.flush();
    file_dab.close();
}


QStringList File::sort_list(QStringList list){

    QCollator coll;
    coll.setNumericMode(true);

    std::sort(list.begin(), list.end(), [&](const QString& s1, const QString& s2){ return coll.compare(s1, s2) < 0; });

    return list;
}
