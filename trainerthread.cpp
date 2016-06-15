#include "trainerthread.h"

TrainerThread::TrainerThread()
{
    trainer = QSharedPointer <NodeTrainer> (new NodeTrainer());
    connect(&*trainer, SIGNAL(echo(QString)), this, SLOT(echoSlot(QString)));
    connect(&*trainer, SIGNAL(error(QString)), this, SLOT(errorSlot(QString)));
    //   wtf ^
}

void TrainerThread::run()
{
    if(trainer->loadClassNames(classFileName))
    {
        if(trainer->loadData(dataFileName))
        {
            trainer->initWeights();
            fann_type mse = trainer->trainNodes();
            emit echoSignal("Закончено обучение нейронных сетей (" + QDateTime::currentDateTime().time().toString("HH:mm:ss") + "), MSE: " + QString::number(mse));
            if(mse > 1e-2f)
                emit error("Ошибка сети слишком высокая, точность работы не гарантируется!");
            if(trainer->saveNetworks())
            {
                if(trainer->saveXML())
                {
                    emit echoSignal("Файлы классификатора сохранены. \r\n");
                    emit resultReady(mse);
                }
                else emit error("Ошибка сохранения файла конфигурации.");
            }
            else emit error("Ошибка сохранения нейронных сетей.");
        }
        else emit error("Ошибка загрузки файла данных.");
    }
    else emit error("Ошибка загрузки файла названий классов.");
}

