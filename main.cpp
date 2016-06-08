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



    return a.exec();
}
