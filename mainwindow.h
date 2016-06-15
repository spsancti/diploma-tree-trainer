#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

#include "datasplitter.h"
#include "trainerthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void updateLeData();
    void updateLeNames();

    void prepareData();

    void startLearning();
    void abortLearning();
    void error(QString err);
    void handleResult(float){ isRunning = false;}
    void openHelp();
private:
    TrainerThread *trainer;
    bool isRunning;
};

#endif // MAINWINDOW_H
