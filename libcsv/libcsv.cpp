#include "libcsv.hpp"
#include <QTextStream>

CsvManipulator::CsvManipulator(QObject *parent, const QString& file_name, const QChar& sep): QObject(parent), _file(file_name), _separator(sep)
{ }

CsvManipulator::~CsvManipulator()
{
    close();
}

bool CsvManipulator::open(){
    if(!_file.open(QIODevice::ReadWrite | QIODevice::Text)) return false;
    else return true;
}


QString CsvManipulator::trimCSV(QString item)
{
    if((!item.isEmpty())&&(item[0] == QChar(34)))
        item.remove(0,1);
    if((!item.isEmpty())&&(!item.isNull())&(item[item.count()-1] == QChar(34)))
        item.remove(item.count()-1,1);
    if(!item.isEmpty())
        item = item.replace("\"\"","\"");
    return item;

}

QList<QStringList> CsvManipulator::CSVRead()
{
    if (_file.isOpen())
    {
        bool Quote = false;
        QList<QString> ItemList;
        QString item = "";
        QTextStream out(&_file);

        while(!out.atEnd())
        {
            QString line(out.readLine().simplified());
            int count = line.count();
            for (int i = 0;i<count;i++)
            {
                if (line[i] == QChar('\''))
                {
                    Quote = (Quote) ? false : true;
                }
                if ((Quote != true)&(line[i] == _separator))
                {
                    ItemList.append(trimCSV(item));
                    item = "";
                }
                else
                {
                    item += line[i];
                }

                if ((count-1 == i)&(Quote != true))
                {
                    item = trimCSV(item);
                    if (item != "")
                        ItemList.append(item);
                    _lines_list.append(ItemList);
                    ItemList.clear();
                    item = "";
                }
            }

        }
    }
    close();
    return _lines_list;
}

bool CsvManipulator::CSVWrite(QList<QStringList> &data)
{
      if(data.isEmpty())  return false;

      if(_file.isOpen()) _file.close();
      if(!_file.remove()) qDebug() << "libcsv.cpp:: Failed to delete file! Probably, it does not exist...";

      if(!open()) qDebug() << "libcsv.cpp: Failed to open file! Probably, it does not exist...";

      QTextStream out(&_file);
      QString toInsert;

      for(int i = 0; i < data.size(); i++)
      {
          for(int j = 0; j < data[i].size(); j++)
          {
              toInsert += data[i][j] + ((j == data[i].size()-1)?"\n":QString(_separator));
          }
      }

      out << toInsert;
      close();
      return true;
}
