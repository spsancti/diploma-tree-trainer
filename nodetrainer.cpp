#include "nodetrainer.h"

NodeTrainer::NodeTrainer()
{

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
    QSharedPointer<FANN::neural_net> temp
            = QSharedPointer<FANN::neural_net> (new FANN::neural_net);
    temp->create_sparse(0.2f, 3, 32, 22, 1);

    fann_type min = 1, mse = 1;
    NetworkOptimumParams bestParams = {FANN::TRAIN_RPROP, FANN::LINEAR, FANN::LINEAR};
    for(int k = 0; k < 4; k++)
    {
        for(int  i = 12; i > 1; i--)
        {
            for(int  j = 12; j > 1; j--)
            {
                temp->randomize_weights(0.000f, 0.000f);
                temp->reset_MSE();

                NetworkOptimumParams params;
                params.algorithm = (FANN::training_algorithm_enum) k;
                params.hidden    = (FANN::activation_function_enum)i;
                params.output    = (FANN::activation_function_enum)j;

                mse = examineTrain(temp, params, datas[classCode]);
                if(mse <= min)
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
                                    QSharedPointer<FANN::training_data> data)
{
    ann->set_training_algorithm(params.algorithm);
    ann->set_activation_function_hidden(params.hidden);
    ann->set_activation_function_output(params.output);

    ann->train_on_data(*data, 100, 0, 0.0);

    if(ann->get_errno() != FANN_E_NO_ERROR)
        emit error("Error occured inside of library. Please, beat programmer!");
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

        nodes[i]->train_on_data(*datas[i], 5000, 1000, 1e-5);
        if(nodes[i]->get_errno() != FANN_E_NO_ERROR)
            emit error("Error occured inside of library. Please, beat programmer!");
        mse += nodes[i]->get_MSE();
    }

    return mse;
}


bool NodeTrainer::saveNetworks()
{
    bool success = true;

    for(int i = 0; i < datas.length(); i++)
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

    bool success = true;
    for(int i = 0; i < datas.length(); i++)
    {
        success = datas[i]->read_train_from_file(QString(
                                                     fileInfo.path()
                                                     + "/"
                                                     + fileInfo.baseName()
                                                     + QString::number(classCodes[i])
                                                     + ".dat")
                                                 .toStdString());
        if(!success)
        {
            return false;
        }
        datas[i]->shuffle_train_data();
    }

    return success;
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

        sscanf(buffer, "%d%s", &classcode, classname);

        classCodes.push_back(classcode);
        classNames[classcode] = classname;
    }

    return init();
}

bool NodeTrainer::init()
{
    nodes.clear();
    datas.clear();
    for(int i = 0; i < classCodes.length(); i++)
    {
        nodes.push_back(QSharedPointer<FANN::neural_net> (new FANN::neural_net));
        datas.push_back(QSharedPointer<FANN::training_data> (new FANN::training_data));
        nodes[i]-> create_standard(3, 32, 22, 1);
    }
    return true;
}

NodeTrainer::~NodeTrainer()
{
    for(int i = 0; i < nodes.length(); i++)
    {
        nodes[i].clear();
        datas[i].clear();
    }
}
