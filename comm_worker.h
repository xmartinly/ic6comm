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


class CommWorker: public QThread {
    Q_OBJECT
  public:
    CommWorker();
    ~CommWorker();

  private:
    QByteArray cmd_;
    bool is_loop_ = false;
    bool is_stop_ = false;
    QString inst_ip_;
    QString inst_name_;
    uint acq_intlv_ = 100;
    qint64 last_tm_ = 0;
    QTcpSocket* socket_ = nullptr;
    QTimer* acq_tmr_ = nullptr;
    CygResp resp_;
  private:


  public slots:
    void getResp();
    void sendCmd();

  public:
    void run();
    void setNewIntlv(uint new_intlv);
    void acqCtrl(bool is_stop);
    bool acqStatus() const;
    void setCmd(const QByteArray& cmd);

    void setIpAddr(const QString& ip);

    bool connectInst(const QString& ip);
    bool disconnectInst();

  signals:
    void sendInstResp(const QByteArray&);


};

#endif // COMM_WORKER_H
