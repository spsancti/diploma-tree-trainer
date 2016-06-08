#ifndef NODETRAINER_H
#define NODETRAINER_H

#include <QObject>
#include "fann.h"
#include "fann_cpp.h"
#include "qdebug.h"
#include <QVector>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QFile>
#include <QFileInfo>
struct NetworkOptimumParams
{
    FANN::training_algorithm_enum algorithm;
    FANN::activation_function_enum hidden;
    FANN::activation_function_enum output;
};

class NodeTrainer
{
public:
   NodeTrainer(QVector <int> classCodes_);
   ~NodeTrainer();

   void initWeights();

   NetworkOptimumParams detectOptimumTrainParams(int idx);
   fann_type examineTrain(QSharedPointer<FANN::neural_net> ann, NetworkOptimumParams params, QSharedPointer<FANN::training_data> data);

   fann_type trainNodes();

   bool saveNetworks();
   bool saveXML();
public:
    bool setFilename(QString filename);
    bool loadData(QString filename);
    bool loadClassNames(QString filename);

private:
    QVector <int> classCodes;
    QMap<int, QString> classNames;

    QVector <QSharedPointer<FANN::neural_net> > nodes;
    QVector <QSharedPointer<FANN::training_data> > datas;
    QFileInfo fileInfo;
};



#endif // NODETRAINER_H
