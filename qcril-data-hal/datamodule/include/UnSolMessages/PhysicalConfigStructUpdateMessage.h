/**
* Copyright (c) 2019 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"
#include "MessageCommon.h"

namespace rildata {

struct PhysicalConfigStructInfo_t {
  CellConnectionStatus_t status;
  int32_t cellBandwidthDownlink;
  RadioAccessFamily_t rat;
  RadioFrequencyInfo_t rfInfo;
  std::vector<int32_t> contextIds;
  uint32_t physicalCellId;
};

/********************** Class Definitions *************************/
class PhysicalConfigStructUpdateMessage: public UnSolicitedMessage,
                           public add_message_id<PhysicalConfigStructUpdateMessage> {

private:
  std::vector<PhysicalConfigStructInfo_t> mPhysicalConfigStructInfo;
public:
  static constexpr const char *MESSAGE_NAME = "PhysicalConfigStructUpdateMessage";

  PhysicalConfigStructUpdateMessage() = delete;
  ~PhysicalConfigStructUpdateMessage(){};
  PhysicalConfigStructUpdateMessage(std::vector<PhysicalConfigStructInfo_t> const &physicalConfigInfo);

  std::shared_ptr<UnSolicitedMessage> clone();
  const std::vector<PhysicalConfigStructInfo_t> & getPhysicalConfigStructInfo();
  string dump();
};

} //namespace
