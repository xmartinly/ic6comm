#include "comm_worker.h"

#include <QNetworkProxy>

///
/// \brief CommWorker::CommWorker
/// \param ip
/// \param name
/// \param parent
///
CommWorker::CommWorker(const QString& ip, const QString& name, QObject* parent)
    : QObject(parent), targetIp(ip), targetName(name) {
    socket = new QTcpSocket(this);
    socket->setProxy(QNetworkProxy::NoProxy);
    timer = new QTimer(this);
    // 连接信号槽
    connect(socket, &QTcpSocket::connected, this, &CommWorker::handleConnected);
    connect(socket, &QTcpSocket::readyRead, this, &CommWorker::handleReadyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &CommWorker::handleError);
    connect(timer, &QTimer::timeout, this, &CommWorker::work);
}

///
/// \brief CommWorker::~CommWorker
///
CommWorker::~CommWorker() {
    stopWork();
    socket->deleteLater();
    timer->deleteLater();
    this->deleteLater();
}

///
/// \brief CommWorker::startWork
/// \param interval
///
void CommWorker::startWork(int interval) {
    if(!timer->isActive()) {
        connectToHost();
        timer->start(interval); // 定时执行任务
    }
}

///
/// \brief CommWorker::stopWork
///
void CommWorker::stopWork() {
    isConnected = false;
    timer->stop();
    if(socket->state() == QTcpSocket::ConnectedState) {
        socket->disconnectFromHost();
        // This is available in all editors.
        qDebug() << __FUNCTION__ << socket->state() << timer->isActive();
    }
}

///
/// \brief CommWorker::work
///
void CommWorker::work() {
    if(isConnected) {
        socket->write(Helper::BA_SNRDATA);
        socket->flush();
    } else {
        // This is available in all editors.
        qDebug() << __FUNCTION__ << isConnected;
        connectToHost(); // 尝试重新连接
    }
}

///
/// \brief CommWorker::connectToHost
///
void CommWorker::connectToHost() {
    if(socket->state() == QTcpSocket::UnconnectedState) {
        socket->connectToHost(targetIp, 2101); // 假设端口2101
        socket->waitForConnected(100);
    }
}

///
/// \brief CommWorker::handleConnected
///
void CommWorker::handleConnected() {
    isConnected = true;
    qDebug() << "Connected to" << targetIp;
}

///
/// \brief CommWorker::handleReadyRead
///
void CommWorker::handleReadyRead() {
    // socket->waitForReadyRead(100);
    QByteArray data = socket->readAll();
    emit dataReceived(data, targetName); // 转发数据到主线程
}

///
/// \brief CommWorker::handleError
/// \param error
///
void CommWorker::handleError(QAbstractSocket::SocketError error) {
    QString errorMsg = socket->errorString();
    emit errorOccurred(errorMsg, targetIp);
    isConnected = false;
}
