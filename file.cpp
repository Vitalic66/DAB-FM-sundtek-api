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

    mScan.dab_vec_vec.clear();

    QTextStream dab_stream_in_file(&file_dab);
    QString dab_stream_in_line;

    while (!dab_stream_in_file.atEnd()) {

       dab_stream_in_line = dab_stream_in_file.readLine();

       QStringList split_dab_stram_in_line = dab_stream_in_line.split(",");

       QVector<QString> dab_vec;

       dab_vec.push_back(split_dab_stram_in_line.at(0)); //== name of station
       dab_vec.push_back(split_dab_stram_in_line.at(1)); //== freq of station
       dab_vec.push_back(split_dab_stram_in_line.at(2)); //== sid of station
       dab_vec.push_back(split_dab_stram_in_line.at(3)); //== is favorit?
       mScan.dab_vec_vec.push_back(dab_vec);
    }

    //qDebug() << "dab vec vec" << dab_vec_vec;

    file_dab.close();
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

    /*
     *  fm_vec_vec to QstringList
    */
    qDebug() << "test overload: " << mScan.mStop_dab_scan;
    QString dab_vec_vec_to_line;
    QStringList unsort_list;
    qDebug() << "dabvecvec before for: " << mScan.dab_vec_vec;
    qDebug() << "g_dabvecvec before for: " << g_dab_vec_vec;
    //qDebug() << "dabvecvecsize blub in for: " << mScan.dab_vec_vec.size();
    qDebug() << "g_dabvecvecsize blub in for: " << g_dab_vec_vec.size();
    //for(int i = 0; i < mScan.dab_vec_vec.size(); i++){
    for(int i = 0; i < g_dab_vec_vec.size(); i++){
        //for(int = j; j < fm_vec.size(); j++){
            //dab_vec_vec_to_line = mScan.dab_vec_vec[i][0] + "," + mScan.dab_vec_vec[i][1] + "," + mScan.dab_vec_vec[i][2] + "," + mScan.dab_vec_vec[i][3];
            dab_vec_vec_to_line = g_dab_vec_vec[i][0] + "," + g_dab_vec_vec[i][1] + "," + g_dab_vec_vec[i][2] + "," + g_dab_vec_vec[i][3];
            qDebug() << "i: " << i << "dabvecvectoline: " << dab_vec_vec_to_line;
            unsort_list.append(dab_vec_vec_to_line);
        //}
    }

    qDebug() << "unsort list: " << unsort_list;

    QTextStream dab_write_to_file(&file_dab);

    QStringList list {File::sort_list(unsort_list)};

qDebug() << "sort list: " << list;

    foreach(QString dab_line_sort, list){
        dab_write_to_file << dab_line_sort << "\n";
        qDebug() << "write: " << dab_line_sort;
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
