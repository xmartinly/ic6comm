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

    void startWork(int interval);  // start timer
    void stopWork();               // stop task

  signals:
    void dataReceived(const QByteArray& data, const QString& ip); // data receive signal
    void errorOccurred(const QString& error, const QString& ip);  // error signal

  public slots:
    void work();  // scheduled task processing

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
