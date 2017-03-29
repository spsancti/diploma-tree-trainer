#include "nodetrainer.h"

NodeTrainer::NodeTrainer()
{

}

void NodeTrainer::initWeights()
{
    for(int i = 0; i < nodes.length(); i++)
    {
        unsigned int inputs  = trainDatas[i]->num_input_train_data();
        nodes[i]-> create_sparse(1.0f, 3, inputs, 2 * (inputs + 1) / 3, 1);
        nodes[i]->init_weights(*trainDatas[i]);
    }
}

NetworkOptimumParams NodeTrainer::detectOptimumTrainParams(int classCode)
{
    QSharedPointer<FANN::neural_net> temp
            = QSharedPointer<FANN::neural_net> (new FANN::neural_net);
    unsigned int inputs  = trainDatas[classCode]->num_input_train_data();
    temp -> create_sparse(0.2f, 3, inputs, 2 * (inputs + 1) / 3, 1);



    QSharedPointer<FANN::training_data> pTrain
            = QSharedPointer<FANN::training_data>(new FANN::training_data(*trainDatas[classCode]));
    pTrain->subset_train_data(0, std::min(pTrain->length_train_data(), 100u));

    QSharedPointer<FANN::training_data> pTest
            = QSharedPointer<FANN::training_data>(new FANN::training_data(*testDatas[classCode]));
    pTest->subset_train_data(0, std::min(pTest->length_train_data(), 100u));

    fann_type min = 1000, mse = 1000;
    NetworkOptimumParams bestParams = {FANN::TRAIN_RPROP, FANN::SIGMOID, FANN::LINEAR};
    for(int k = FANN::TRAIN_INCREMENTAL; k <= FANN::TRAIN_SARPROP; k++)
    {
        for(int  i = FANN::LINEAR; i <= FANN::COS_SYMMETRIC; i++)
        {
            for(int  j = FANN::LINEAR; j <= FANN::COS_SYMMETRIC; j++)
            {
                //skip THRESHOLD as it has no derivative
                if(i == FANN::THRESHOLD
                || j == FANN::THRESHOLD)
                    continue;

                temp->nullify_weights();
                temp->reset_MSE();

                NetworkOptimumParams params;
                params.algorithm = (FANN::training_algorithm_enum) k;
                params.hidden    = (FANN::activation_function_enum)i;
                params.output    = (FANN::activation_function_enum)j;

                mse = examineTrain(temp, params, pTrain, pTest);
                if(mse < min)
                {
                    min = mse;
                    bestParams = params;
                }
            }
        }
    }
    emit echo(QString(classNames[classCodes[classCode]] +" : " + QString(FANN_ACTIVATIONFUNC_NAMES[bestParams.hidden])
              + " : " + QString(FANN_ACTIVATIONFUNC_NAMES[bestParams.output])
              + ", with MSE: " + QString::number(min)
                  ));

    return bestParams;
}

fann_type NodeTrainer::examineTrain(QSharedPointer<FANN::neural_net> ann,
                                    NetworkOptimumParams params,
                                    QSharedPointer<FANN::training_data> data,
                                    QSharedPointer<FANN::training_data> testData)
{
    ann->set_training_algorithm(params.algorithm);
    ann->set_activation_function_hidden(params.hidden);
    ann->set_activation_function_output(params.output);

    ann->train_on_data(*data, 100, 0, 0.0);
    float trainMSE = ann->get_MSE();
    ann->reset_MSE();

    ann->test_data(*testData);
    float testMSE = ann->get_MSE();

    if(ann->get_errno() != FANN_E_NO_ERROR)
    {
        emit error("Error occured inside of library. Please, beat programmer!");
    }
    return (trainMSE + testMSE) / 2;
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

        float testMSE = 1, prevTestMSE = 1.1;
        int epochs = 0;
        QString termination = "";

        while (1)
        {
           if(testMSE < 1e-2)
            {
                termination = " Reached minima";
                break;
            }

            float diff = fabsf(prevTestMSE - testMSE);
            emit echo(QString::number(diff));
            if(diff < 1e-4)
            {
                termination = " Stagnation";
                break;
            }

            nodes[i]->train_epoch(*trainDatas[i]);
            nodes[i]->reset_MSE();

            prevTestMSE = testMSE;
            nodes[i]->test_data(*testDatas[i]);
            mse = testMSE = nodes[i]->get_MSE();

            epochs ++;
        }

        emit echo("Epochs :" + QString::number(epochs) + termination);
/*
        nodes[i]->train_on_data(*trainDatas[i], 5000, 1000, 1e-4);
        if(nodes[i]->get_errno() != FANN_E_NO_ERROR)
            emit error("Error occured inside of library. Please, beat programmer!");
        mse += nodes[i]->get_MSE();*/
    }

    return mse;
}


bool NodeTrainer::saveNetworks()
{
    bool success = true;

    for(int i = 0; i < nodes.length(); i++)
    {
        success = nodes[i]->save(QString(
                                     fileInfo.path()
                                     + "/class"
                                     + QString::number(classCodes[i])
                                     + ".net")
                                 .toStdString());
        if(!success)
        {
            return false;
        }
    }

    return success;
}

bool NodeTrainer::saveXML()
{
    QFile f(fileInfo.path() + "/classifier.xml");
    if(!f.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        return false;
    }

    QXmlStreamWriter xml(&f);

    xml.setAutoFormatting(true);
    xml.writeStartDocument();

    xml.writeStartElement("tree");

    for(int i = 0; i < nodes.length(); i++)
    {
        xml.writeStartElement("classifier");
        xml.writeAttribute("idx", QString::number(i));
        xml.writeAttribute("mse", QString::number(nodes[i]->get_MSE()));
            xml.writeStartElement("disease");
                xml.writeCharacters(classNames[classCodes[i]]);
            xml.writeEndElement(); //disease
            xml.writeStartElement("code");
                xml.writeCharacters(QString::number(classCodes[i]));
            xml.writeEndElement(); //code
            xml.writeStartElement("path");
                xml.writeCharacters(QString("class" + QString::number(classCodes[i])+ ".net"));
            xml.writeEndElement(); //path
        xml.writeEndElement(); // classifier
    }

    xml.writeEndElement(); // /tree
    xml.writeEndDocument();

    f.flush();
    f.close();

    return true;
}

bool NodeTrainer::setFilename(QString filename)
{
    fileInfo.setFile(filename);
    if(!fileInfo.exists())
        return false;
    else
        return true;
}

bool NodeTrainer::loadData(QString filename)
{
    if(!setFilename(filename))
    {
        return false;
    }

    for(int i = 0; i < nodes.length(); i++)
    {
        bool successTrain = trainDatas[i]->read_train_from_file(QString(
                                                     fileInfo.path()
                                                     + "/"
                                                     + fileInfo.baseName()
                                                     + QString::number(classCodes[i])
                                                     + "-train.dat")
                                                 .toStdString());
        bool successTest = testDatas[i]->read_train_from_file(QString(
                                                     fileInfo.path()
                                                     + "/"
                                                     + fileInfo.baseName()
                                                     + QString::number(classCodes[i])
                                                     + "-test.dat")
                                                 .toStdString());

        if(!successTrain || !successTest)
        {
            return false;
        }
    }

    return true;
}

bool NodeTrainer::loadClassNames(QString filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly))
    {
        return false;
    }

    classCodes.clear();
    classNames.clear();

    char buffer[128];
    while(!f.atEnd())
    {
        f.readLine(buffer, 128);

        char classname[100];
        int classcode;

        if(sscanf(buffer, "%d%s", &classcode, classname) != 2)
            return false;

        classCodes.push_back(classcode);
        classNames[classcode] = classname;
    }

    return init();
}

bool NodeTrainer::init()
{
    nodes.clear();
    trainDatas.clear();
    testDatas.clear();
    for(int i = 0; i < classCodes.length(); i++)
    {
        nodes.push_back(QSharedPointer<FANN::neural_net> (new FANN::neural_net));
        trainDatas.push_back(QSharedPointer<FANN::training_data> (new FANN::training_data));
        testDatas.push_back(QSharedPointer<FANN::training_data> (new FANN::training_data));
    }
    return true;
}

NodeTrainer::~NodeTrainer()
{
    for(int i = 0; i < nodes.length(); i++)
    {
        nodes[i].clear();
        trainDatas[i].clear();
        testDatas[i].clear();
    }
}
