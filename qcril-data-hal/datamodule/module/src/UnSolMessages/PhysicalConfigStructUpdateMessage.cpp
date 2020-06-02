/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "UnSolMessages/PhysicalConfigStructUpdateMessage.h"

using namespace rildata;

PhysicalConfigStructUpdateMessage::PhysicalConfigStructUpdateMessage
(
  const std::vector<PhysicalConfigStructInfo_t> &physicalConfigStructInfo
) : UnSolicitedMessage(get_class_message_id())
{
  mName = MESSAGE_NAME;
  mPhysicalConfigStructInfo = physicalConfigStructInfo;
}

const std::vector<PhysicalConfigStructInfo_t> & PhysicalConfigStructUpdateMessage::getPhysicalConfigStructInfo()
{
  return mPhysicalConfigStructInfo;
}

std::shared_ptr<UnSolicitedMessage> PhysicalConfigStructUpdateMessage::clone()
{
  return std::make_shared<PhysicalConfigStructUpdateMessage>(mPhysicalConfigStructInfo);
}

string PhysicalConfigStructUpdateMessage::dump()
{
  return MESSAGE_NAME;
}
