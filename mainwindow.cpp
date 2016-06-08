#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pbOpenData,    SIGNAL(released()), this, SLOT(updateLeData()));
    connect(ui->pbOpenNames,   SIGNAL(released()), this, SLOT(updateLeNames()));
    connect(ui->pbStartLearn,  SIGNAL(released()), this, SLOT(startLearning()));
    connect(ui->pbPrepare,     SIGNAL(released()), this, SLOT(prepareData()));

    trainer = new TrainerThread();
    connect(trainer, SIGNAL(echoSignal(QString)),
            ui->tbLogs, SLOT(append(QString)));
    connect(trainer, SIGNAL(error(QString)),
            this, SLOT(error(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateLeData()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Выберите файл данных"),
                                                    "D:\\",
                                                    tr("Data files (*.dat)"));
    ui->leData->setText(filename);
    ui->pbPrepare->setDisabled(ui->leData->text().isEmpty());
    ui->pbStartLearn->setDisabled(ui->leData->text().isEmpty()
                               || ui->leNames->text().isEmpty());
    trainer->setDataFilename(filename);
}

void MainWindow::updateLeNames()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Выберите файл названий классов"),
                                                    "D:\\",
                                                    tr("Data files (*.dat)"));
    ui->leNames->setText(filename);
    ui->pbStartLearn->setDisabled(ui->leData->text().isEmpty()
                               || ui->leNames->text().isEmpty());

    trainer->setClassFilename(filename);
}

void MainWindow::prepareData()
{
    //DataSplitter may grow fat for stack
    DataSplitter *d = new DataSplitter();

    if(!d->loadData(ui->leData->text()))
    {
        error("Ошибка чтения файла данных.");
        return;
    }

    d->splitData();

    if(d->exists())
    {
        QMessageBox msgBox;
        msgBox.setText("Найдены подготовленные данные.");
        msgBox.setInformativeText("Перезаписать?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Ok)
        {
            if(d->saveData())
                ui->tbLogs->append("Данные перезаписаны.");
            else
                error("Ошибка перезаписи данных.");
        }
    }
    else
    {
        if(d->saveData())
            ui->tbLogs->append("Данные сохранены.");
        else
            error("Ошибка сохранения данных.");
    }

    delete d;
}

void MainWindow::startLearning()
{
    if(ui->leData->text().isEmpty() || ui->leNames->text().isEmpty())
    {
        return;
    }

    trainer->start();
    ui->tbLogs->append("Начато обучение нейронных сетей (" + QDateTime::currentDateTime().time().toString("HH:mm:ss") + ")");
}

void MainWindow::abortLearning()
{
    trainer->exit(-1);
}

void MainWindow::error(QString err)
{
    ui->tbLogs->setTextColor(QColor::fromRgb(255, 0, 0));
    ui->tbLogs->append(err);
    ui->tbLogs->setTextColor(QColor::fromRgb(0, 0, 0));
    QMessageBox msgBox;
    msgBox.setText(err);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
