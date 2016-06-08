#include "mainwindow.h"
#include <QApplication>
#include "fann.h"
#include "fann_cpp.h"
#include <QDebug>
#include "datasplitter.h"
#include "nodetrainer.h"

DataSplitter d;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    d.loadData(QString("D:\\Discovery\\Software_eclipse\\NeuralCoughClassifier\\cascade\\generated\\rawdata.dat"));
    d.splitData();

    NodeTrainer n(d.getClassCodes());

    n.loadData(d.getFilename());
    n.loadClassNames("D:\\Discovery\\Software_eclipse\\NeuralCoughClassifier\\cascade\\generated\\classnames.dat");
    n.initWeights();
    qDebug() << n.trainNodes();
    n.saveNetworks();
    n.saveXML();

    return a.exec();
}
