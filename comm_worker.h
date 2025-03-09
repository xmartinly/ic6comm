#ifndef COMM_WORKER_H
#define COMM_WORKER_H

#include <QObject>
#include <QWidget>

// struct CygInst {
//     QString ip_;
//     QString name_;
//     uint acq_interval_;
//     QByteArray ba_cmd_;
//     QString last_data_tm_;
//     QStringList status_;
//     QStringList frequencies_;
//     QStringList activities_;
//     //
//     bool is_connected_;
// };

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
    QTcpSocket* socket_;
    QTimer* timer_;
    QString target_ip_;
    QString target_name_;
    bool is_connected_ = false;
    double factor_ic6_ = 0.000873114913702011;
    QList<bool> status_ = {};
    QList<int> activities_ = {};
    QList<double> frequencies_ = {};
    QByteArray ba_command_ = QByteArray::fromHex("1500534709535303005347095353040053470953530500E7");

  private:
    void connectToHost();

    void calcFreq(const QByteArray& resp);
    void calcInt(const QByteArray& resp);
    void calcStatus(const QByteArray& resp);
    void dataHandel(const QByteArray& data);

};

#endif // COMM_WORKER_H
