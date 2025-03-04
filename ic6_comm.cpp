#include "ic6_comm.h"
#include "ui_ic6_comm.h"

IC6Comm::IC6Comm(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::IC6Comm) {
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 1);
    setStatusbar();
    statusBar()->showMessage("ShowMessage", 2000);
    initListTble();
    ui->wd_ip1->setIP("172.16.6.136");
    ui->wd_ip2->setIP("172.16.6.137");
}

IC6Comm::~IC6Comm() {
    foreach (auto thread, threads) {
        thread->quit();
        thread->wait();
        delete thread;
    }
    delete ui;
}

void IC6Comm::on_act_comm_setting_triggered() {
}


void IC6Comm::on_act_exit_triggered() {
    this->close();
}


void IC6Comm::on_act_man_triggered() {
}


void IC6Comm::on_act_about_triggered() {
}


void IC6Comm::on_act_lang_triggered() {
}


void IC6Comm::on_tb_start1_clicked() {
    QString ip = ui->wd_ip1->getIP();
    QString name = ui->le_name1->text();
    uint intvl = ui->cb_intvl1->currentText().toUInt();
    ui->wd_ip1->setDisabled(true);
    ui->le_name1->setDisabled(true);
    ui->cb_intvl1->setDisabled(true);
    startAcq(ip, name, intvl);
}

void IC6Comm::on_tb_start2_clicked() {
    QString ip = ui->wd_ip2->getIP();
    QString name = ui->le_name2->text();
    uint intvl = ui->cb_intvl2->currentText().toUInt();
    ui->wd_ip2->setDisabled(true);
    ui->le_name2->setDisabled(true);
    ui->cb_intvl2->setDisabled(true);
    startAcq(ip, name, intvl);
}


void IC6Comm::on_tb_stop1_clicked() {
    ui->wd_ip1->setDisabled(false);
    ui->le_name1->setDisabled(false);
    ui->cb_intvl1->setDisabled(false);
}
void IC6Comm::on_tb_stop2_clicked() {
    ui->wd_ip2->setDisabled(false);
    ui->le_name2->setDisabled(false);
    ui->cb_intvl2->setDisabled(false);
}


void IC6Comm::on_tb_view1_clicked() {
}
void IC6Comm::on_tb_view2_clicked() {
}

void IC6Comm::setStatusbar() {
    QLabel* labCellIndex = new QLabel("IC/6 data logger. v1.0.0", this);
    statusBar()->addPermanentWidget(labCellIndex);
}

void IC6Comm::initListTble(const QStringList& sl_ips) {
    qDebug() << sl_ips;
}

void IC6Comm::startAcq(const QString& ip, const QString& name, uint intvl) {
    if(QHostAddress(ip).isNull()) {
        QMessageBox::warning(this, "错误", "无效的IP地址");
        return;
    }
    bool connected = connectTest(ip);
    if(!connected) {
        return;
    }
    // 创建通信对象和线程
    QThread* thread = new QThread;
    CommWorker* device = new CommWorker(ip, name);
    // 移动对象到线程
    device->moveToThread(thread);
    // 连接信号槽
    connect(thread, &QThread::started, device, [this, intvl]() { // 使用 this 作为上下文
        if (!devices.isEmpty()) {
            devices.last()->startWork(intvl); // 通过成员变量访问
        }
    });
    connect(thread, &QThread::finished, device, &CommWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(device, &CommWorker::dataReceived, this, &IC6Comm::handleDataReceived);
    connect(device, &CommWorker::errorOccurred, this, &IC6Comm::handleError);
    // 存储对象和线程
    devices.insert(name, device);
    threads.append(thread);
    // 启动线程
    thread->start();
}

bool IC6Comm::connectTest(const QString& ip) {
    QTcpSocket socket;
    socket.setProxy(QNetworkProxy::NoProxy);
    socket.connectToHost(ip, 2101);
    socket.waitForConnected(200);
    connect(&socket, &QTcpSocket::readyRead, [&socket, this]() {
        QByteArray resp = socket.readAll();
        int resp_len = resp.length();
        statusBar()->showMessage(QString(resp.mid(5, resp_len - 5)), 2000);
    });
    bool connected = socket.state() == QTcpSocket::ConnectedState;
    if(connected) {
        connected = true;
        socket.write(Helper::BA_HELLO);
        socket.flush();
    }
    socket.waitForReadyRead(200);
    socket.disconnectFromHost();
    return connected;
}


void IC6Comm::handleDataReceived(const QByteArray& data, const QString& name) {
    uint cks   = data.back() & 0xff;
    QByteArray msg_len_ba = data.mid(0, 2);
    int msg_len = Helper::calcMsgLen(msg_len_ba);
    QByteArray msg_body_ba = data.mid(2, msg_len);
    auto cks_ = Helper::calcCks(msg_body_ba);
    if(cks == cks_) {
        Helper::calcData(msg_body_ba.mid(2, -1));
        // qDebug() << name << msg_body_ba.mid(2, -1).toHex();
    }
}

void IC6Comm::handleError(const QString& error, const QString& ip) {
    qDebug() << QString("[%1] error: %2").arg(ip, error);
}

