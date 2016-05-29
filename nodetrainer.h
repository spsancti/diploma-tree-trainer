#ifndef NODETRAINER_H
#define NODETRAINER_H

#include <QObject>
#include "fann.h"
#include "fann_cpp.h"
#include <QVector>

class NodeTrainer
{
public:
   NodeTrainer(QVector <int> classCodes_);
   fann_type trainNodes();

   inline void setFilename(QString filename) { filename_ = filename; }

private:
    QVector <FANN::neural_net> nodes;
    QVector <int> classCodes;
    QString filename_;
};

#endif // NODETRAINER_H
