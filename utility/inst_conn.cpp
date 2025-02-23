#include "inst_conn.h"

using namespace Utility;

///
/// \brief SerConn::SerConn. deep copy constructor
/// \param ser
///
SerConn::SerConn(const SerConn& ser) {
    conn_way_ = ser.conn_way_;
    conn_port_bdrt_ = ser.conn_port_bdrt_;
    ser_ = new QSerialPort(ser.ser_);
}

///
/// \brief SerConn::~SerConn
///
SerConn::~SerConn() {
    qDebug() << __FUNCTION__;
    if(ser_->isOpen() && ser_->isReadable() && ser_->isWritable()) {
        disconnect(ser_, &QSerialPort::readyRead, this, &SerConn::recvResp);
        ser_->clear();
        ser_->close();
        delete ser_;
        ser_ = nullptr;
    }
}

///
/// \brief SerConn::delConn
///
void SerConn::delConn() {
    if(connState()) {
        disconnect(ser_, &QSerialPort::readyRead, this, &SerConn::recvResp);
        ser_->clear();
        ser_->close();
        delete ser_;
        ser_ = nullptr;
    }
}

///
/// \brief SerConn::setupConn
/// \return
///
bool SerConn::setupConn() {
    if(!verifyParam()) {
        return false;
    }
    delConn();
    ser_ = new QSerialPort(this);
    connect(ser_, &QSerialPort::readyRead, this, &SerConn::recvResp);
    ser_->setPortName(conn_way_);
    ser_->setBaudRate(conn_port_bdrt_);
    ser_->setDataBits(QSerialPort::Data8);
    ser_->setParity(QSerialPort::NoParity);
    ser_->setStopBits(QSerialPort::OneStop);
    ser_->setFlowControl(QSerialPort::NoFlowControl);
    if(ser_->open(QSerialPort::ReadWrite)) {
        return connState();
    }
    QString err_str = "Can't connect to: " + QString("%1:%2 ").arg(conn_way_, QString::number(conn_port_bdrt_)) + ser_->errorString();
    emit sendConnErr(err_str);
    ser_->close();
    return false;
}

///
/// \brief SerConn::connState
/// \return
///
bool SerConn::connState() {
    if(ser_ == nullptr) {
        return false;
    }
    return ser_->isOpen() && ser_->isReadable() && ser_->isWritable();
}

///
/// \brief SerConn::verifyParam
/// \return
///
bool SerConn::verifyParam() {
    if (!conn_way_.contains("COM")) {
        emit sendConnErr("The serial port number is incorrect.");
        return false;
    }
    if(!ser_bdrt_list_.contains(conn_port_bdrt_)) {
        emit sendConnErr("The baud rate is out of range.");
        return false;
    }
    return true;
}

///
/// \brief SerConn::recvResp
///
void SerConn::recvResp() {
    read_fin_ = false;
    QByteArray ba_resp;
    while(ser_->bytesAvailable()) {
        ba_resp += ser_->readAll();
    }
    emit sendResp(ba_resp, QDateTime::currentMSecsSinceEpoch());
    read_fin_ = true;
}

///
/// \brief SerConn::sendCmd
/// \param cmd
///
void SerConn::sendCmd(const QByteArray& cmd) {
    if(connState()) {
        emit sendConnErr("Communication is unavailable or not open.");
        return;
    }
    while(!read_fin_) {
        return;
    }
    ser_->write(cmd);
    ser_->flush();
}

///
/// \brief TcpConn::TcpConn
/// \param tcp
///
TcpConn::TcpConn(const TcpConn& tcp) {
    conn_way_ = tcp.conn_way_;
    conn_port_bdrt_ = tcp.conn_port_bdrt_;
    socket_ = new QTcpSocket(tcp.socket_);
}

///
/// \brief TcpConn::~TcpConn
///
TcpConn::~TcpConn() {
    qDebug() << __FUNCTION__;
    if(socket_->isOpen() && socket_->isReadable() && socket_->isWritable()) {
        disconnect(socket_, &QTcpSocket::readyRead, this, &TcpConn::recvResp);
        socket_->disconnectFromHost();
        socket_->close();
        delete socket_;
        socket_ = nullptr;
    }
}

///
/// \brief TcpConn::delConn
///
void TcpConn::delConn() {
    if(connState()) {
        disconnect(socket_, &QTcpSocket::readyRead, this, &TcpConn::recvResp);
        socket_->disconnectFromHost();
        socket_->close();
        delete socket_;
        socket_ = nullptr;
    }
}

///
/// \brief TcpConn::setupConn
/// \return
///
bool TcpConn::setupConn() {
    if(!verifyParam()) {
        return false;
    }
    // delConn();
    socket_ = new QTcpSocket(this);
    socket_->setProxy(QNetworkProxy::NoProxy);
    connect(socket_, &QTcpSocket::readyRead, this, &TcpConn::recvResp);
    socket_->connectToHost(conn_way_, conn_port_bdrt_);
    if(socket_->waitForConnected(200)) {
        return connState();
    }
    if(socket_->state() != QTcpSocket::ConnectedState) {
        QString err_str = "Can't connect to: " + QString("%1:%2 ").arg(conn_way_, QString::number(conn_port_bdrt_)) + socket_->errorString();
        emit sendConnErr(err_str);
        socket_->disconnectFromHost();
        socket_->close();
    }
    return false;
}

///
/// \brief TcpConn::connState
/// \return
///
bool TcpConn::connState() {
    if(socket_ == nullptr) {
        return false;
    }
    return socket_->isOpen() && socket_->isReadable() && socket_->isWritable();
}

///
/// \brief TcpConn::verifyParam
/// \return
///
bool TcpConn::verifyParam() {
    QHostAddress test_ip;
    if (!test_ip.setAddress(conn_way_)) {
        emit sendConnErr("The IP address format is incorrect.");
        return false;
    }
    if(conn_port_bdrt_ > 65535 || conn_port_bdrt_ < 1) {
        emit sendConnErr("The port number is out of range.");
        return false;
    }
    return true;
}

///
/// \brief TcpConn::recvResp
///
void TcpConn::recvResp() {
    read_fin_ = false;
    QByteArray ba_resp;
    while(socket_->bytesAvailable()) {
        ba_resp += socket_->readAll();
    }
    emit sendResp(ba_resp, QDateTime::currentMSecsSinceEpoch());
    read_fin_ = true;
}

///
/// \brief TcpConn::sendCmd
/// \param cmd
///
void TcpConn::sendCmd(const QByteArray& cmd) {
    if(connState()) {
        emit sendConnErr("Communication is unavailable or not open.");
        return;
    }
    while(!read_fin_) {
        return;
    }
    socket_->write(cmd);
    socket_->flush();
}


InstConn::InstConn(const QString& conn_way, int conn_port_bdrt, CygEnums::ConnType conn_type, QObject* parent)
    : QObject(parent),
      conn_way_(conn_way),
      conn_port_bdrt_(conn_port_bdrt),
      conn_type_(conn_type) {
}
