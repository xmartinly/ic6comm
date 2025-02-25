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
    // This is available in all editors.
    // qDebug() << __FUNCTION__ << Helper::calcCks(data_);
}

IC6Comm::~IC6Comm() {
    // ui->tbl_list.widget
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

void IC6Comm::handleOperationClick(int row, BtnOp operation) {
    QTableWidgetItem* ipItem = ui->tbl_list->item(row, 0);
    QString click_str;
    switch (operation) {
        case StopBtn:
            click_str = "Stop Click: ";
            break;
        case RestartBtn:
            click_str = "Restart Click: ";
            break;
        case RemoveBtn:
            click_str = "Remove Click: ";
            break;
        default:
            click_str = "Start Click: ";
            break;
    }
    if (ipItem) {
        qDebug() << __FUNCTION__ << operation << click_str << ipItem->text();
    }
}

void IC6Comm::tblMenu(const QPoint pos) {
    QMenu* menu = new QMenu(ui->tbl_list);
    QAction* startAction = menu->addAction(startIcon, "Start");
    QAction* stopAction = menu->addAction(stopIcon, "Stop");
    QAction* restartAction = menu->addAction(restartIcon, "Restart");
    QAction* deleteAction = menu->addAction(deleteIcon, "Remove");
    menu->addAction(startAction);
    menu->addAction(stopAction);
    menu->addAction(restartAction);
    menu->addAction(deleteAction);
    int x = pos.x ();
    int y = pos.y ();
    QModelIndex index = ui->tbl_list->indexAt (QPoint(x, y));
    int row  = index.row ();
    connect(startAction, &QAction::triggered, this, [this, row]() {
        handleOperationClick(row, StartBtn);
    });
    connect(stopAction, &QAction::triggered, this, [this, row]() {
        handleOperationClick(row, StopBtn);
    });
    connect(restartAction, &QAction::triggered, this, [this, row]() {
        handleOperationClick(row, RestartBtn);
    });
    connect(deleteAction, &QAction::triggered, this, [this, row]() {
        handleOperationClick(row, RemoveBtn);
        ui->tbl_list->removeRow(row);
    });
    menu->move(cursor().pos ());
    menu->show();
}



void IC6Comm::setStatusbar() {
    QLabel* labCellIndex = new QLabel("IC/6 data logger. v1.0.0", this);
    statusBar()->addPermanentWidget(labCellIndex);
}

void IC6Comm::initListTble(const QStringList& sl_ips) {
    QTableWidget* tableWidget = ui->tbl_list;
    tableWidget->setRowCount(5);
    tableWidget->setColumnCount(3);
    QStringList headers;
    headers << "IP" << "Run Time" << "Status";
    tableWidget->setHorizontalHeaderLabels(headers);
    // 设置表格内容
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        // IP 列
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString("192.168.1.%1").arg(row + 1)));
        // 运行时间列
        tableWidget->setItem(row, 1, new QTableWidgetItem("00:00:00"));
        tableWidget->setItem(row, 2, new QTableWidgetItem("00:00:00"));
    }
    // 设置表格属性
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列自适应宽度
    tableWidget->verticalHeader()->setVisible(false); // 隐藏行号
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行
    tableWidget->setContextMenuPolicy (Qt::CustomContextMenu);
    connect(tableWidget, &QTableWidget::customContextMenuRequested, this, &IC6Comm::tblMenu);
}

