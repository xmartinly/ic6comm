#ifndef EQUIP_INSTANCE_H
#define EQUIP_INSTANCE_H

#include <QObject>
namespace Utility {
    class EquipInstance: public QObject {
        Q_OBJECT
      public:
        EquipInstance( InstConn* const inst_conn, AcquireRecipe* const recipe_set, QObject* parent);
        EquipInstance(const EquipInstance&)  = delete;
        // explicit EquipInstance(const EquipInstance& equip);
        ~EquipInstance();

      public:
        int acqStat() const;
        const bool setAcqStat();
        void newRecipe(AcquireRecipe recipe_set);
      private:
        InstConn* conn_ = nullptr;
        AcquireRecipe* recipe_ = nullptr;
        QTimer* acq_tmr_ = nullptr;
        QString reponse_str_;
        bool timely_model_ = false;
        int acq_stat_ = -1;

      private:
        void timelySendCmd();
        void setupAcq();

      private slots:
        void acqTmrAction();

      public slots:
        void commResp(const QByteArray& resp, qint64 tm);
        void connErr(const QString& err);
    };



};

#endif // EQUIP_INSTANCE_H
