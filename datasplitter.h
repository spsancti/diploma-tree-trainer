#ifndef DATASPLITTER_H
#define DATASPLITTER_H

#include "fann.h"
#include "fann_train.h"
#include "fann_cpp.h"
#include <QString>
#include <QVector>
#include <QFileInfo>


class DataSplitter
{
public:
    DataSplitter();

public:
    bool exists();
    bool loadData(QString filename);
    bool splitData();
    bool saveData();

    inline QString getFilename() { return QString(filename_); }
    inline unsigned int getClassNumber() { return classNumber; }
    inline QVector <int> getClassCodes() { return QVector <int> (classCodes); }


private:
    FANN::training_data data;
    QString filename_;
    QVector <int> classCodes;
    FANN::training_data* classData;

    unsigned int classNumber;
};


#endif // DATASPLITTER_H
