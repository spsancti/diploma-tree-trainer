#ifndef NODETRAINER_H
#define NODETRAINER_H

#include <QObject>
#include "fann.h"
#include "fann_cpp.h"
#include "qdebug.h"
#include <QVector>

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

   void initWeights();

   NetworkOptimumParams detectOptimumTrainParams(int classCode);
   fann_type examineTrain(FANN::neural_net *ann, NetworkOptimumParams params, FANN::training_data *data);

   fann_type trainNodes();

   bool saveNetworks();

public:
    inline void setFilename(QString filename) { filename_ = filename; }
    bool loadData();


private:
    QVector <int> classCodes;
    QVector <FANN::neural_net> nodes;
    QVector <FANN::training_data> datas;
    QString filename_;
};

#endif // NODETRAINER_H
