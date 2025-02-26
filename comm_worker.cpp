#include "comm_worker.h"


CommWorker::CommWorker() {
    last_tm_ = QDateTime::currentMSecsSinceEpoch();
}

CommWorker::~CommWorker() {
}

bool CommWorker::connectInst(const QString& ip) {
    // This is available in all editors.
    qDebug() << __FUNCTION__ << ip;
    socket_->connectToHost(ip, 2101);
    socket_->waitForConnected(500);
    return socket_->isOpen();
}

bool CommWorker::disconnectInst() {
    if(!socket_->isOpen()) {
        return true;
    }
    return socket_->waitForDisconnected(100);
}

void CommWorker::getResp() {
    QByteArray ba_resp;
    while (socket_->waitForReadyRead(50)) {
        ba_resp = socket_->readAll();
    }
    // This is available in all editors.
    qDebug() << __FUNCTION__ << ba_resp;
}

void CommWorker::sendCmd() {
    if(socket_ == nullptr || !socket_->isOpen() || is_stop_) {
        return;
    }
    socket_->write(cmd_);
    socket_->flush();
}



void CommWorker::run() {
    acq_tmr_ = new QTimer();
    socket_ = new QTcpSocket();
    socket_->setProxy(QNetworkProxy::NoProxy);
    connect(acq_tmr_, &QTimer::timeout, this, &CommWorker::sendCmd, Qt::DirectConnection);
    connect(socket_, &QTcpSocket::readyRead, this, &CommWorker::getResp);
    exec();
}

void CommWorker::setNewIntlv(uint new_intlv) {
    acq_tmr_->setInterval(new_intlv);
}

void CommWorker::acqCtrl(bool is_stop) {
    is_stop_ = is_stop;
    if(!is_stop_) {
        acq_tmr_->start();
    } else {
        acq_tmr_->stop();
    }
}

bool CommWorker::acqStatus() const {
    return is_stop_;
}

void CommWorker::setCmd(const QByteArray& cmd) {
    cmd_ = cmd;
}

void CommWorker::setIpAddr(const QString& ip) {
    inst_ip_ = ip;
}



