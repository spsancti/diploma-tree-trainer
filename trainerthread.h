#ifndef TRAINERTHREAD_H
#define TRAINERTHREAD_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QSharedPointer>
#include <QDateTime>
#include "nodetrainer.h"

class TrainerThread : public QThread
{
    Q_OBJECT
public:
    TrainerThread();

    void run();

signals:
    void resultReady(float mse);
    void echoSignal(QString log);
    void error(QString);

public slots:    
    void setDataFilename(QString filename) {dataFileName = filename;}
    void setClassFilename(QString filename) {classFileName = filename;}
    void echoSlot(QString log) {emit echoSignal(log);}
    void errorSlot(QString log) {emit error(log);}

private:
    QSharedPointer <NodeTrainer> trainer;

private:
    QString dataFileName;
    QString classFileName;
};

#endif // TRAINERTHREAD_H
