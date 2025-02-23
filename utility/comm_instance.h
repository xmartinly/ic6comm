#ifndef COMM_INSTANCE_H
#define COMM_INSTANCE_H

#include <QObject>
class CommInstance: public QObject {
    Q_OBJECT

  public:


  public:
    static CommInstance* getInstance();
    ~CommInstance();

    bool setupConn(const QString& ip, uint port = 2101);
    void setCmd(const QByteArray& cmd);
    void setupIntvl(uint interval = 250);
    void oneTimeCmd(const QByteArray& cmd, Utility::CygEnums::CommandGroup group = Utility::CygEnums::HelloMsg);
    void stopLoop();

    uint errCount() const;
    void resetErrCount();
    uint commCount() const;
    void resetCommCount();
    quint32 bytesRecv() const;
    void resetBytesRecv();
    quint32 bytesSend() const;
    void resetBytesSend();
    void resetCommResp();

    bool isAcquiring() const;
    bool connState() const;
    int commDelay() const;

    const Utility::CygEnums::CommandType commandType() const;
    const QString cmdHex() const;
    const QString commConnInfo() const;
    const QString commResp() const;
    const quint32 commBytesTotal();

    void setRunningTime(qint64 r_t);
    qint64 getRemTime() const;

  private:
    void respHandle(const QByteArray& resp_data, const QDateTime& curr_time);
    void stopDecide(const QDateTime& curr_time) ;


  private:
    CommInstance(QObject* parent = nullptr);
    CommInstance(CommInstance&) = delete;
    CommInstance& operator=(CommInstance ci) = delete;

    class Private;
    friend class Private;
    Private* friend_d_;

    QTimer* comm_tmr_ = nullptr;
    QTimer* timeout_tmr_ = nullptr;
    QByteArray cmd_array_;
    bool in_tm_model_ = false;
    bool read_fin_ = false;
    uint err_cnt_ = 0;
    uint comm_cnt_ = 0;
    quint32 bytes_recv_ = 0;
    quint32 bytes_send_ = 0;

    qint64 running_tm_ = 0;
    const qint64 max_running_tm_ = 2592000;

    uint cmd_len_ = 0;
    uint resp_len_ = 0;

    int comm_intvl_ = 0;
    int comm_delay_sum_ = 0;
    int comm_tmout_count_ = 0;

    qint64 comm_start_tm_ = 0;

    QString resp_data_;
    Utility::CygEnums::CommandType cmd_type_;
    Utility::CygEnums::CommandGroup cmd_group_;


  private slots:
    void recvResp();
    void sendCmd();
    void tcpErr(QAbstractSocket::SocketError socketError);
    void commTimeout();

  signals:
    void sendRespErr(const QString& data);
    void sendTimelyResp(int group, const QByteArray& data);
};


#endif // COMM_INSTANCE_H
