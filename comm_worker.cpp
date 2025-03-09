#include "comm_worker.h"

#include <QNetworkProxy>

///
/// \brief CommWorker::CommWorker
/// \param ip
/// \param name
/// \param parent
///
CommWorker::CommWorker(const QString& ip, const QString& name, QObject* parent)
    : QObject(parent), target_ip_(ip), target_name_(name) {
    socket_ = new QTcpSocket(this);
    socket_->setProxy(QNetworkProxy::NoProxy);
    timer_ = new QTimer(this);
    // 连接信号槽
    connect(socket_, &QTcpSocket::connected, this, &CommWorker::handleConnected);
    connect(socket_, &QTcpSocket::readyRead, this, &CommWorker::handleReadyRead);
    connect(socket_, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &CommWorker::handleError);
    connect(timer_, &QTimer::timeout, this, &CommWorker::work);
}

///
/// \brief CommWorker::~CommWorker
///
CommWorker::~CommWorker() {
    stopWork();
    socket_->deleteLater();
    timer_->deleteLater();
    this->deleteLater();
}

///
/// \brief CommWorker::startWork
/// \param interval
///
void CommWorker::startWork(int interval) {
    if(!timer_->isActive()) {
        connectToHost();
        timer_->start(interval); // 定时执行任务
    }
}

///
/// \brief CommWorker::stopWork
///
void CommWorker::stopWork() {
    is_connected_ = false;
    timer_->stop();
    if(socket_->state() == QTcpSocket::ConnectedState) {
        socket_->disconnectFromHost();
        // This is available in all editors.
        qDebug() << __FUNCTION__ << socket_->state() << timer_->isActive();
    }
}

///
/// \brief CommWorker::work
///
void CommWorker::work() {
    // This is available in all editors.
    // qDebug() << __FUNCTION__ << QThread::currentThreadId();
    if(is_connected_) {
        socket_->write(ba_command_);
        socket_->flush();
    } else {
        connectToHost(); // 尝试重新连接
    }
}

///
/// \brief CommWorker::connectToHost
///
void CommWorker::connectToHost() {
    if(socket_->state() == QTcpSocket::UnconnectedState) {
        socket_->connectToHost(target_ip_, 2101); // 假设端口2101
        socket_->waitForConnected(100);
    }
}

void CommWorker::dataHandel(const QByteArray& data) {
    bool checksumValid = false;
    if (data.size() >= 4) {
        int start = 2;
        int end = data.size() - 2;
        quint8 sum = 0;
        for (int i = start; i <= end; ++i) {
            sum += static_cast<quint8>(data[i]);
        }
        quint8 lastByte = static_cast<quint8>(data.back());
        checksumValid = (sum == lastByte);
    }
    QList<QByteArray> splitParts;
    if (checksumValid && data.size() >= 7) {
        QByteArray delimiter = data.mid(4, 3);
        int splitStart = 7;
        int splitEnd = data.size() - 2;
        QByteArray splitData = data.mid(splitStart, splitEnd - splitStart + 1);
        int pos = 0;
        while (pos < splitData.size()) {
            int next = splitData.indexOf(delimiter, pos);
            if (next == -1) {
                splitParts.append(splitData.mid(pos));
                break;
            }
            splitParts.append(splitData.mid(pos, next - pos));
            pos = next + delimiter.size();
        }
        splitParts.erase(
            std::remove_if(splitParts.begin(), splitParts.end(),
        [](const QByteArray & part) {
            return part.isEmpty();
        }),
        splitParts.end()
        );
    }
    if(splitParts.count() != 3) {
        return;
    }
    calcStatus(splitParts.at(0));
    calcFreq(splitParts.at(1));
    calcInt(splitParts.at(2));
}



///
/// \brief CommWorker::calcFreq
/// \param resp
/// \return
///
void CommWorker::calcFreq(const QByteArray& resp) {
    frequencies_.clear();
    int resp_len = resp.length();
    int freq_ba_len = resp_len / 8;
    if(resp_len % 8) {
        return;
    }
    for (int var = 0; var < freq_ba_len; ++var) {
        QByteArray freq_ba = resp.mid(8 * var, 8);
        QDataStream stream(freq_ba);
        stream.setByteOrder(QDataStream::LittleEndian);
        qint64 value;
        stream >> value;
        frequencies_.append( value * factor_ic6_);
    }
}

///
/// \brief CommWorker::calcInt
/// \param resp
/// \return
///
void CommWorker::calcInt(const QByteArray& resp) {
    activities_ .clear();
    int resp_len = resp.length();
    int act_ba_len = resp_len / 4;
    if(resp_len % 4) {
        return;
    }
    for (int var = 0; var < act_ba_len; ++var) {
        QByteArray act_ba = resp.mid(4 * var, 4);
        QDataStream stream(act_ba);
        stream.setByteOrder(QDataStream::LittleEndian);
        int value;
        stream >> value;
        activities_.append(value);
    }
}

///
/// \brief CommWorker::calcStatus
/// \param stat
/// \return
///
void CommWorker::calcStatus(const QByteArray& resp) {
    status_.clear();
    foreach (auto c, resp) {
        status_.append( (c & 1) == 0);
    }
}

///
/// \brief CommWorker::handleConnected
///
void CommWorker::handleConnected() {
    is_connected_ = true;
    // qDebug() << "Connected to" << targetIp;
}

///
/// \brief CommWorker::handleReadyRead
///
void CommWorker::handleReadyRead() {
    QByteArray data;
    while(socket_->bytesAvailable()) {
        data += socket_->readAll();
    }
    if(data.length() < 7) {
        return;
    }
    dataHandel(data);
    emit sendData(status_, frequencies_, activities_, target_name_);
}

///
/// \brief CommWorker::handleError
/// \param error
///
void CommWorker::handleError(QAbstractSocket::SocketError error) {
    qDebug() << error;
    QString errorMsg = socket_->errorString();
    emit errorOccurred(errorMsg, target_ip_);
    is_connected_ = false;
}
