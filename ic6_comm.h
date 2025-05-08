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
    QString inst_ch_;
    QString inst_ver_;
    QString inst_name_;
    QString file_name_;
    QString comm_data_;
    uint intvl_;
    uint write_threshold_;
    uint data_count_ = 0;
    uint data_file_count_ = 0;
    quint64 data_run_count_ = 0;
    QString first_file_name_ = "";
//    QString file_header_ = "Name:,%1\nIP:,%2\nVersion:,%3\nInterval:,%4\nStartTime:,%5\nData:\nTime,Frequency1,Act1,Frequency2,Act2,Frequency3,Act3,Frequency4,Act4,Frequency5,Act5,Frequency6,Act6,Frequency7,Act7,Frequency8,Act8\n";
    QString file_header_ = QString("IC6 Data Log %1\n").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    void setFileName() {
//        QDateTime start_tm = QDateTime::currentDateTime();
//        QString tm1 = start_tm.toString("yyyy-MM-dd hh:mm:ss");
//        QString tm2 = start_tm.toString("yyyyMMdd_hhmmss");
//        if(!first_file_name_.isEmpty() && data_file_count_ > 0) {
//            file_name_ = first_file_name_ + ("_" + QString::number(data_file_count_) + ".txt");
//        } else {
//            QString _file = QString("%1/data/%2_%3");
//            first_file_name_ = _file.arg(
//                                   QDir::currentPath(),
//                                   tm2,
//                                   inst_name_);
//            file_name_ = first_file_name_ + ".txt";
//        }
//        QString header = file_header_.arg(inst_name_, ip_addr_, inst_ver_, QString::number(intvl_), tm1);
        QString _file = QString("%1/data/%2.txt");
        file_name_ = _file.arg(QDir::currentPath(), inst_name_);
        qDebug() << file_name_;
        QFile data_file = QFile(file_name_);
        if(data_file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            data_file.write(file_header_.toStdString().c_str());
            data_file.close();
        }
        write_threshold_ = 10000 / intvl_ - 1;
    }
};


class IC6Comm : public QMainWindow {
    Q_OBJECT

  public:
    IC6Comm(QWidget* parent = nullptr);
    ~IC6Comm();

  private slots:
    void on_act_open_folder_triggered();
    void on_act_exit_triggered();
    void on_act_man_triggered();
    void on_act_about_triggered();
    void on_tb_start1_clicked();
    void on_tb_stop1_clicked();
    void on_tb_start2_clicked();
    void on_tb_stop2_clicked();


  private:
    Ui::IC6Comm* ui;

    QThreadPool* write_pool_;
    QMap<QString, CommWorker*> devices_; // QMap to store workers
    QMap<QString, QThread*> threads_; // QMap to store threads
    QMap<QString, InstConfig*> inst_list_;
    QStringList ip_list_;
    QString ch_data_str_ = "Freq: %1, Act: %2";
    const QByteArray BA_HELLO_ = QByteArray::fromHex("0200480149");
    const QStringList ABOUT_INFO_ = {
        "2025/03/05 v1.0.0 \tBeta release.",
        "2025/03/07 v1.0.1 \tChange write file function to 10s.",
        "2025/03/07 v1.0.2 \tChange data calculation to thread.",
        "2025/03/07 v1.0.3 \tChange crystal icon to dynamic widget.",
        "2025/03/08 v1.0.4 \tRedesign UI.",
        "2025/03/09 v1.1.0 \tRelease the first version. Connect function bug fixed.",
        "2025/05/04 v1.1.1 \tCustom data store format.",
    };


  private:
    bool connectTest(const QString& ip, QString* version);
    bool instIpCheck(const QString& ip);
    bool nameCheck(const QString& inst_name);
    bool startAcq(const QString& ip, const QString& inst_name, uint intvl = 200, const QString& ch = "1");
    void setStatusbar();
    void stopThread(const QString& inst_name);
    void writeData(InstConfig* inst);
    void setChLabel(const QString& name, const QList<int>& act, const QStringList& data);
    void readConfig();
    void writeConfig();
    void initCirWidget();
    uint calcChannels(uint chs);


  private slots:
    void handleError(const QString& error, const QString& ip);
    void appInfoShow(const QString& msg);
    void writeDataSize(const QString& name, float size);
    void getConnErr(const QString& name);
    void getData(const QList<bool>& status, const QList<double>& frequencies, const QList<int>& activities, const QString& name, const QString& data_tm);



};
#endif // IC6_COMM_H
