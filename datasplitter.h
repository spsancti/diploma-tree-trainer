#ifndef DATASPLITTER_H
#define DATASPLITTER_H

#include "fann.h"
#include "fann_train.h"
#include "fann_cpp.h"
#include <QString>
#include <QVector>

class DataSplitter
{
public:
    DataSplitter();

public:
    bool loadData(QString filename);
    bool splitData();

    inline QString getFilename() { return QString(filename_); }
    inline unsigned int getClassNumber() { return classNumber; }
    inline QVector <int> getClassCodes() {return QVector <int> (classCodes); }


private:
    FANN::training_data data;
    QString filename_;
    QVector <int> classCodes;
    unsigned int classNumber;
};


#endif // DATASPLITTER_H
