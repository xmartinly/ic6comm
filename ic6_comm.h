#ifndef IC6_COMM_H
#define IC6_COMM_H

#include <QMainWindow>

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

    enum BtnOp {
        StartBtn,
        StopBtn,
        RestartBtn,
        RemoveBtn
    };

  private slots:
    void on_act_comm_setting_triggered();

    void on_act_exit_triggered();

    void on_act_man_triggered();

    void on_act_about_triggered();

    void on_act_lang_triggered();


    void handleOperationClick(int row, BtnOp operation);

    void tblMenu(const QPoint pos);

  private:
    Ui::IC6Comm* ui;
    // 加载 SVG 图标
    QIcon startIcon = QIcon(":/Picture/svg/wm_play.svg"); // 开始图标
    QIcon stopIcon = QIcon(":/Picture/svg/wm_stop.svg"); // 停止
    QIcon restartIcon = QIcon(":/Picture/svg/wm_restart.svg"); // 重启
    QIcon deleteIcon = QIcon(":/Picture/svg/wm_trash_can.svg"); // 删除



  private:
    void setStatusbar();
    void initListTble(const QStringList& sl_ips = {});

};
#endif // IC6_COMM_H
