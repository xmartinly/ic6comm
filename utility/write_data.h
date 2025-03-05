#ifndef WRITEDATA_H
#define WRITEDATA_H

#include <QObject>
#include <QRunnable>

class WriteData: public QObject, public QRunnable {
    Q_OBJECT
  public:
    WriteData(const QString& data, const QString& file);
    ~WriteData();

  protected:
    void run();

  signals:
    //发送写文件的信息.
    void emit_write_data_res(const QString&);

  private:
    //需要写入的数据
    QString data_str;
    //完全文件名. c:\xx\aa.csv
    QString data_file;
};

#endif // WRITEDATA_H