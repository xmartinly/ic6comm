#ifndef ACQUIRE_RECIPE_H
#define ACQUIRE_RECIPE_H

namespace Utility {
    class AcquireRecipe {
      public:
        explicit AcquireRecipe(const QByteArray& cmd, QList<CygEnums::CommandType> cmd_tp): acq_cmd_(cmd), cmd_tp_(cmd_tp) {
        }

        inline qint64 runTime(qint64 curr_secs = 0) const {
            return run_time_ - curr_secs;
        }
        inline void setRunTime(int new_run_time) {
            qint64 curr_msecs = QDateTime::currentSecsSinceEpoch();
            run_time_ = new_run_time ? (new_run_time + curr_msecs) : (Helper::maxRunningTime() + curr_msecs);
        }

        ///
        /// \brief interval
        /// \return
        ///
        inline int interval() const {
            return interval_;
        }
        inline void setInterval(int new_interval) {
            interval_ = new_interval;
        }

        ///
        /// \brief acqCmd
        /// \return
        ///
        inline QByteArray acqCmd() const {
            return acq_cmd_;
        }

        inline QList<CygEnums::CommandType> cmdTypes() const {
            return cmd_tp_;
        }

        ///
        /// \brief isSave
        /// \return
        ///
        inline bool isSave() const {
            return is_save_;
        }
        inline void setIsSave(bool new_is_save) {
            is_save_ = new_is_save;
        }

        ///
        /// \brief isAllSettle
        /// \return
        ///
        inline bool isRecipeSettle() const {
            return run_time_ > -1 && interval_ > -1 && acq_cmd_.length() > 0;
        }
        inline const void clearRecipe() {
            acq_cmd_ = {};
            run_time_ = -1;
            interval_ = -1;
            is_save_ = true;
            cmd_tp_ = {};
        }

      private:
        QByteArray acq_cmd_;
        qint64 run_time_ = -1;
        int interval_ = -1;
        bool is_save_ = true;
        QList<CygEnums::CommandType> cmd_tp_;
    };
}

#endif // ACQUIRE_RECIPE_H
