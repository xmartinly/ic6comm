#ifndef IC6_COMM_H
#define IC6_COMM_H

#include <QMainWindow>

#include "comm_worker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class IC6Comm;
}
QT_END_NAMESPACE

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

    void on_tb_view1_clicked();

    void on_tb_start2_clicked();

    void on_tb_stop2_clicked();

    void on_tb_view2_clicked();

  private:
    Ui::IC6Comm* ui;



    QMap<QString, CommWorker*> devices; // 存储设备对象
    QVector<QThread*> threads; // 存储对应线程

    bool inst1_conn_ = false;
    bool inst2_conn_ = false;


  private:
    void setStatusbar();
    void initListTble(const QStringList& sl_ips = {});
    void startAcq(const QString& ip, const QString& name, uint intvl = 200);

    bool connectTest(const QString& ip);

  private slots:
    void handleDataReceived(const QByteArray& data, const QString& name);
    void handleError(const QString& error, const QString& ip);

  signals:
    void stopWorker();


};
#endif // IC6_COMM_H
