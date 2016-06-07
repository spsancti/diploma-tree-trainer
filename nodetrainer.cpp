#include "nodetrainer.h"

NodeTrainer::NodeTrainer(QVector <int> classCodes_) :
    classCodes(classCodes_),
    nodes(classCodes_.length()),
    datas(classCodes_.length())
{
    for(int i = 0; i < nodes.length(); i++)
    {
        nodes[i] = QSharedPointer<FANN::neural_net> (new FANN::neural_net);
        datas[i] = QSharedPointer<FANN::training_data> (new FANN::training_data);
        nodes[i]->create_standard(3, 32, 22, 1);
    }
}

void NodeTrainer::initWeights()
{
    for(int i = 0; i < nodes.length(); i++)
    {
        nodes[i]->init_weights(*datas[i]);
    }
}

NetworkOptimumParams NodeTrainer::detectOptimumTrainParams(int classCode)
{
    QSharedPointer<FANN::neural_net> temp = QSharedPointer<FANN::neural_net> (new FANN::neural_net);
    temp->create_sparse(0.2f, 3, 32, 22, 1);

    fann_type min = 1, mse = 1;
    NetworkOptimumParams bestParams = {FANN::TRAIN_RPROP, FANN::LINEAR, FANN::LINEAR};
    for(int k = 0; k < 4; k++)
    {
        for(int  i = 2; i < 13; i++)
        {
            for(int  j = 2; j < 13; j++)
            {
                temp->randomize_weights(-0.000f, 0.000f);
                temp->reset_MSE();

                //qDebug() << FANN_TRAIN_NAMES[k] << ":" << FANN_ACTIVATIONFUNC_NAMES[i] << "->" << FANN_ACTIVATIONFUNC_NAMES[j];

                NetworkOptimumParams params;
                params.algorithm = (FANN::training_algorithm_enum) k;
                params.hidden    = (FANN::activation_function_enum)i;
                params.output    = (FANN::activation_function_enum)j;

                mse = examineTrain(temp, params, datas[classCode]);
                if(mse < min)
                {
                    min = mse;
                    bestParams = params;
                }
            }
        }
    }

    qDebug() << "Best fit: " << FANN_TRAIN_NAMES[bestParams.algorithm] << ":" << FANN_ACTIVATIONFUNC_NAMES[bestParams.hidden] << "->" << FANN_ACTIVATIONFUNC_NAMES[bestParams.output] << "with MSE: " << min;

    return bestParams;
}

fann_type NodeTrainer::examineTrain(QSharedPointer<FANN::neural_net> ann, NetworkOptimumParams params, QSharedPointer<FANN::training_data> data)
{
    ann->set_training_algorithm(params.algorithm);
    ann->set_activation_function_hidden(params.hidden);
    ann->set_activation_function_output(params.output);

    ann->train_on_data(*data, 100, 0, 0.0);

    return ann->get_MSE();
}

fann_type NodeTrainer::trainNodes()
{
    fann_type mse = 0;
    for(int i = 0; i < nodes.length(); i++)
    {
        NetworkOptimumParams params = detectOptimumTrainParams(i);

        nodes[i]->set_training_algorithm(params.algorithm);
        nodes[i]->set_activation_function_hidden(params.hidden);
        nodes[i]->set_activation_function_output(params.output);

        nodes[i]->train_on_data(*datas[i], 50000, 1000, 1e-5);
        mse += nodes[i]->get_MSE();
    }

    return mse;
}


bool NodeTrainer::saveNetworks()
{
    bool success = true;
    for(int i = 0; i < datas.length(); i++)
    {
        success = nodes[i]->save(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ ".net").toStdString());
        if(!success)
            return false;
    }

    return success;
}


bool NodeTrainer::loadData()
{
    bool success = true;
    for(int i = 0; i < datas.length(); i++)
    {
        success = datas[i]->read_train_from_file(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ ".dat").toStdString());
        if(!success)
            return false;
        datas[i]->shuffle_train_data();
    }

    return success;
}

NodeTrainer::~NodeTrainer()
{
    for(int i = 0; i < nodes.length(); i++)
    {
        nodes[i].clear();
        datas[i].clear();
    }
}
