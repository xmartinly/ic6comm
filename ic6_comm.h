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
    uint intvl_;
    uint write_threshold_;
    uint data_count_ = 0;
    uint data_file_count_ = 0;
    quint64 data_run_count_ = 0;

    QString ip_addr_;
    QString inst_ch_;
    QString inst_ver_;
    QString inst_name_;
    QString file_name_;
    QString comm_data_;

    QString file_header_ = QString("IC6 Data Log %1\n").arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    void setFileName() {
        QString _file = QString("%1/data/%2.txt");
        file_name_ = _file.arg(QDir::currentPath(), inst_name_);
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

    void handleError(const QString& error, const QString& ip);
    void appInfoShow(const QString& msg);
    void writeDataSize(const QString& name, float size);
    void getConnErr(const QString& name);
    void getData(const QList<bool>& status, const QList<double>& frequencies, const QList<int>& activities, const QString& name, const QString& data_tm);

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
        "2025/06/19 v1.1.2 \tFix connection issue after disconnect from instrument.",
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
};
#endif // IC6_COMM_H
