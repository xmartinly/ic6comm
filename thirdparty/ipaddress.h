#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <QWidget>

class QLabel;
class QLineEdit;

#ifdef quc
class Q_DECL_EXPORT IPAddress : public QWidget
#else
class IPAddress : public QWidget
#endif

{
    Q_OBJECT
    Q_PROPERTY(QString ip READ getIP WRITE setIP)


  public:
    explicit IPAddress(QWidget* parent = nullptr);
    ~IPAddress();

  protected:
    bool eventFilter(QObject* watched, QEvent* event);

  private:

    QString ip;
    QLabel* labDot1;    //first dot
    QLabel* labDot2;    //second dot
    QLabel* labDot3;    //third dot

    QLineEdit* txtIP1;  //IP section 1
    QLineEdit* txtIP2;  //IP section 2
    QLineEdit* txtIP3;  //IP section 3
    QLineEdit* txtIP4;  //IP section 4

  private slots:
    void textChanged(const QString& text);

  public:
    //get ip address input
    QString getIP()                 const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

  public Q_SLOTS:
    //set ip address
    void setIP(const QString& ip);
    //clear ip address
    void clear();

};

#endif // IPADDRESS_H
