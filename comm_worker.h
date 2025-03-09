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
    void sendData(const QList<bool>& status, const QList<double>& frequencies, const QList<int>& activities, const QString& name);

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
    double factor_ic6 = 0.000873114913702011;
    QList<bool> status_ = {};
    QList<int> activities_ = {};
    QList<double> frequencies_ = {};
    QByteArray ba_test = QByteArray::fromHex("1500534709535303005347095353040053470953530500E7");

  private:
    QByteArray calcMsg(const QByteArray& resp);
    void calcFreq(const QByteArray& resp);
    void calcInt(const QByteArray& resp);
    void calcStatus(const QByteArray& resp);
    void connectToHost();
    void dataHandel(const QByteArray& data);
    uint calcMsgLen(const QByteArray& len_ba);

};

#endif // COMM_WORKER_H
