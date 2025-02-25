#ifndef COMM_WORKER_H
#define COMM_WORKER_H

#include <QObject>
#include <QWidget>

class CommWorker: public QObject {
    Q_OBJECT
  public:
    CommWorker(QWidget* parent = nullptr);
    ~CommWorker();

  private:
    QByteArray hello_ = QByteArray::fromHex("0200480149");
    QByteArray data_ = QByteArray::fromHex("0C00535303005353040053530500FE");

};

#endif // COMM_WORKER_H
