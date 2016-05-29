#include "mainwindow.h"
#include <QApplication>
#include "fann.h"
#include "fann_cpp.h"

#include "datasplitter.h"
#include "nodetrainer.h"

DataSplitter d;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();



    d.loadData(QString("D:\\rawdata.dat"));
    d.splitData();

    NodeTrainer n(d.getClassCodes());
    n.setFilename(d.getFilename());
    n.trainNodes();

    return a.exec();
}
