#ifndef INST_MSG_H
#define INST_MSG_H


namespace InstMsg {

    class InstResponse {
      public:
        explicit InstResponse(const QByteArray& resp): ba_data_(resp) {
            // "\x14\x00\x00\x5F\x06\x49\x43\x36\x20\x56\x65\x72\x73\x69\x6F\x6E\x20\x30\x2E\x31\x34\x00\x10"
            cks_   = ba_data_.back() & 0xff;
            ccb_   = ba_data_.at(2) & 0xff;
            timer_ = ba_data_.at(3) & 0xff;
            resp_len_ = ba_data_.length();
            msg_body_ = ba_data_.mid(2, resp_len_ - 3);
            ccb_err_msg_ = ba_data_.at(resp_len_ - 2);
            ccb_err_  = QMetaEnum::fromType<CygEnums::RespErr>();
            resp_err_ = ccb_ == 128 ? static_cast<CygEnums::RespErr>(ccb_err_msg_) : CygEnums::RespErr::NoErr;
        }

        ///
        /// \brief cksOk
        /// \return
        ///
        const bool cksOk() const {
            return Helper::chksumVerify(msg_body_, cks_);
        }

        ///
        /// \brief errMsg
        /// \return
        ///
        // const QString errMsg() const {
        //     QString msg_info = "Hex:" + Helper::hexFormat(ba_data_)
        //                        + ";MsgLenght:" + QString::number(resp_len_ - 5)
        //                        + ";TotalLength:" + QString::number(resp_len_)
        //                        + ";CCB:" + QString::number(ccb_)
        //                        + ";CheckSumCalc:" + QString::number(Helper::calcCks(msg_body_))
        //                        + ";CheckSum:" + QString::number(cks_) + "\n";
        //     return msg_info;
        // }

        ///
        /// \brief respStatus
        /// \return
        ///
        const CygEnums::RespErr respStatus() const {
            return resp_err_;
        }

        ///
        /// \brief respStatus
        /// \return
        ///
        const QString respStatusString() const {
            return QString(ccb_err_.valueToKey(resp_err_));
        }

        ///
        /// \brief respMsg
        /// \param only_body
        /// \return
        ///
        const QByteArray respMsg(bool only_message = true) const {
            return only_message ? ba_data_.mid(5, resp_len_ - 6) : ba_data_;
        }

        ///
        /// \brief actionOK
        /// \return
        ///
        const bool actionOK() const {
            bool ack_ok = static_cast<int>(ba_data_.at(4)) == 6;
            return cksOk() && ack_ok;
        }

        ///
        /// \brief respTmr
        /// \return
        ///
        const int respTmr() const {
            return timer_;
        }

      protected:
        QByteArray ba_data_;
        QByteArray msg_body_;
        QMetaEnum ccb_err_;
        CygEnums::RespErr resp_err_;
        int ccb_err_msg_;
        int resp_len_;
        int ccb_;
        int timer_;
        int cks_;
    };

}

#endif // INST_MSG_H
