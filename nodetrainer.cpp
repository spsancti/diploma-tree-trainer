#include "nodetrainer.h"

NodeTrainer::NodeTrainer(QVector <int> classCodes_) : classCodes(classCodes_), nodes(classCodes_.length())
{
    for(int i = 0; i < nodes.length(); i++)
    {
        nodes[i].create_standard(4, 32, 7, 7, 1);
    }

}

fann_type NodeTrainer::trainNodes()
{

    for(int i = 0; i < nodes.length(); i++)
    {
        QString name = QString(filename_.split('.')[0] + QString::number(classCodes[i])+ ".dat");

        nodes[i].train_on_file(name.toStdString(), 50000, 1000, 1e-3);
        nodes[i].save(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ ".net").toStdString());

    }

}
