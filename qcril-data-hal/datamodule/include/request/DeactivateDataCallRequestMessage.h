/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include "MessageCommon.h"

using namespace std;

namespace rildata {

class DeactivateDataCallRequestMessage : public SolicitedMessage<ResponseError_t>,
                          public add_message_id<DeactivateDataCallRequestMessage> {
  private:
    int32_t mSerial;
    int32_t mCid;
    DataRequestReason_t mReason;
    shared_ptr<function<void(uint16_t)>> mAcknowlegeRequestCb;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.data.DeactivateDataCallRequestMessage";
    DeactivateDataCallRequestMessage() = delete;
    DeactivateDataCallRequestMessage(
      const int32_t serial,
      const int32_t cid,
      const DataRequestReason_t reason,
      const shared_ptr<function<void(uint16_t)>> ackCb
      ):SolicitedMessage<ResponseError_t>(get_class_message_id()) {

      mName = MESSAGE_NAME;
      mSerial = serial;
      mCid = cid;
      mReason = reason;
      mAcknowlegeRequestCb = ackCb;
    }
    ~DeactivateDataCallRequestMessage() = default;

    string dump(){return mName;}
    int32_t getSerial() {return mSerial;}
    int32_t getCid() {return mCid;}
    DataRequestReason_t getDataRequestReason() {return mReason;}
    shared_ptr<function<void(uint16_t)>> getAcknowlegeRequestCb() {return mAcknowlegeRequestCb;}
};

}
