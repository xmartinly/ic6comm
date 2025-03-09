#ifndef CIRCLEWIDGET_h
#define CIRCLEWIDGET_h
#include <QWidget>

class CircleWidget : public QWidget {

  public:
    explicit CircleWidget(QWidget* parent = nullptr);

    void setValue(int value);
    void setIndex(int idx);

  protected:
    void paintEvent(QPaintEvent*) override;

  private:
    int w_index_ = 0;    // 显示的数字（固定）
    int w_value = 0;     // 控制颜色的值

    const qreal red_factor_ = 0.31875;
    const int GREEN_VAL_ = 160;
    const int BLUE_VAL_ = 0;
    int red_val_ = 0;
    const QColor RED_COLOR_ = QColor::fromRgb(204, 51, 0);

};

#endif // CIRCLEWIDGET_h
