#include "ic6_comm.h"
#include "ui_ic6_comm.h"
#include "utility/write_data.h"
#include "utility/circle_widget.h"

IC6Comm::IC6Comm(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::IC6Comm) {
    ui->setupUi(this);
    qRegisterMetaType<QList<bool>>("QList<bool>");
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<QList<double>>("QList<double>");
    setStatusbar();
    write_pool_ = new QThreadPool(this);
    write_pool_->setMaxThreadCount(QThread::idealThreadCount() / 2);
    readConfig();
    QDir data_dir("data");
    if (!data_dir.exists()) {
        data_dir.mkpath(".");
    }
    initCirWidget();
    // This is available in all editors.
    // qDebug() << __FUNCTION__ << calcChannels(0x14);
}

IC6Comm::~IC6Comm() {
    writeConfig();
    foreach (auto inst, inst_list_) {
        QString name = inst->inst_name_;
        stopThread(name);
    }
    delete ui;
}


///
/// \brief IC6Comm::on_act_exit_triggered
///
void IC6Comm::on_act_exit_triggered() {
    this->close();
}

///
/// \brief IC6Comm::on_act_man_triggered
///
void IC6Comm::on_act_man_triggered() {
    QString ic6_url = "https://www.inficon.com/en/products/thin-film-technology/ic6";
    QDesktopServices::openUrl(QUrl::fromLocalFile(ic6_url));
}

///
/// \brief IC6Comm::on_act_about_triggered
///
void IC6Comm::on_act_about_triggered() {
    QMessageBox::information(this, "About", ABOUT_INFO_.join("\n"));
}


///
/// \brief IC6Comm::on_tb_start1_clicked
///
void IC6Comm::on_tb_start1_clicked() {
    QString ip = ui->wd_ip1->getIP();
    if(!instIpCheck(ip)) {
        return;
    }
    QString inst_name = ui->le_name1->text();
    if(!nameCheck(inst_name)) {
        return;
    }
    uint intvl = ui->cb_intvl1->currentText().toUInt();
    if(startAcq(ip, inst_name, intvl, "1")) {
        ui->wd_ip1->setDisabled(true);
        ui->le_name1->setDisabled(true);
        ui->cb_intvl1->setDisabled(true);
        ip_list_.append(ip);
        ui->frame1->setObjectName(inst_name);
        ui->frame1->setProperty("channel", "1");
    }
}

///
/// \brief IC6Comm::on_tb_start2_clicked
///
void IC6Comm::on_tb_start2_clicked() {
    QString ip = ui->wd_ip2->getIP();
    if(!instIpCheck(ip)) {
        return;
    }
    QString name = ui->le_name2->text();
    if(!nameCheck(name)) {
        return;
    }
    uint intvl = ui->cb_intvl2->currentText().toUInt();
    if(startAcq(ip, name, intvl, "2")) {
        ui->wd_ip2->setDisabled(true);
        ui->le_name2->setDisabled(true);
        ui->cb_intvl2->setDisabled(true);
        ip_list_.append(ip);
        ui->frame2->setObjectName(name);
        ui->frame2->setProperty("channel", "2");
    }
}

///
/// \brief IC6Comm::on_tb_stop1_clicked
///
void IC6Comm::on_tb_stop1_clicked() {
    QString ip = ui->wd_ip1->getIP();
    QString name = ui->le_name1->text();
    stopThread(name);
    ui->wd_ip1->setDisabled(false);
    ui->le_name1->setDisabled(false);
    ui->cb_intvl1->setDisabled(false);
}

///
/// \brief IC6Comm::on_tb_stop2_clicked
///
void IC6Comm::on_tb_stop2_clicked() {
    QString ip = ui->wd_ip2->getIP();
    QString name = ui->le_name2->text();
//    this->startAcq();
    stopThread(name);
    ui->wd_ip2->setDisabled(false);
    ui->le_name2->setDisabled(false);
    ui->cb_intvl2->setDisabled(false);
}

///
/// \brief IC6Comm::on_act_open_folder_triggered
///
void IC6Comm::on_act_open_folder_triggered() {
    QString data_dir = QDir::currentPath() + "/data/";
    QDir dir = QDir(data_dir);
    if (dir.exists()) {
        QDesktopServices::openUrl(QUrl(data_dir));
    }
}

///
/// \brief IC6Comm::writeData
/// \param inst
///
void IC6Comm::writeData(InstConfig* inst) {
    WriteData* write_worker = new WriteData(inst->comm_data_, inst->file_name_, inst->inst_name_);
    connect(write_worker, &WriteData::emit_write_data_res, this, &IC6Comm::appInfoShow);
    connect(write_worker, &WriteData::emit_file_size, this, &IC6Comm::writeDataSize);
    write_pool_->start(write_worker);
    inst->comm_data_.clear();
    inst->data_count_ = 0;
}

///
/// \brief IC6Comm::setChLabel
/// \param name
/// \param status
/// \param data
///
void IC6Comm::setChLabel(const QString& name, const QList<int>& acts, const QStringList& data) {
    auto frame = this->findChild<QFrame*>(name);
    QString ch_s = frame->property("channel").toString();
    int ch_count = acts.count();
    for (int var = 0; var < ch_count; ++var) {
        QString idx_s = QString::number(var + 1);
        QString data_lb_name = QString("ch%1_%2").arg(ch_s, idx_s);
        QString cir_wd_name = QString("xtal%1_%2").arg(ch_s, idx_s);
        auto data_label = frame->findChild<QLabel*>(data_lb_name);
        auto cir_wd = frame->findChild<CircleWidget*>(cir_wd_name);
        if(data_label) {
            data_label->setText(data.at(var));
        }
        if(cir_wd) {
            cir_wd->setValue(acts.at(var));
        }
    }
}

///
/// \brief IC6Comm::readConfig
///
void IC6Comm::readConfig() {
    QString path = QCoreApplication::applicationDirPath();
    QSettings m_iniFile = QSettings(path + "/settings.ini", QSettings::IniFormat);
    m_iniFile.beginGroup("CH1");
    QString ch1_ip = m_iniFile.value("IP").toString();
    QString ch1_intvl = m_iniFile.value("INTERVAL").toString();
    QString ch1_name = m_iniFile.value("NAME").toString();
    m_iniFile.endGroup();
    ui->wd_ip1->setIP(ch1_ip);
    ui->cb_intvl1->setCurrentText(ch1_intvl);
    ui->le_name1->setText(ch1_name);
    m_iniFile.beginGroup("CH2");
    QString ch2_ip = m_iniFile.value("IP").toString();
    QString ch2_intvl = m_iniFile.value("INTERVAL").toString();
    QString ch2_name = m_iniFile.value("NAME").toString();
    m_iniFile.endGroup();
    ui->wd_ip2->setIP(ch2_ip);
    ui->cb_intvl2->setCurrentText(ch2_intvl);
    ui->le_name2->setText(ch2_name);
}

///
/// \brief IC6Comm::writeConfig
///
void IC6Comm::writeConfig() {
    QString path = QCoreApplication::applicationDirPath();
    QSettings m_iniFile = QSettings(path + "/settings.ini", QSettings::IniFormat);
    m_iniFile.clear();
    QString ch1_ip = ui->wd_ip1->getIP();
    QString ch2_ip = ui->wd_ip2->getIP();
    QString ch1_intvl = ui->cb_intvl1->currentText();
    QString ch2_intvl = ui->cb_intvl2->currentText();
    QString ch1_name = ui->le_name1->text();
    QString ch2_name = ui->le_name2->text();
    m_iniFile.beginGroup("CH1");
    m_iniFile.setValue("IP", ch1_ip);
    m_iniFile.setValue("NAME", ch1_name);
    m_iniFile.setValue("INTERVAL", ch1_intvl);
    m_iniFile.endGroup();
    m_iniFile.beginGroup("CH2");
    m_iniFile.setValue("IP", ch2_ip);
    m_iniFile.setValue("NAME", ch2_name);
    m_iniFile.setValue("INTERVAL", ch2_intvl);
    m_iniFile.endGroup();
}

///
/// \brief IC6Comm::initCirWidget
///
void IC6Comm::initCirWidget() {
    QString frm1_wd_prefix = "xtal1_%1";
    QString frm2_wd_prefix = "xtal2_%1";
    QFrame* frame1 = ui->frame1;
    QFrame* frame2 = ui->frame2;
    for (int var = 1; var < 9; ++var) {
        QString idx = QString::number(var);
        QString wd1_name = frm1_wd_prefix.arg(idx);
        QString wd2_name = frm2_wd_prefix.arg(idx);
        auto wd1 = frame1->findChild<CircleWidget*>(wd1_name);
        auto wd2 = frame2->findChild<CircleWidget*>(wd2_name);
        wd1->setIndex(var);
        wd2->setIndex(var);
    }
}

///
/// \brief IC6Comm::calcChannels
/// \param chs
/// \return
///
uint IC6Comm::calcChannels(uint chs) {
    uint ch_count = chs & 0x0f;
    uint ch_count_ = 0;
    while (ch_count) {
        ch_count_++;
        ch_count = ch_count >> 1;
    }
    return ch_count_ * 2;
}
\

///
/// \brief VgcComm::appInfoShow. 状态栏显示信息槽函数.
/// \param msg
///
void IC6Comm::appInfoShow(const QString& msg) {
    statusBar()->showMessage(msg, 3000);
}

void IC6Comm::writeDataSize(const QString& name, float size) {
    auto inst = inst_list_.find(name).value();
    if(size > 50) {
        inst->data_file_count_++;
        inst->setFileName();
    }
}

///
/// \brief IC6Comm::getConnErr
/// \param name
///
void IC6Comm::getConnErr(const QString& name) {
    // This is available in all editors.
    qDebug() << __FUNCTION__ << name;
    InstConfig* inst = inst_list_.find(name).value();
    if(inst) {
        QMessageBox::warning(this, "Error", "Instrument connect error after 10 tries.");
        auto wd_ip = this->findChild<IPAddress*>(QString("wd_ip%1").arg(inst->inst_ch_));
        wd_ip->setEnabled(true);
        auto cb_intvl = this->findChild<QComboBox*>(QString("cb_intvl%1").arg(inst->inst_ch_));
        cb_intvl->setEnabled(true);
        auto name_le = this->findChild<QLineEdit*>(QString("le_name%1").arg(inst->inst_ch_));
        name_le->setEnabled(true);
        auto thread = threads_.find(name).value();
        auto device = devices_.find(name).value();
        if(thread && device) {
            stopThread(name);
        }
    }
}

///
/// \brief IC6Comm::getData
/// \param status
/// \param frequencies
/// \param activities
/// \param name
///
void IC6Comm::getData(const QList<bool>& status, const QList<double>& frequencies, const QList<int>& activities, const QString& name, const QString& data_tm) {
    Q_UNUSED(data_tm);
    if(!inst_list_.contains(name)) {
        return;
    }
    InstConfig* inst = inst_list_.find(name).value();
    if(!inst) {
        return;
    }
    inst->data_count_++;
    inst->data_run_count_++;
    QStringList inst_data_store;
    QStringList data_freq;
    QStringList data_act;
    QStringList shown_data;
    int data_cnt = status.count();
    // QString tm = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    for (int var = 0; var < data_cnt; ++var) {
        bool state = status.at(var);
        QString s_freq = state ? QString::number(frequencies.at(var), 'f', 3) : "000.000";
        QString s_act = state ? QString::number(activities.at(var)) : "0";
        shown_data.append(ch_data_str_.arg(s_freq, s_act));
        data_freq.append(s_freq);
        data_act.append(s_act);
    }
//    inst->comm_data_.append(data_tm + "," + inst_data_store.join(",") + "\n");
    QDateTime now = QDateTime::currentDateTime();
    inst_data_store.append(data_freq.join(","));
    inst_data_store.append(data_act.join(","));
    inst_data_store.append(now.toString("MM/dd/yyyy"));
    inst_data_store.append(now.toString("hh:mm:ss"));
    inst_data_store.prepend(QString::number(inst->data_run_count_));
    inst->comm_data_.append(inst_data_store.join(",") + "\n");
    if(inst->data_count_ > inst->write_threshold_) { // write data every 10 seconds
        writeData(inst);
    }
    setChLabel(name, activities, shown_data);
}

///
/// \brief IC6Comm::instIpCheck
/// \param ip
/// \return
///
bool IC6Comm::instIpCheck(const QString& ip) {
    if(QHostAddress(ip).isNull()) {
        QMessageBox::warning(this, "Error", "Invalid IP address");
        return false;
    }
    if(ip_list_.contains(ip)) {
        QMessageBox::warning(this, "Error", "Duplicate IP address");
        return false;
    }
    return true;
}

///
/// \brief IC6Comm::nameCheck
/// \param name
/// \return
///
bool IC6Comm::nameCheck(const QString& inst_name) {
    if(inst_name.length() < 2) {
        QMessageBox::warning(this, "Error", "Name field must be filled in.");
        return false;
    }
    if(threads_.contains(inst_name)) {
        QMessageBox::warning(this, "Error", "Duplicate name.");
        return false;
    }
    return true;
}

///
/// \brief IC6Comm::stopThread
/// \param ip
/// \param name
///
void IC6Comm::stopThread( const QString& inst_name) {
    QString ip;
    if(inst_list_.contains(inst_name)) {
        auto inst = inst_list_.find(inst_name).value();
        ip = inst->ip_addr_;
        writeData(inst);
        inst_list_.remove(inst_name);
        delete inst;
    }
    if(threads_.contains(inst_name)) {
        auto thread = threads_.find(inst_name).value();
        thread->exit();
        thread->wait(100);
        threads_.remove(inst_name);
        delete thread;
    }
    if(devices_.contains(inst_name)) {
        devices_.remove(inst_name);
    }
    if(ip_list_.contains(ip)) {
        ip_list_.removeOne(ip);
    }
}

///
/// \brief IC6Comm::setStatusbar
///
void IC6Comm::setStatusbar() {
    QLabel* labCellIndex = new QLabel("IC/6 data logger. v1.1.1", this);
    statusBar()->addPermanentWidget(labCellIndex);
}

///
/// \brief IC6Comm::startAcq
/// \param ip
/// \param name
/// \param intvl
///
bool IC6Comm::startAcq(const QString& ip, const QString& inst_name, uint intvl, const QString& ch) {
    QString version;
    bool connected = connectTest(ip, &version);
    if(!connected) {
        QMessageBox::warning(this, "Error", "Can't connect to " + ip);
        return false;
    }
    // create instrument instacne
    InstConfig* inst = new InstConfig();
    auto lbl = this->findChild<QLabel*>(QString("ch%1_%2").arg(ch, "info"));
    inst->inst_name_ = inst_name;
    inst->ip_addr_ = ip;
    inst->inst_ver_ = version;
    inst->intvl_ = intvl;
    inst->inst_ch_ = ch;
    inst->data_count_ = 0;
    inst->setFileName();
    inst_list_.insert(inst_name, inst);
    if(lbl) {
        lbl->setText(QString("Inst%1#: %2, %3, %4").arg(ch, inst_name, version, ip));
        lbl->setToolTip(QString("Data File: %1").arg(inst->file_name_));
    }
    // craete communication object and thread
    QThread* thread = new QThread;
    CommWorker* device = new CommWorker(ip, inst_name);
    // move object to thread
    device->moveToThread(thread);
    // store object and thread
    devices_.insert(inst_name, device);
    threads_.insert(inst_name, thread);
    // connect signals and slots
    connect(thread, &QThread::finished, device, &CommWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(device, &CommWorker::connectError, this, &IC6Comm::getConnErr);
    connect(device, &CommWorker::sendData, this, &IC6Comm::getData);
    connect(device, &CommWorker::errorOccurred, this, &IC6Comm::handleError);
    connect(thread, &QThread::started, device, [device, intvl]() { // use this as the content
        // if (!devices.isEmpty()) {
        //     devices.last()->startWork(intvl);
        // }
        device->startWork(intvl);
    });
    // start thread
    thread->start();
    return true;
}

///
/// \brief IC6Comm::connectTest
/// \param ip
/// \param version
/// \return
///
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
        socket.write(BA_HELLO_);
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


///
/// \brief IC6Comm::handleError
/// \param error
/// \param ip
///
void IC6Comm::handleError(const QString& error, const QString& ip) {
    // This is available in all editors.
    qDebug() << __FUNCTION__ << QString("[%1] error: %2").arg(ip, error);
}





