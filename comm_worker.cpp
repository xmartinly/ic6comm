﻿#include "comm_worker.h"

#include <QNetworkProxy>

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

CommWorker::~CommWorker() {
    stopWork();
    socket->deleteLater();
    timer->deleteLater();
    this->deleteLater();
}

void CommWorker::startWork(int interval) {
    if(!timer->isActive()) {
        connectToHost();
        timer->start(interval); // 定时执行任务
    }
}

void CommWorker::stopWork() {
    isConnected = false;
    timer->stop();
    if(socket->state() == QTcpSocket::ConnectedState) {
        socket->disconnectFromHost();
        // This is available in all editors.
        qDebug() << __FUNCTION__ << socket->state() << timer->isActive();
    }
}

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

void CommWorker::connectToHost() {
    if(socket->state() == QTcpSocket::UnconnectedState) {
        socket->connectToHost(targetIp, 2101); // 假设端口2101
        socket->waitForConnected(100);
    }
}

void CommWorker::handleConnected() {
    isConnected = true;
    qDebug() << "Connected to" << targetIp;
}

void CommWorker::handleReadyRead() {
    // socket->waitForReadyRead(100);
    QByteArray data = socket->readAll();
    emit dataReceived(data, targetName); // 转发数据到主线程
}

void CommWorker::handleError(QAbstractSocket::SocketError error) {
    QString errorMsg = socket->errorString();
    emit errorOccurred(errorMsg, targetIp);
    isConnected = false;
}
