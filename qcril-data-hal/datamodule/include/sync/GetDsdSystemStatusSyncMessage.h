/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"

#include "MessageCommon.h"
#include "data_system_determination_v01.h"

using namespace std;

namespace rildata {

struct DsdSystemStatusResult_t {
  dsd_get_system_status_resp_msg_v01 resp;
};

class GetDsdSystemStatusSyncMessage : public SolicitedSyncMessage<DsdSystemStatusResult_t>,
                                    public add_message_id<GetDsdSystemStatusSyncMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.data.GetDsdSystemStatusSyncMessage";
  inline GetDsdSystemStatusSyncMessage() :
    SolicitedSyncMessage<DsdSystemStatusResult_t>(get_class_message_id())
  {
    mName = MESSAGE_NAME;
  }
  ~GetDsdSystemStatusSyncMessage();

  string dump();
};

}