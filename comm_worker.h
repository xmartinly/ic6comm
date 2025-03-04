#ifndef COMM_WORKER_H
#define COMM_WORKER_H

#include <QObject>
#include <QWidget>

struct CygResp {
    QString version_;
    QList<int> activity_;
    QList<double> frequency_;
    QList<int> crystal_ok_;
};


class CommWorker: public QObject {
    Q_OBJECT
  public:
    explicit CommWorker(const QString& ip, const QString& name, QObject* parent = nullptr);
    ~CommWorker();

    void startWork(int interval);  // 启动定时任务
    void stopWork();               // 停止任务

  signals:
    void dataReceived(const QByteArray& data, const QString& ip); // 数据接收信号
    void errorOccurred(const QString& error, const QString& ip);  // 错误信号

  public slots:
    void work();  // 定时任务处理

  private slots:
    void handleConnected();
    void handleReadyRead();
    void handleError(QAbstractSocket::SocketError error);


  private:
    QTcpSocket* socket;
    QTimer* timer;
    QString targetIp;
    QString targetName;
    bool isConnected = false;

    void connectToHost();

};

#endif // COMM_WORKER_H
