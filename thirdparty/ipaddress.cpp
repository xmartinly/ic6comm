

#pragma execution_character_set("utf-8")


#include "ipaddress.h"
#include "qlabel.h"
#include "qlineedit.h"
#include <QVBoxLayout>
#include "qvalidator.h"
#include "qevent.h"
#include "qapplication.h"

#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    #include "qregexp.h"
#endif


IPAddress::IPAddress(QWidget* parent) : QWidget(parent) {
    labDot1 = new QLabel;
    labDot1->setAlignment(Qt::AlignBottom);
    labDot1->setText(".");
    labDot1->setFont(QFont("Courier New", 16, QFont::Bold));
    labDot2 = new QLabel;
    labDot2->setAlignment(Qt::AlignBottom);
    labDot2->setText(".");
    labDot2->setFont(QFont("Courier New", 16, QFont::Bold));
    labDot3 = new QLabel;
    labDot3->setAlignment(Qt::AlignBottom);
    labDot3->setText(".");
    labDot3->setFont(QFont("Courier New", 16, QFont::Bold));
    txtIP1 = new QLineEdit;
    txtIP1->setObjectName("txtIP1");
    txtIP1->setAlignment(Qt::AlignCenter);
    txtIP1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    txtIP1->setFont(QFont("Courier New", 14, QFont::Bold));
//    txtIP1->setPlaceholderText("192");
    connect(txtIP1, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    txtIP2 = new QLineEdit;
    txtIP2->setObjectName("txtIP2");
    txtIP2->setAlignment(Qt::AlignCenter);
    txtIP2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    txtIP2->setFont(QFont("Courier New", 14, QFont::Bold));
//    txtIP2->setPlaceholderText("168");
    connect(txtIP2, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    txtIP3 = new QLineEdit;
    txtIP3->setObjectName("txtIP3");
    txtIP3->setAlignment(Qt::AlignCenter);
    txtIP3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    txtIP3->setFont(QFont("Courier New", 14, QFont::Bold));
//    txtIP3->setPlaceholderText("1");
    connect(txtIP3, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    txtIP4 = new QLineEdit;
    txtIP4->setObjectName("txtIP4");
    txtIP4->setAlignment(Qt::AlignCenter);
    txtIP4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    txtIP4->setFont(QFont("Courier New", 14, QFont::Bold));
//    txtIP4->setPlaceholderText("100");
    connect(txtIP4, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    QRegExp regExp("(2[0-5]{2}|2[0-4][0-9]|1?[0-9]{1,2})");
    QRegExpValidator* validator = new QRegExpValidator(regExp, this);
    txtIP1->setValidator(validator);
    txtIP2->setValidator(validator);
    txtIP3->setValidator(validator);
    txtIP4->setValidator(validator);
#endif
    txtIP1->installEventFilter(this);
    txtIP2->installEventFilter(this);
    txtIP3->installEventFilter(this);
    txtIP4->installEventFilter(this);
    QFrame* frame = new QFrame;
    frame->setObjectName("frameIP");
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);
    verticalLayout->addWidget(frame);
    QHBoxLayout* layout = new QHBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(txtIP1);
    layout->addWidget(labDot1);
    layout->addWidget(txtIP2);
    layout->addWidget(labDot2);
    layout->addWidget(txtIP3);
    layout->addWidget(labDot3);
    layout->addWidget(txtIP4);
}

IPAddress::~IPAddress() {
}

bool IPAddress::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QLineEdit* txt = static_cast<QLineEdit*>(watched);
        if (txt == txtIP1 || txt == txtIP2 || txt == txtIP3 || txt == txtIP4) {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            if (key->text() == "." && txt->text().length() > 0) {
                this->focusNextChild();
            }
            if (key->key() == Qt::Key_Backspace) {
                if (txt->text().length() <= 1) {
                    this->focusNextPrevChild(false);
                    auto targetWidget = QApplication::focusWidget();
                    // 创建按下事件
                    QKeyEvent pressEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
                    QCoreApplication::sendEvent(targetWidget, &pressEvent);
                    // 创建释放事件
                    QKeyEvent releaseEvent(QEvent::KeyRelease, Qt::Key_Right, Qt::NoModifier);
                    QCoreApplication::sendEvent(targetWidget, &releaseEvent);
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void IPAddress::textChanged(const QString& text) {
    int len = text.length();
    int value = text.toInt();
    if (len == 3) {
        if (value >= 100 && value <= 255) {
            this->focusNextChild();
        }
    }
    ip = QString("%1.%2.%3.%4").arg(txtIP1->text(), txtIP2->text(), txtIP3->text(), txtIP4->text());
}

QString IPAddress::getIP() const {
    return this->ip;
}

QSize IPAddress::sizeHint() const {
    return QSize(250, 20);
}

QSize IPAddress::minimumSizeHint() const {
    return QSize(30, 10);
}

void IPAddress::setIP(const QString& ip) {
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if (!regExp.exactMatch(ip)) {
        return;
    }
#endif
    if (this->ip != ip) {
        this->ip = ip;
        QStringList list = ip.split(".");
        txtIP1->setText(list.at(0));
        txtIP2->setText(list.at(1));
        txtIP3->setText(list.at(2));
        txtIP4->setText(list.at(3));
    }
}

void IPAddress::clear() {
    txtIP1->clear();
    txtIP2->clear();
    txtIP3->clear();
    txtIP4->clear();
    txtIP1->setFocus();
}
