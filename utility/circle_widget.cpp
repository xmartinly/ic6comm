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
    bool act_error = w_value < 1 || w_value > 800;
    // 计算颜色
    QColor circleColor;
    int red_val = 255 - red_factor_ * w_value;
    circleColor = QColor(red_val, GREEN_VAL_, BLUE_VAL_);
    if (act_error) {
        circleColor = RED_COLOR_;
        // circleColor = QColor(243, 129, 129);
    }
    // 绘制圆形
    painter.setPen(Qt::NoPen);
    painter.setBrush(circleColor);
    painter.drawEllipse(center, radius, radius);
    // 绘制居中数字
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setFamily("Microsoft YaHei UI");
    // font.setFamily("Microsoft YaHei Mono");
    font.setBold(true);
    font.setPixelSize(radius * 1.4);
    painter.setFont(font);
    if (act_error) {
        // painter.setPen(QColor(254, 255, 228));
        painter.setPen(GRAY_COLOR_);
    }
    QString text = QString::number(w_index_);
    QRect textRect = painter.fontMetrics().boundingRect(text);
    textRect.moveCenter(center);
    painter.drawText(textRect, Qt::AlignCenter, text);
}
