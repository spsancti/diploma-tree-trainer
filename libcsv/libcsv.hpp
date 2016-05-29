#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTableWidget>
#include <QFile>
#include <QDebug>

class QTextStream;


class CsvManipulator: public QObject
{
    Q_OBJECT

private:
    QFile _file;                        // ucxoдный CSV-фaйл
    QChar _separator;                   // разделитель, по которому определяется разбивка на колонки
    QList<QStringList> _lines_list;     // массив полученный в результате разбора CSV-файла

public:
    CsvManipulator(QObject *parent = 0, const QString& file_name = QString(""), const QChar& sep = QChar(';'));
    ~CsvManipulator();

    void setFileName(const QString& name) { _file.setFileName(name); }
    void setSeparator(QChar sep) {_separator = sep; }
    bool open();
    void close() { if(isOpen()) _file.close(); }

    QList<QStringList> CSVRead();
    bool CSVWrite(QList<QStringList> &data);

    bool isOpen() const { return _file.isOpen(); }
private:
    QString trimCSV(QString item);
};
#endif // CSVREADER_H
