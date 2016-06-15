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
    connect(ui->pbHelp,        SIGNAL(released()), this, SLOT(openHelp()));

    trainer = new TrainerThread();
    connect(trainer, SIGNAL(echoSignal(QString)),
            ui->tbLogs, SLOT(append(QString)));
    connect(trainer, SIGNAL(error(QString)),
            this, SLOT(error(QString)));
    connect(trainer, SIGNAL(resultReady(float)),
            this, SLOT(handleResult(float)));

    setWindowTitle("DiaSpectrEx learn tool");
    isRunning = false;
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
    if(isRunning)
    {
        ui->tbLogs->setTextColor(QColor::fromRgb(0x5e, 0x27, 0x50));
        ui->tbLogs->append("Обучение в процессе (" + QDateTime::currentDateTime().time().toString("HH:mm:ss") + ")");
        ui->tbLogs->setTextColor(QColor::fromRgb(0, 0, 0));
        return;
    }

    //DataSplitter may grow fat for stack
    DataSplitter *d = new DataSplitter();

    if(!d->loadData(ui->leData->text()))
    {
        error("Ошибка чтения файла данных.");

        delete d;
        return;
    }

    d->splitData();

    if(d->exists())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Внимание");
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
    if(isRunning)
    {
        ui->tbLogs->setTextColor(QColor::fromRgb(0x5e, 0x27, 0x50));
        ui->tbLogs->append("Обучение в процессе (" + QDateTime::currentDateTime().time().toString("HH:mm:ss") + ")");
        ui->tbLogs->setTextColor(QColor::fromRgb(0, 0, 0));
        return;
    }

    trainer->start();
    isRunning = true;
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
    msgBox.setWindowTitle("Ошибка");
    msgBox.setText(err);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::openHelp()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Скорая помощь");
    msgBox.setText("\
<b>Программа подготовки классификаторов системы DiaSpectrEx</b><br><br>\
Чтобы создать новый классификатор:<br>\
1. В поле \"Файл данных\" выберите файл ,содержащий тренировочные данные для классификатора<br>\
2. В поле \"Файл названий\" выберите файл, содержащий коды классов и названия болезней<br>\
3. Нажмите кнопку \"Подготовка данных\"<br>\
4. Если данные успешно подготовлены, нажмите кнопку \"Обучение\"<br>\
5. После завершения подготовки классификатора перенесите файлы в директорию классификатора системы DiaSpectrEx\
");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
