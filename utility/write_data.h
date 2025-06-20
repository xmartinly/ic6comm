#ifndef WRITEDATA_H
#define WRITEDATA_H

#include <QObject>
#include <QRunnable>

class WriteData: public QObject, public QRunnable {
    Q_OBJECT
  public:
    WriteData(const QString& data, const QString& file, const QString& name);
    ~WriteData();

  protected:
    void run();

  signals:
    // send write file information to main thread
    void emit_write_data_res(const QString&);
    void emit_file_size(const QString& name, float size);

  private:
    // data to be written
    QString data_str_;
    // data file name
    QString data_file_;
    // inst name
    QString inst_name_;
    QString file_name_ = "n/a";
};

#endif // WRITEDATA_H
