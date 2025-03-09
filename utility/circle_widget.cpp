#include "circle_widget.h"

#include <QDebug>
#include <QPainter>


CircleWidget::CircleWidget(QWidget* parent): QWidget(parent) {
}

void CircleWidget::setValue(int value) {
    w_value = value;
    update();
}

void CircleWidget::setIndex(int idx) {
    w_index_ = idx;
    update();
}

void CircleWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const int diameter = qMin(width(), height()) * 0.8;
    const int radius = diameter / 2;
    QPoint center = rect().center();
    // 计算颜色
    QColor circleColor;
    int red_val = 255 - red_factor_ * w_value;
    circleColor = QColor(red_val, GREEN_VAL_, BLUE_VAL_);
    if (w_value < 1) {
        circleColor = QColor(50, 0, 0);
    }
    // 绘制圆形
    painter.setPen(Qt::NoPen);
    painter.setBrush(circleColor);
    painter.drawEllipse(center, radius, radius);
    // 绘制居中数字
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setFamily("Microsoft YaHei UI");
    font.setBold(true);
    font.setPixelSize(radius * 1.4);
    painter.setFont(font);
    if (w_value < 1) {
        painter.setPen(RED_COLOR_);
    }
    QString text = QString::number(w_index_);
    QRect textRect = painter.fontMetrics().boundingRect(text);
    // This is customer debug information.
    qDebug() << __FUNCTION__ << center;
    textRect.moveCenter(center);
    painter.drawText(textRect, Qt::AlignCenter, text);
}
