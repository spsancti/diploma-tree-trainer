#include "datasplitter.h"
#include "QDebug"
DataSplitter::DataSplitter() : classNumber(0)
{
    classData = NULL;
}

bool DataSplitter::exists()
{
    if(classData == NULL)
        return false;

    QFileInfo info;
    size_t cnt = 0;
    for(size_t i = 0; i < classNumber; i++)
    {
        info.setFile(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ "-train.dat"));
        if(info.exists())
            cnt ++;
        info.setFile(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ "-test.dat"));
        if(info.exists())
            cnt ++;
    }
    if(cnt == 2 * classNumber) return true;
    else return false;
}

bool DataSplitter::loadData(QString filename)
{
    filename_ = filename;
    if(data.read_train_from_file(filename.toStdString()))
    {
        data.shuffle_train_data();
        return true;
    }
    return false;
}

//TODO: refactor this function
bool DataSplitter::splitData()
{
    fann_type ** outputs = data.get_output();
    int samples [1024] = { 0};
    int classIds[1024] = {-1};

    int j = 1;
    classIds[j] = outputs[0][0];
    classCodes.push_back(outputs[j][0]);

    for(unsigned int i = 1; i < data.length_train_data(); i++)
    {
        if(outputs[i][0] != outputs[i-1][0])
        {
            classIds[++j] = outputs[i][0];
            classCodes.push_back(outputs[i][0]);
        }
        samples[j] ++;
    }

    classNumber = j;

    int pos = 0;
    classData = new FANN::training_data[classNumber];

    for(size_t i = 0; i < classNumber; i ++)
    {
        classData[i] = data;
        pos += samples[i];

        for(unsigned int k = 0; k < classData[i].length_train_data(); k++)
        {
            if(classData[i].get_output()[k][0] == classIds[i+1])
            {
                classData[i].get_output()[k][0] = 1;
            }
            else
            {
                classData[i].get_output()[k][0] = 0;
            }
        }
    }

    return true;
}

bool DataSplitter::saveData()
{
    if(classData == NULL)
        return false;

    FANN::training_data *pTrain = new FANN::training_data[classNumber];
    FANN::training_data *pTest  = new FANN::training_data[classNumber];

    for(size_t i = 0; i < classNumber; i++)
    {
        pTrain[i] = classData[i];
        pTest[i] = classData[i];

        pTrain[i].subset_train_data(0, floorf(0.5 * (float)pTrain[i].length_train_data()));
        pTest[i].subset_train_data(floorf(0.5 * (float)pTest[i].length_train_data())+1, floorf(0.5 * (float)pTest[i].length_train_data()) - 1);

        if(pTrain[i].save_train(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ "-train.dat").toStdString()) == -1)
            return false;
        if(pTest[i].save_train(QString(filename_.split('.')[0] + QString::number(classCodes[i])+ "-test.dat").toStdString()) == -1)
            return false;
    }
    return true;
}
