#include "ic6_comm.h"
#include "ui_ic6_comm.h"
#include "utility/write_data.h"

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
    write_pool = new QThreadPool(this);
    write_pool->setMaxThreadCount(QThread::idealThreadCount());
}

IC6Comm::~IC6Comm() {
    qDebug() << threads.count();
    foreach (auto thread, threads) {
        thread->quit();
        thread->wait();
        delete thread;
    }
    qDeleteAll(inst_list_);
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
    if(ipDupCheck(ip)) {
        return;
    }
    QString name = ui->le_name1->text();
    uint intvl = ui->cb_intvl1->currentText().toUInt();
    ui->wd_ip1->setDisabled(true);
    ui->le_name1->setDisabled(true);
    ui->cb_intvl1->setDisabled(true);
    startAcq(ip, name, intvl);
    // inst_list_.insert(name, )
    ip_list_.append(ip);
}

void IC6Comm::on_tb_start2_clicked() {
    QString ip = ui->wd_ip2->getIP();
    if(ipDupCheck(ip)) {
        return;
    }
    QString name = ui->le_name2->text();
    uint intvl = ui->cb_intvl2->currentText().toUInt();
    ui->wd_ip2->setDisabled(true);
    ui->le_name2->setDisabled(true);
    ui->cb_intvl2->setDisabled(true);
    startAcq(ip, name, intvl);
    ip_list_.append(ip);
}


void IC6Comm::on_tb_stop1_clicked() {
    QString ip = ui->wd_ip1->getIP();
    QString name = ui->le_name1->text();
    ip_list_.removeOne(ip);
    stopThread(ip, name);
    ui->wd_ip1->setDisabled(false);
    ui->le_name1->setDisabled(false);
    ui->cb_intvl1->setDisabled(false);
}

void IC6Comm::on_tb_stop2_clicked() {
    QString ip = ui->wd_ip2->getIP();
    QString name = ui->le_name2->text();
    ip_list_.removeOne(ip);
    stopThread(ip, name);
    ui->wd_ip2->setDisabled(false);
    ui->le_name2->setDisabled(false);
    ui->cb_intvl2->setDisabled(false);
}


void IC6Comm::on_tb_view1_clicked() {
}
void IC6Comm::on_tb_view2_clicked() {
}

void IC6Comm::dataHandle(const QString& name, const QList<bool>& status, const QList<float> frequencies, const QList<int>& acts) {
    if(!inst_list_.contains(name)) {
        return;
    }
    InstConfig* inst = inst_list_.find(name).value();
    inst->data_count_++;
    QStringList inst_data_store;
    QStringList inst_data_shown;
    int data_cnt = status.count();
    for (int var = 0; var < data_cnt; ++var) {
        bool state = status.at(var);
        QString s_freq = state ? QString::number(frequencies.at(var), 'f', 3) : "0";
        QString s_act = state ? QString::number(acts.at(var)) : "0";
        QString s_state = state ? "OK" : "NG";
        inst_data_shown.append(s_state + ":" + s_freq + ":" + s_act);
        inst_data_store.append(s_freq);
        inst_data_store.append(s_act);
    }
    inst->comm_data_ += ",," + inst_data_store.join(",") + "\n";
    if(inst->data_count_ > 50) {
        write_data(inst);
    }
}

void IC6Comm::write_data(InstConfig* inst) {
    // This is available in all editors.
    qDebug() << __FUNCTION__ << inst->comm_data_;
    WriteData* write_worker = new WriteData(inst->comm_data_, inst->file_name_);
    connect(write_worker, &WriteData::emit_write_data_res, this, &IC6Comm::app_info_show);
    write_pool->start(write_worker);
    inst->comm_data_.clear();
    inst->data_count_ = 0;
}

///
/// \brief VgcComm::app_info_show. 状态栏显示信息槽函数.
/// \param msg
///
void IC6Comm::app_info_show(const QString& msg) {
    statusBar()->showMessage(msg, 3000);
}


bool IC6Comm::ipDupCheck(const QString& ip) {
    if(ip_list_.contains(ip)) {
        QMessageBox::warning(this, u8"错误", u8"重复的IP地址");
        return true;
    }
    return false;
}

void IC6Comm::stopThread(const QString& ip, const QString& name) {
    auto thread = threads.find(ip).value();
    thread->exit();
    threads.remove(ip);
    // auto device = devices.find(name).value();
    // device.
    devices.remove(name);
    if(inst_list_.contains(name)) {
        write_data(inst_list_.find(name).value());
    }
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
        QMessageBox::warning(this, u8"错误", u8"无效的IP地址");
        return;
    }
    QString version;
    bool connected = connectTest(ip, &version);
    if(!connected) {
        return;
    }
    // create instrument instacne
    InstConfig* inst = new InstConfig();
    inst->inst_name_ = name;
    inst->ip_addr_ = ip;
    inst->inst_ver_ = version;
    inst->intvl_ = intvl;
    inst->data_count_ = 0;
    inst->setFileName();
    inst_list_.insert(name, inst);
    // craete communication object and thread
    QThread* thread = new QThread;
    CommWorker* device = new CommWorker(ip, name);
    // move object to thread
    device->moveToThread(thread);
    // connect signals and slots
    connect(thread, &QThread::started, device, [this, intvl]() { // use this as the content
        if (!devices.isEmpty()) {
            devices.last()->startWork(intvl);
        }
    });
    connect(thread, &QThread::finished, device, &CommWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(device, &CommWorker::dataReceived, this, &IC6Comm::handleDataReceived);
    connect(device, &CommWorker::errorOccurred, this, &IC6Comm::handleError);
    // store object and thread
    devices.insert(name, device);
    threads.insert(ip, thread);
    // start thread
    thread->start();
}

bool IC6Comm::connectTest(const QString& ip, QString* version) {
    QTcpSocket socket;
    socket.setProxy(QNetworkProxy::NoProxy);
    socket.connectToHost(ip, 2101);
    socket.waitForConnected(200);
    QString version_;
    connect(&socket, &QTcpSocket::readyRead, [&socket, &version_]() {
        QByteArray resp = socket.readAll();
        int resp_len = resp.length();
        version_ = QString(resp.mid(5, resp_len - 5));
    });
    bool connected = socket.state() == QTcpSocket::ConnectedState;
    if(connected) {
        connected = true;
        socket.write(Helper::BA_HELLO);
        socket.flush();
    }
    socket.waitForReadyRead(200);
    socket.disconnectFromHost();
    if(connected) {
        *version = version_;
        statusBar()->showMessage(version_, 3000);
    }
    return connected;
}



void IC6Comm::handleDataReceived(const QByteArray& data, const QString& name) {
    uint cks   = data.back() & 0xff;
    QByteArray msg_len_ba = data.mid(0, 2);
    int msg_len = Helper::calcMsgLen(msg_len_ba);
    QByteArray msg_body_ba = data.mid(2, msg_len);
    auto cks_ = Helper::calcCks(msg_body_ba);
    QList<int> l_act;
    QList<float> l_freq;
    QList<bool> l_stat;
    if(cks == cks_) {
        Helper::calcData(msg_body_ba.mid(2, -1), &l_act, &l_freq, &l_stat);
    }
    dataHandle(name, l_stat, l_freq, l_act);
}

void IC6Comm::handleError(const QString& error, const QString& ip) {
    // This is available in all editors.
    qDebug() << __FUNCTION__ << QString("[%1] error: %2").arg(ip, error);
}

