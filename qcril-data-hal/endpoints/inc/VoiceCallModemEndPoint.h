/******************************************************************************
#  Copyright (c) 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "modules/qmi/ModemEndPoint.h"
#include "VoiceCallModemEndPointModule.h"
#include "framework/Log.h"
#include "voice_service_v02.h"

class VoiceCallModemEndPoint : public ModemEndPoint
{
public:
  VoiceCallModemEndPoint(std::string moduleName,int sub) : ModemEndPoint(moduleName)
  {
    mModule = new VoiceCallModemEndPointModule("VoiceCallModemEndPointModule", *this, sub);
    mModule->init();
    Log::getInstance().d("[VoiceCallModemEndPoint]: xtor");
  }
  ~VoiceCallModemEndPoint()
  {
    Log::getInstance().d("[VoiceCallModemEndPoint]: destructor");
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, GenericCallback<string>* cb);
};
