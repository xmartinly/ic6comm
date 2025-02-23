#include "equip_instance.h"

using namespace Utility;

EquipInstance::EquipInstance(InstConn* const inst_conn, AcquireRecipe* const recipe_set, QObject* parent)
    : QObject(parent),
      conn_(inst_conn),
      recipe_(recipe_set) {
    connect(conn_, &InstConn::sendResp, this, &EquipInstance::commResp);
    connect(conn_, &InstConn::sendConnErr, this, &EquipInstance::connErr);
    acq_tmr_ = new QTimer(this);
    setupAcq();
}


EquipInstance::~EquipInstance() {
    disconnect(conn_, &InstConn::sendResp, this, &EquipInstance::commResp);
    disconnect(conn_, &InstConn::sendConnErr, this, &EquipInstance::connErr);
}

///
/// \brief EquipInstance::acqStat
/// \return int. 0:stopped; 1: acquiring; -1; unkonwn
///
int EquipInstance::acqStat() const {
    return acq_stat_;
}

///
/// \brief EquipInstance::setAcqStat
/// \return
///
const bool EquipInstance::setAcqStat() {
    switch (acq_stat_) {
        case 1:
            acq_tmr_->stop();
            break;
        default:
            acq_tmr_->start();
            break;
    }
    return acq_tmr_->isActive();
}

void EquipInstance::newRecipe(AcquireRecipe recipe_set) {
    acq_stat_ = 0;
    acq_tmr_->stop();
    recipe_ = &recipe_set;
    setupAcq();
}

///
/// \brief EquipInstance::timelySendCmd
///
void EquipInstance::timelySendCmd() {
    if(timely_model_) { //timel loop, when set timer interval to 0
        QTimer::singleShot(5, this, &EquipInstance::acqTmrAction);
        qDebug() << __FUNCTION__;
    }
}

///
/// \brief EquipInstance::setupAcq
///
void EquipInstance::setupAcq() {
    if(!recipe_ || !recipe_->isRecipeSettle()) {
        return;
    }
    int interval = recipe_->interval();
    timely_model_ = interval > 0;
    if(!timely_model_) {
        acq_tmr_->setInterval(interval);
    }
}

void EquipInstance::acqTmrAction() {
    if(conn_->connState()) {
        conn_->sendCmd(recipe_->acqCmd());
    }
}

void EquipInstance::commResp(const QByteArray& resp, qint64 tm) {
    qDebug() << __FUNCTION__ << resp << tm;
    if(recipe_->runTime(tm) < 0) { //stop timer if reach run time setting
        acq_tmr_->stop();
        return;
    }
    if(timely_model_) { //timel loop, when set timer interval to 0
        QTimer::singleShot(5, this, &EquipInstance::acqTmrAction);
        return;
    }
}

void EquipInstance::connErr(const QString& err) {
    qDebug() << __FUNCTION__ << err;
}
