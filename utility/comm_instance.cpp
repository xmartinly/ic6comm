#include "comm_instance.h"

///
/// \brief The CommInstance::Private class. friend class for tcpsocket instance
///
class CommInstance::Private {
  public:
    Private(CommInstance* q) : socket_(new QTcpSocket(q)) {
        socket_->setProxy(QNetworkProxy::NoProxy);
    }
    QTcpSocket* socket_;
};


///
/// \brief CommInstance::CommInstance. constructor for CommInstance
/// \param parent
///
CommInstance::CommInstance(QObject* parent) : QObject(parent) {
    comm_tmr_ = new QTimer(this);
    timeout_tmr_ = new QTimer(this);
    connect(comm_tmr_, &QTimer::timeout, this, &CommInstance::sendCmd);
    connect(timeout_tmr_, &QTimer::timeout, this, &CommInstance::commTimeout);
    timeout_tmr_->setInterval(500);
    friend_d_ = new CommInstance::Private(this);
    connect(friend_d_->socket_, &QTcpSocket::readyRead, this,
            &CommInstance::recvResp);
    connect(friend_d_->socket_, &QTcpSocket::errorOccurred, this,
            &CommInstance::tcpErr);
}

///
/// \brief CommInstance::recvResp. receive message handle
///
void CommInstance::recvResp() {
    read_fin_ = false;
    QDateTime curr_time = QDateTime::currentDateTime();
    QByteArray ba_resp;
    while(friend_d_->socket_->bytesAvailable()) {
        ba_resp += friend_d_->socket_->readAll();
    }
    respHandle(ba_resp, curr_time);
    stopDecide(curr_time);
    read_fin_ = true;
}

///
/// \brief CommInstance::respHandle
/// \param resp_data
/// \param curr_time
///
void CommInstance::respHandle(const QByteArray& resp_data, const QDateTime& curr_time)  {
    bytes_recv_ += resp_data.length();
    InstMsg::InstResponse inst_resp =  InstMsg::InstResponse(resp_data);//InstResponse instance
    auto err = inst_resp.respStatus();
    auto stat_str = inst_resp.respStatusString();
    if(err != Utility::CygEnums::NoErr) {
        emit sendRespErr("Response Error:" + stat_str);
        err_cnt_++;
    }
    //communicate delay sum
    comm_delay_sum_ += (curr_time.toMSecsSinceEpoch() - comm_start_tm_);
    comm_start_tm_ = curr_time.toMSecsSinceEpoch();
    if(cmd_type_ == Utility::CygEnums::TimelyCmd) {
        cmd_type_ = Utility::CygEnums::LoopCmd;
        qDebug() << __FUNCTION__ << Helper::calcInt(inst_resp.respMsg(true));
        emit sendTimelyResp(static_cast<int>(cmd_group_), inst_resp.respMsg(true));
    } else {
        resp_data_ += curr_time.toString("yyyy-MM-dd hh:mm:ss.zzz") + ",";
        resp_data_ += stat_str + "," + Helper::hexFormat(resp_data) + "\n";
    }
}

///
/// \brief CommInstance::stopDecide
/// \param curr_time
///
void CommInstance::stopDecide(const QDateTime& curr_time) {
    if(getRemTime() < 0) {
        stopLoop();
        return;
    }
    if(in_tm_model_) { //infinite loop, when set timer interval to 0
        QTimer::singleShot(5, this, &CommInstance::sendCmd);
    }
}


///
/// \brief CommInstance::sendCmd. send command slot function
///
void CommInstance::sendCmd() {
    if(!connState()) {
        return;
    }
    while(!read_fin_) { //return if read not finished
        return;
    }
    read_fin_ = false;
    friend_d_->socket_->write(cmd_array_);
    friend_d_->socket_->flush();
    comm_cnt_++;
    bytes_send_ += cmd_len_;
}

///
/// \brief CommInstance::tcpErr
/// \param socketError
///
void CommInstance::tcpErr(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    emit sendRespErr("Connection Error:" + friend_d_->socket_->errorString());
}

void CommInstance::commTimeout() {
    comm_tmout_count_++;
    if(comm_tmout_count_ > 5) {
        stopLoop();
        emit sendRespErr("Communication time time out exceed to 5, stop.");
    }
}

///
/// \brief CommInstance::~CommInstance
///
CommInstance::~CommInstance() {
    if(comm_tmr_->isActive()) {
        comm_tmr_->stop();
    }
    delete comm_tmr_;
    comm_tmr_ = nullptr;
    if(timeout_tmr_->isActive()) {
        timeout_tmr_->stop();
    }
    delete timeout_tmr_;
    timeout_tmr_ = nullptr;
    friend_d_->socket_->disconnectFromHost();
    delete friend_d_;
    friend_d_ = nullptr;
}

///
/// \brief CommInstance::getInstance
/// \return
///
CommInstance* CommInstance::getInstance() {
    static CommInstance comm;
    return &comm;
}

///
/// \brief CommInstance::setupConn
/// \param ip
/// \param port
/// \return bool. return true if connection success built.
///
bool CommInstance::setupConn(const QString& ip, uint port) {
    if(friend_d_->socket_->state() == QTcpSocket::ConnectedState) {
        friend_d_->socket_->disconnectFromHost();
        friend_d_->socket_->close();
    }
    friend_d_->socket_->connectToHost(ip, port);
    if(friend_d_->socket_->waitForConnected(200)) {
        comm_tmout_count_ = 0;
        return connState();
    }
    if(friend_d_->socket_->state() != QTcpSocket::ConnectedState) {
        QString err_str = "Can't connect to: " + QString("%1:%2 ").arg(ip, QString::number(port)) + friend_d_->socket_->errorString();
        emit sendRespErr(err_str);
        friend_d_->socket_->disconnectFromHost();
        friend_d_->socket_->close();
    }
    return false;
}

///
/// \brief CommInstance::setCmd. setup command array
/// \param QByteArray cmd
///
void CommInstance::setCmd(const QByteArray& cmd) {
    this->cmd_array_ = cmd;
    cmd_len_ = cmd.length();
}


///
/// \brief CommInstance::setupIntvl. setup new acquire timer intervel
/// \param uint interval
///
void CommInstance::setupIntvl(uint interval) {
    comm_intvl_ = interval;
    comm_delay_sum_ = 0;
    comm_start_tm_ = QDateTime::currentMSecsSinceEpoch();
    if(!interval) { //in time mode
        in_tm_model_ = true;
        QTimer::singleShot(1000, this, &CommInstance::sendCmd);//initiator
        return;
    }
    comm_tmr_->start(interval);
}

///
/// \brief CommInstance::oneTimeCmd. send command one time
/// \param QByteArray cmd
///
void CommInstance::oneTimeCmd(const QByteArray& cmd, CygEnums::CommandGroup group) {
    if(!connState()) {
        return;
    }
    bool in_tm_model_temp = in_tm_model_;
    bool comm_tmr_active = comm_tmr_->isActive();
    if(in_tm_model_temp && comm_tmr_active) {
        in_tm_model_ = false;
        comm_tmr_->stop();
    }
    cmd_type_ = CygEnums::TimelyCmd;
    cmd_group_ = group;
    friend_d_->socket_->write(cmd);
    friend_d_->socket_->flush();
    while(!read_fin_) {
        return;
    }
    in_tm_model_ = in_tm_model_temp;
    if(comm_tmr_active) {
        comm_tmr_->start();
    }
}

///
/// \brief CommInstance::stopLoop. stop command send loop
///
void CommInstance::stopLoop() {
    if(comm_tmr_->isActive()) {
        comm_tmr_->stop();
    }
    in_tm_model_ = false;
}

///
/// \brief CommInstance::errCount. get error count
/// \return
///
uint CommInstance::errCount() const {
    return err_cnt_;
}

///
/// \brief CommInstance::resetErrCount
///
void CommInstance::resetErrCount() {
    err_cnt_ = 0;
}

///
/// \brief CommInstance::commCount. get communicate count
/// \return
///
uint CommInstance::commCount() const {
    return comm_cnt_;
}

///
/// \brief CommInstance::resetCommCount
///
void CommInstance::resetCommCount() {
    comm_cnt_ = 0;
}

///
/// \brief CommInstance::bytesRecv
/// \return
///
quint32 CommInstance::bytesRecv() const {
    return bytes_recv_;
}

///
/// \brief CommInstance::resetBytesRecv
///
void CommInstance::resetBytesRecv() {
    bytes_recv_ = 0;
}

///
/// \brief CommInstance::bytesSend
/// \return
///
quint32 CommInstance::bytesSend() const {
    return bytes_send_;
}

///
/// \brief CommInstance::setRunningTime
/// \param r_t
///
void CommInstance::setRunningTime(qint64 r_t) {
    qint64 curr_sec =  QDateTime::currentSecsSinceEpoch();
    running_tm_ = r_t ? (r_t + curr_sec) :  (max_running_tm_ + curr_sec);
}

///
/// \brief CommInstance::getRemTime
/// \return
///
qint64 CommInstance::getRemTime() const {
    return running_tm_ - QDateTime::currentSecsSinceEpoch();
}


///
/// \brief CommInstance::resetBytesSend
///
void CommInstance::resetBytesSend() {
    bytes_send_ = 0;
}

///
/// \brief CommInstance::commBytesTotal
/// \return
///
const quint32 CommInstance::commBytesTotal() {
    return  bytes_send_ + bytes_recv_;
}

///
/// \brief CommInstance::isAcquiring
/// \return
///
bool CommInstance::isAcquiring() const {
    return comm_tmr_->isActive() || in_tm_model_;
}

///
/// \brief CommInstance::connState
/// \return
///
bool CommInstance::connState() const {
    return friend_d_->socket_->isOpen() && friend_d_->socket_->isReadable() && friend_d_->socket_->isWritable();
}

///
/// \brief CommInstance::cmdHex. get command hex string
/// \return
///
const QString CommInstance::cmdHex() const {
    if(cmd_array_.length() > 0) {
        return Helper::hexFormat(cmd_array_);
    }
    return "";
}

///
/// \brief CommInstance::commConnInfo. connection information, "ip:port"
/// \return
///
const QString CommInstance::commConnInfo() const {
    if(!friend_d_->socket_->isOpen()) {
        return "";
    }
    return friend_d_->socket_->peerAddress().toString() + ":" + QString::number(friend_d_->socket_->peerPort());
}

///
/// \brief CommInstance::commResp. get response string
/// \return
///
const QString CommInstance::commResp() const {
    return resp_data_;
}

void CommInstance::resetCommResp() {
    resp_data_ = "";
}

///
/// \brief CommInstance::commDelay. get communication delay avg value.
/// \return
///
int CommInstance::commDelay() const {
    int delay = 0;
    if(comm_cnt_ > 0) {
        delay = comm_delay_sum_ / comm_cnt_;
    }
    return delay;
}


///
/// \brief CommInstance::commandType
/// \return
///
const Utility::CygEnums::CommandType CommInstance::commandType() const {
    return cmd_type_;
}



