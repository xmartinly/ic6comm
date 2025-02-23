#ifndef INST_CONN_H
#define INST_CONN_H

#include <QObject>

namespace Utility {

    class InstConn: public QObject {
        Q_OBJECT
      public:
        explicit InstConn(const QString& conn_way,
                          int conn_port_bdrt,
                          CygEnums::ConnType conn_type = CygEnums::TcpIpConn,
                          QObject* parent = nullptr);


        InstConn(const InstConn&)  = delete;

        InstConn() {};
        virtual ~InstConn() {};

      public:
        virtual void sendCmd(const QByteArray&) = 0;
        virtual void delConn() = 0;
        virtual bool setupConn() = 0;
        virtual bool connState() = 0;
        virtual bool verifyParam() = 0;

      protected:
        bool read_fin_ = false;
        QString conn_way_;
        int conn_port_bdrt_;
        CygEnums::ConnType conn_type_;

      protected slots:
        virtual void recvResp() = 0;


      signals:
        void sendResp(const QByteArray&, qint64);
        void sendConnErr(const QString&);
    };

    ///
    /// \brief The TcpConn class
    ///
    class TcpConn: public InstConn {
        Q_OBJECT
      public:
        using InstConn::InstConn;
        explicit TcpConn(const TcpConn& tcp);
        ~TcpConn();

      public:
        void sendCmd(const QByteArray& cmd);
        void delConn();
        bool setupConn();
        bool connState();
        bool verifyParam();

      private:
        QTcpSocket* socket_;

      private slots:
        void recvResp();

    };

    ///
    /// \brief The SerConn class
    ///
    class SerConn: public InstConn {
        Q_OBJECT
      public:
        using InstConn::InstConn;
        explicit SerConn(const SerConn& ser);
        ~SerConn();
      public:
        void sendCmd(const QByteArray& cmd);
        void delConn();
        bool setupConn();
        bool connState();
        bool verifyParam();

      private:
        QSerialPort* ser_;
        QList<uint> ser_bdrt_list_ = {115200, 57600, 19200, 9600};

      private slots:
        void recvResp();

    };
}

#endif // INST_CONN_H
