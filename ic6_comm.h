#ifndef IC6_COMM_H
#define IC6_COMM_H

#include <QMainWindow>
#include <QMetaType>

#include "comm_worker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class IC6Comm;
}
QT_END_NAMESPACE

struct InstConfig {
    QString ip_addr_;
    QString inst_ver_;
    QString inst_name_;
    QString file_name_;
    QString comm_data_;
    uint intvl_;
    uint data_count_ = 0;
    uint data_file_count_ = 0;
    QString first_file_name_ = "";
    QString file_header_ = "Name:,%1\nIP:,%2\nVersion:,%3\nInterval:,%4\nStartTime:,%5\nData:\nTime,Frequency1,Act1,Frequency2,Act2,Frequency3,Act3,Frequency4,Act4,Frequency5,Act5,Frequency6,Act6,Frequency7,Act7,Frequency8,Act8\n";
    void setFileName() {
        QDateTime start_tm = QDateTime::currentDateTime();
        QString tm1 = start_tm.toString("yyyy-MM-dd hh:mm:ss");
        QString tm2 = start_tm.toString("yyyyMMdd_hhmmss");
        if(!first_file_name_.isEmpty() && data_file_count_ > 0) {
            file_name_ = first_file_name_ + ("_" + QString::number(data_file_count_) + ".csv");
        } else {
            QString _file = QString("%1/data/%2_%3");
            first_file_name_ = _file.arg(
                                   QDir::currentPath(),
                                   tm2,
                                   inst_name_);
            file_name_ = first_file_name_ + ".csv";
        }
        QString header = file_header_.arg(inst_name_, ip_addr_, inst_ver_, QString::number(intvl_), tm1);
        QFile data_file = QFile(file_name_);
        if(data_file.open(QIODevice::WriteOnly)) {
            data_file.write(header.toStdString().c_str());
            data_file.close();
        }
    }
};


class IC6Comm : public QMainWindow {
    Q_OBJECT

  public:
    IC6Comm(QWidget* parent = nullptr);
    ~IC6Comm();

  private slots:
    void on_act_comm_setting_triggered();
    void on_act_exit_triggered();
    void on_act_man_triggered();
    void on_act_about_triggered();
    void on_act_lang_triggered();
    void on_tb_start1_clicked();
    void on_tb_stop1_clicked();
    void on_tb_start2_clicked();
    void on_tb_stop2_clicked();

  private:
    Ui::IC6Comm* ui;

    QMap<QString, CommWorker*> devices; // QMap to store workers
    QMap<QString, QThread*> threads; // QMap to store threads
    QMap<QString, InstConfig*> inst_list_;

    bool inst1_conn_ = false;
    bool inst2_conn_ = false;

    QStringList ip_list_;

    //数据写入线程池, 此次当单线程使用
    QThreadPool* write_pool;
    // QByteArray ba_test = QByteArray::fromHex("1500534709535303005347095353040053470953530500E7");


  private:
    bool connectTest(const QString& ip, QString* version);
    bool ipDupCheck(const QString& ip);
    bool nameCheck(const QString& name);
    bool startAcq(const QString& ip, const QString& name, uint intvl = 200, const QString& ch = "1");
    void setStatusbar();
    void stopThread(const QString& ip, const QString& name);
    void writeData(InstConfig* inst);
    void setChLabel(const QString& name, const QList<int>& act, const QStringList& data);
    void readConfig();
    void writeConfig();
    void initCirWidget();

  private slots:
    void handleError(const QString& error, const QString& ip);
    void appInfoShow(const QString& msg);
    void writeDataSize(const QString& name, float size);
    void getData(const QList<bool>& status, const QList<double>& frequencies, const QList<int>& activities, const QString& name);

  signals:
    void stopWorker();


};
#endif // IC6_COMM_H
