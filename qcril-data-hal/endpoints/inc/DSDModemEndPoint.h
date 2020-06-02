/******************************************************************************
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <vector>
#include "modules/qmi/ModemEndPoint.h"
#include "DSDModemEndPointModule.h"
#include "framework/Log.h"
#include "DataCommon.h"

class DSDModemEndPoint : public ModemEndPoint
{
private:
  bool mReportLimitedSysIndicationChange;
public:
  static constexpr const char *NAME = "DSDModemEndPoint";
  DSDModemEndPoint() : ModemEndPoint(NAME)
  {
    mModule = new DSDModemEndPointModule("DSDModemEndPointModule", *this);
    mModule->init();
    mReportLimitedSysIndicationChange = false;
    Log::getInstance().d("[DSDModemEndPoint]: xtor");
  }
  ~DSDModemEndPoint()
  {
      Log::getInstance().d("[DSDModemEndPoint]: destructor");
    //mModule->killLooper();
    delete mModule;
    mModule = nullptr;
  }

  void requestSetup(string clientToken, GenericCallback<string>* cb);
  void registerForUiChangeInd();

  /**
   * @brief      Sets the apn information synchronously
   *
   * @param[in]  apn_name       The apn name
   * @param[in]  apn_type_mask  The apn type mask
   *
   * @return     { description_of_the_return_value }
   */
  Message::Callback::Status setApnInfoSync(const std::string apn_name,
   const uint8_t apn_type_mask);

  Message::Callback::Status sendAPAssistIWLANSupportedSync( );

  Message::Callback::Status registerForSystemStatusSync( );

  Message::Callback::Status registerForCurrentDDSInd( );

  Message::Callback::Status registerForAPAsstIWlanIndsSync(bool toRegister );

  Message::Callback::Status setApnPreferredSystemChangeSync(const std::string apnName, const int32_t prefRat);

  Message::Callback::Status getCurrentDDSSync(DDSSubIdInfo &subId);

  Message::Callback::Status triggerDDSSwitchSync(int subId, int &error, dsd_dds_switch_type_enum_v01 switch_type);

  Message::Callback::Status getUiInfoSync(dsd_sys_ui_mask_v01 &uiMask);

  int toggleLimitedSysIndicationChangeReport(bool report);
};
