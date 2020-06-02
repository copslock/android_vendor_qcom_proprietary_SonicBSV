/*===========================================================================

  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#pragma once

#include <set>

#include "framework/Log.h"
#include "framework/Message.h"
#include "dsi_netctrl.h"
#include "CallInfo.h"
#include "DSDModemEndPoint.h"
#include "WDSModemEndPoint.h"
#include "LocalLogBuffer.h"

#include "modules/nas/NasPhysChanConfigMessage.h"
#include "request/RilRequestSetupDataCallMessage.h"
#include "request/RilRequestDeactivateDataCallMessage.h"
#include "request/SetupDataCallRequestMessage.h"
#include "request/DeactivateDataCallRequestMessage.h"
#include "request/GetIWlanDataCallListRequestMessage.h"
#include "sync/GetCallBringUpCapabilitySyncMessage.h"

#define MaxConcurrentDataCalls DSI_MAX_DATA_CALLS

namespace rildata {

struct AvailableRadioDataService_t {
  RadioAccessFamily_t radioFamily;
  RadioFrequencyInfo_t radioFrequency;
};

/**
 * @brief CallManager class
 * @details CallManager class
 */
class CallManager {
public:
  CallManager(LocalLogBuffer& logBuffer);
  ~CallManager();

  void init(bool apAssistMode, bool partialRetry, unsigned long maxPartialRetryTimeout);
  void handleSetupDataCallRequestMessage(std::shared_ptr<Message> msg);
  void handleDeactivateDataCallRequestMessage(std::shared_ptr<Message> msg);
  void handleRilEventDataCallback(std::shared_ptr<Message> msg);
  void handleDataBearerTypeUpdate(int32_t cid, rildata::BearerInfo_t bearer);
  void handleDataAllBearerTypeUpdate(rildata::AllocatedBearer_t bearer);
  void handleNasPhysChanConfigMessage(std::shared_ptr<Message> msg);
  void handleHandoverInformationIndMessage(std::shared_ptr<Message> msg);
  void handleCallManagerEventMessage(std::shared_ptr<Message> msg);
  void handleHandoverTimeout(int cid);
  void handleSetupDataCallTimeout(int cid);
  void handlePartialRetryTimeout(int cid);
  void handlePartialRetryResponseTimeout(int cid);

  int toggleLinkActiveStateChangeReport(bool report);

  void setupDataCallCompleted(CallInfo *pCallInstance, SetupDataCallResponse_t result);
  void setupDataCallCompletedResp(CallInfo *pCallInstance, SetupDataCallResponse_t result);
  void deactivateDataCallCompleted(CallInfo *pCallInstance, ResponseError_t result);

  void getRadioDataCallList(vector<DataCallResult_t> &callList);
  void getIWlanDataCallList(vector<DataCallResult_t> &callList);

  int getAvailableCid(void);
  void dataCallListChanged(void);
  string getApnByCid(int cid);
  CallInfo *findCallInfo(int cid);


  ResponseError_t handleToggleBearerAllocationUpdate(bool enable);
  AllocatedBearerResult_t handleGetBearerAllocation(int32_t cid);
  AllocatedBearerResult_t handleGetAllBearerAllocations();
  void updateAllBearerAllocations();
  void cleanUpBearerAllocation(int32_t cid);
  void cleanUpAllBearerAllocation();
  void updatePhysicalChannelConfigs(void);
  void enablePhysChanConfigReporting(bool enable);
  void generateCurrentPhysicalChannelConfigs(const std::vector<NasPhysChanInfo> nasPhysChanConfigInfo);

  void processQmiDsdApnPreferredSystemResultInd(const dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind);
  void processQmiDsdIntentToChangeApnPrefSysInd(const dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *ind);
  void processQmiDsdSystemStatusInd(const dsd_system_status_ind_msg_v01 * ind);
  bool notifyApAssistApnPreferredSystemChangeSuported(bool flag);
  static bool sendSetApnPreferredSystemRequest(string apnName, int32_t prefSys);
  static void dsiNetCallback(dsi_hndl_t dsiHandle, void *userData,
    dsi_net_evt_t netEvent, dsi_evt_payload_t *payload);
  static bool isApnTypeInvalid(ApnTypes_t mSupportedApnTypesBitmap);
  static bool isApnTypeEmergency(ApnTypes_t mSupportedApnTypesBitmap);
  static std::string convertToString(const RatType_t& rat);
  static std::string convertToString(const BearerInfo_t& bearer);
  void dump(string padding, ostream& os);

private:
  LocalLogBuffer& logBuffer;
  bool mApAssistMode;
  bool mPartialRetryEnabled;
  bool mRadioDataAvailable;
  bool mRadioVoiceAvailable;
  unsigned long mMaxPartialRetryTimeout;
  bool mReportPhysicalChannelConfig;
  bool mReportLinkActiveStateChange;
  BringUpCapability mCallBringupCapability;
  std::set<CallInfo *> pCallInfoList;
  std::vector<DataCallResult_t> radioDcList;
  std::vector<DataCallResult_t> iwlanDcList;
  std::map<int32_t, rildata::AllocatedBearer_t> bearerCollection;
  std::vector<rildata::AvailableRadioDataService_t> availableRadioDataService;
  bool toggleBearerUpdateToDataConnectionService;
  bool availableCallIds[MaxConcurrentDataCalls];
  std::shared_ptr<WDSModemEndPoint> wds_endpoint;
  std::shared_ptr<DSDModemEndPoint> dsd_endpoint;
  bool insertCallInfo(CallInfo *pCallInstance);
  bool removeCallInfo(CallInfo *pCallinstance);
  void cleanCallInfo(CallInfo *pCallInstance);
  CallInfo *findCallInfo(ApnTypes_t apnTypes);
  CallInfo *findCallInfo(std::string apn);
  CallInfo *findCallInfo(std::string apn, ApnTypes_t apnTypes, std::string ipType);
  CallInfo *findEmergencyCallInfo(void);
  bool convertQmiDsdToAvailableRadioDataService(dsd_system_status_info_type_v01 sys,
                                                AvailableRadioDataService_t &radioDataService);
};

} /* namespace rildata */
