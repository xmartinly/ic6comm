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
    // This is available in all editors.
    // qDebug() << __FUNCTION__ << QThread::currentThreadId();
    if(isConnected) {
        socket->write(ba_test);
        socket->flush();
    } else {
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

void CommWorker::dataHandel(const QByteArray& data) {
    bool checksumValid = false;
    if (data.size() >= 4) { // 至少需要4字节才能计算
        // 提取范围：索引2（第三个字节）到倒数第二个字节
        int start = 2;
        int end = data.size() - 2;
        quint8 sum = 0;
        // 计算校验和
        for (int i = start; i <= end; ++i) {
            sum += static_cast<quint8>(data[i]); // 转换为无符号字节
        }
        // 获取最后一个字节的值
        quint8 lastByte = static_cast<quint8>(data.back());
        // 验证校验和
        checksumValid = (sum == lastByte);
    }
    // ------------------------
    // 任务2：按动态分隔符分割
    // ------------------------
    QList<QByteArray> splitParts;
    if (checksumValid && data.size() >= 7) { // 至少需要7字节才能提取分隔符
        // 提取分隔符（第5-7字节，索引4-6）
        QByteArray delimiter = data.mid(4, 3);
        // 提取待分割数据：从第8字节（索引7）到倒数第二个字节
        int splitStart = 7;
        int splitEnd = data.size() - 2;
        QByteArray splitData = data.mid(splitStart, splitEnd - splitStart + 1);
        // 按分隔符分割
        int pos = 0;
        while (pos < splitData.size()) {
            int next = splitData.indexOf(delimiter, pos);
            if (next == -1) {
                splitParts.append(splitData.mid(pos));
                break;
            }
            splitParts.append(splitData.mid(pos, next - pos));
            pos = next + delimiter.size(); // 跳过分隔符
        }
        // 过滤空元素
        splitParts.erase(
            std::remove_if(splitParts.begin(), splitParts.end(),
        [](const QByteArray & part) {
            return part.isEmpty();
        }),
        splitParts.end()
        );
        // 输出分割结果
        // int list_len = splitParts.count();
    }
    if(splitParts.count() != 3) {
        return;
    }
    // This is available in all editors.
    calcStatus(splitParts.at(0));
    calcInt(splitParts.at(2));
    calcFreq(splitParts.at(1));
}

uint CommWorker::calcMsgLen(const QByteArray& len_ba) {
    if(len_ba.length() != 2) {
        return 0;
    }
    QByteArray new_len;
    new_len.resize(2);
    new_len[0] = len_ba.at(1);
    new_len[1] = len_ba.at(0);
    bool cvt_ok;
    uint res = new_len.toHex().toUInt(&cvt_ok, 16);
    return cvt_ok ? res : 0;
}

///
/// \brief CommWorker::calcCks
/// \param ba_msg
/// \return
///
QByteArray CommWorker::calcMsg(const QByteArray& resp) {
    uint cks   = resp.back() & 0xff;
    QByteArray msg_len_ba = resp.mid(0, 2);
    uint cks_ = 0;
    int msg_len = calcMsgLen(msg_len_ba);
    QByteArray resp_ = resp.mid(2, msg_len);
    foreach (auto c, resp_) {
        cks_ += static_cast<uint>(c);
    }
    cks_ &= 0xff;
    if(cks_ != cks) {
        return {};
    }
    // resp.back()
    return resp_.mid(2, -1);
}

///
/// \brief CommWorker::calcFreq
/// \param resp
/// \return
///
void CommWorker::calcFreq(const QByteArray& resp) {
    frequencies_ = {};
    int resp_len = resp.length();
    if(resp_len % 8) {
        return;
    }
    for (int var = 0; var < resp_len; ++var) {
        QByteArray freq_ba = resp.mid(8 * var, 8);
        QDataStream stream(freq_ba);
        stream.setByteOrder(QDataStream::LittleEndian);
        qint64 value;
        stream >> value;
        frequencies_.append( value * factor_ic6);
    }
}

///
/// \brief CommWorker::calcInt
/// \param resp
/// \return
///
void CommWorker::calcInt(const QByteArray& resp) {
    activities_ = {};
    int resp_len = resp.length();
    if(resp_len % 4) {
        return;
    }
    for (int var = 0; var < resp_len; ++var) {
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
    status_ = {};
    foreach (auto c, resp) {
        status_.append( (c & 1) == 0);
    }
}

///
/// \brief CommWorker::handleConnected
///
void CommWorker::handleConnected() {
    isConnected = true;
    // qDebug() << "Connected to" << targetIp;
}

///
/// \brief CommWorker::handleReadyRead
///
void CommWorker::handleReadyRead() {
    // socket->waitForReadyRead(100);
    QByteArray data = socket->readAll();
    status_ = {};
    activities_ = {};
    frequencies_ = {};
    dataHandel(data);
    emit sendData(status_, frequencies_, activities_, targetName);
    // emit dataReceived(data, targetName); // 转发数据到主线程
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
