/******************************************************************************
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "DSDModemEndPoint.h"
#include "sync/SetApnInfoSyncMessage.h"
#include "sync/RegisterForSystemStatusSyncMessage.h"
#include "sync/TurnOnAPAssistIWLANSyncMessage.h"
#include "sync/RegisterForAPAsstIWlanIndsSyncMessage.h"
#include "sync/TriggerDDSSwitchSyncMessage.h"
#include "UnSolMessages/DDSSwitchResultIndMessage.h"
#include "UnSolMessages/DsdSystemStatusMessage.h"
#include "MessageCommon.h"
#include <sstream>

using std::to_string;
using namespace rildata;
constexpr const char *DSDModemEndPoint::NAME;

void DSDModemEndPoint::requestSetup(string clientToken,
                                      GenericCallback<string>* callback)
{
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, callback);
  mModule->dispatch(shared_setupMsg);
}

void DSDModemEndPoint::registerForUiChangeInd()
{
  Log::getInstance().d("[DSDModemEndPoint]: registerForUiChangeInd ENTRY");
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    dsd_register_ui_info_change_req_msg_v01 qmiReq;
    dsd_register_ui_info_change_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));

    qmiReq.report_ui_changes_valid = true;
    qmiReq.report_ui_changes = 1;
    qmiReq.suppress_so_change_valid = true;
    qmiReq.suppress_so_change = 1;
    qmiReq.suppress_null_bearer_reason_valid = true;
    qmiReq.suppress_null_bearer_reason = 1;

    auto retVal = sendRawSync(QMI_DSD_REGISTER_UI_INFO_CHANGE_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint::registerForUiChangeInd]"
                           "Failed to send QMI_DSD_REGISTER_UI_CHANGE_REQ_V01");
    }
  } else {
    Log::getInstance().d("[DSDModemEndPoint::registerForUiChangeInd]"
                         "Failed to register for UI indication");
  }
}

Message::Callback::Status DSDModemEndPoint::setApnInfoSync
( const std::string apn_name,
  const uint8_t apn_type_mask)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::handleSetApnInfo]"
                         "dispatching message SetApnInfoSyncMessage");
    // Note that callback is not required for sync calls.
    auto msg = std::make_shared<SetApnInfoSyncMessage>(nullptr);
    msg->setParams(apn_name,apn_type_mask);
    Message::Callback::Status apiStatus;
    auto r = std::make_shared<int>();
    apiStatus = msg->dispatchSync(r);
    std::ostringstream ss;
    ss << "[DSDModemEndPoint::handleSetApnInfo] status = " << (int) apiStatus;
    Log::getInstance().d(ss.str());
    return apiStatus;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::handleSetApnInfo]"
                         "Failed to send message SetApnInfoSyncMessage");
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::sendAPAssistIWLANSupportedSync ( )
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::sendAPAssistIWLANSupportedSync]");
    // Note that callback is not required for sync calls.
    auto msg = std::make_shared<TurnOnAPAssistIWLANSyncMessage>(nullptr);
    Message::Callback::Status apiStatus;
    auto r = std::make_shared<bool>();
    apiStatus = msg->dispatchSync(r);
    std::ostringstream ss;
    ss << "[DSDModemEndPoint::sendAPAssistIWLANSupportedSync] status = " << (int) apiStatus;
    Log::getInstance().d(ss.str());
    return apiStatus;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::sendAPAssistIWLANSupportedSync]"
                         "Failed to send message TurnOnAPAssistIWLANSyncMessage");
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::registerForSystemStatusSync ( )
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForSystemStatusSync]"
                         "dispatching message RegisterForSystemStatusSyncMessage");
    // Note that callback is not required for sync calls.
    auto msg = std::make_shared<RegisterForSystemStatusSyncMessage>(nullptr);
    Message::Callback::Status apiStatus;
    auto r = std::make_shared<int>();
    apiStatus = msg->dispatchSync(r);
    std::ostringstream ss;
    ss << "[DSDModemEndPoint::registerForSystemStatusSync] status = " << (int) apiStatus;
    Log::getInstance().d(ss.str());
    return apiStatus;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::registerForSystemStatusSync]"
                         "Failed to send message RegisterForSystemStatusSyncMessage");
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::registerForCurrentDDSInd ( )
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    dsd_indication_register_req_msg_v01 ind_req;
    dsd_indication_register_resp_msg_v01 ind_resp;
    memset(&ind_req, 0, sizeof(ind_req));
    memset(&ind_resp, 0, sizeof(ind_resp));

    ind_req.report_current_dds_valid = true;
    ind_req.report_current_dds = 1;

    int rc = sendRawSync(QMI_DSD_INDICATION_REGISTER_REQ_V01,
                         (void *)&ind_req, sizeof(ind_req),
                         (void *)&ind_resp, sizeof(ind_resp),
                         QCRIL_DATA_QMI_TIMEOUT);
    if ((rc != QMI_NO_ERR) || (ind_resp.resp.result == QMI_RESULT_FAILURE_V01 )) {
      Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_INDICATION_REGISTER_REQ_V01");
    } else {
      Log::getInstance().d("[DSDModemEndPointModule] QMI_DSD_INDICATION_REGISTER_REQ_V01::Successfully sent");
      return Message::Callback::Status::SUCCESS;
    }
  } else {
    Log::getInstance().d("[DSDModemEndPoint::registerForCurrentDDSInd]"
                         "Failed to send message registerForCurrentDDSInd");
  }
  return Message::Callback::Status::FAILURE;
}

Message::Callback::Status DSDModemEndPoint::registerForAPAsstIWlanIndsSync (bool toRegister )
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::registerForAPAsstIWlanIndsSync]"
                         "dispatching message RegisterForAPAsstIWlanIndsSyncMessage register "
                         + std::to_string(toRegister));
    // Note that callback is not required for sync calls.
    auto msg = std::make_shared<RegisterForAPAsstIWlanIndsSyncMessage>(nullptr);
    Message::Callback::Status apiStatus;
    auto r = std::make_shared<int>();
    msg->setParams(toRegister);
    apiStatus = msg->dispatchSync(r);
    std::ostringstream ss;
    ss << "[DSDModemEndPoint::registerForAPAsstIWlanIndsSync] status = " << (int) apiStatus;
    Log::getInstance().d(ss.str());
    return apiStatus;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::registerForAPAsstIWlanIndsSync]"
                         "Failed to send message RegisterForAPAsstIWlanIndsSyncMessage");
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::setApnPreferredSystemChangeSync(
  const std::string apnName, const int32_t prefRat)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::setApnPreferredSystemChangeSync]"
                         "dispatching message SetApnPreferredSystemSyncMessage");

    dsd_set_apn_preferred_system_req_msg_v01 qmiReq;
    dsd_set_apn_preferred_system_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));

    strlcpy(qmiReq.apn_pref_sys.apn_name, apnName.c_str(), QMI_DSD_MAX_APN_LEN_V01+1);
    qmiReq.apn_pref_sys.pref_sys = (dsd_apn_pref_sys_enum_v01)prefRat;
    qmiReq.is_ap_asst_valid = true;
    qmiReq.is_ap_asst = true;
    auto retVal = sendRawSync(QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint] Failed to send"
        "QMI_DSD_SET_APN_PREFERRED_SYSTEM_REQ_V01");
      return Message::Callback::Status::FAILURE;
    } else {
      return Message::Callback::Status::SUCCESS;
    }
  } else {
    Log::getInstance().d("[DSDModemEndPoint::setApnPreferredSystemChangeSync]"
                         "Failed to send message SetApnPreferredSystemSyncMessage");
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::getCurrentDDSSync(DDSSubIdInfo &ddsInfo)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    dsd_get_current_dds_req_msg_v01 qmiReq;
    dsd_get_current_dds_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));
    auto retVal = sendRawSync(QMI_DSD_GET_CURRENT_DDS_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint] Failed to send"
        "QMI_DSD_GET_CURRENT_DDS_REQ_V01");
      return Message::Callback::Status::FAILURE;
    } else if (!qmiResp.dds_valid) {
      // TODO: discuss how to handle case where temporary dds is returned

      Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_GET_CURRENT_DDS_REQ_V01 "
        "did not return dds");
      return Message::Callback::Status::FAILURE;
    }
    switch (qmiResp.dds) {
      case DSD_PRIMARY_SUBS_V01:
        ddsInfo.dds_sub_id = 0;
        break;
      case DSD_SECONDARY_SUBS_V01:
        ddsInfo.dds_sub_id = 1;
        break;
      default:
        Log::getInstance().d("[DSDModemEndPoint] QMI_DSD_GET_CURRENT_DDS_REQ_V01 "
          "invalid subId=" + std::to_string(qmiResp.dds));
        return Message::Callback::Status::FAILURE;
    }
    if(qmiResp.dds_switch_type_valid) {

      switch(qmiResp.dds_switch_type) {
        case DSD_DDS_SWITCH_PERMANENT_V01:
          ddsInfo.switch_type = DSD_DDS_DURATION_PERMANANT_V01;
          break;
        case DSD_DDS_SWITCH_TEMPORARY_V01:
          ddsInfo.switch_type = DSD_DDS_DURATION_TEMPORARY_V01;
          break;
        default:
          ddsInfo.switch_type = DSD_DDS_DURATION_PERMANANT_V01;
      }
    } else {
      ddsInfo.switch_type = DSD_DDS_DURATION_PERMANANT_V01; //default value
    }
    return Message::Callback::Status::SUCCESS;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::getCurrentDDSSync]"
                         "Failed to send message QMI_DSD_GET_CURRENT_DDS_REQ_V01");
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::triggerDDSSwitchSync(int subId, int& error, dsd_dds_switch_type_enum_v01 switch_type)
{
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitchSync]"
                         "dispatching message TriggerDDSSwitchSyncMessage");
    auto msg = std::make_shared<TriggerDDSSwitchSyncMessage>(subId, switch_type);
    auto retVal = std::make_shared<SetPreferredDataModemResult_t>();
    auto apiStatus = msg->dispatchSync(retVal);
    Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitchSync] status=" +
                         std::to_string(static_cast<int>(apiStatus)));
    if (retVal == nullptr) {
      Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitchSync] returned null");
      error = static_cast<int>(SetPreferredDataModemResult_t::QMI_ERROR);
      return Message::Callback::Status::FAILURE;
    }
    error = static_cast<int>(*retVal);
    Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitchSync] returned errorcode=" +
                         std::to_string(error));
    return apiStatus;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::triggerDDSSwitchSync]"
                         "Failed to send message TriggerDDSSwitchSyncMessage");
    error = static_cast<int>(SetPreferredDataModemResult_t::MODEM_ENDPOINT_NOT_OPERATIONAL);
    auto msg = std::make_shared<DDSSwitchResultIndMessage>();
    msg->setError(TriggerDDSSwitchError::MODEM_NOT_UP);
    msg->broadcast();
    return Message::Callback::Status::FAILURE;
  }
}

Message::Callback::Status DSDModemEndPoint::getUiInfoSync(dsd_sys_ui_mask_v01& uiMask)
{
  Log::getInstance().d("[DSDModemEndPoint]: getUiInfoSync ENTRY");
  if (getState() == ModemEndPoint::State::OPERATIONAL)
  {
    dsd_get_ui_info_req_msg_v01 qmiReq;
    dsd_get_ui_info_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));

    auto retVal = sendRawSync(QMI_DSD_GET_UI_INFO_REQ_V01,
                              (void *)&qmiReq, sizeof(qmiReq),
                              (void *)&qmiResp, sizeof(qmiResp),
                              QCRIL_DATA_QMI_TIMEOUT);
    if (retVal != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint::getUiInfoSync]"
                           "Failed to send QMI_DSD_GET_UI_INFO_REQ_V01");
      return Message::Callback::Status::FAILURE;
    }
    if (qmiResp.global_ui_info_valid) {
      uiMask = qmiResp.global_ui_info.ui_mask;
      return Message::Callback::Status::SUCCESS;
    }
    Log::getInstance().d("[DSDModemEndPoint::getUiInfoSync] bad QMI response");
    return Message::Callback::Status::FAILURE;
  } else {
    Log::getInstance().d("[DSDModemEndPoint::getUiInfoSync] Failed to get UI info");
    uiMask = QMI_DSD_UI_MASK_UNSPECIFIED_V01;
    return Message::Callback::Status::SUCCESS;
  }
}

int DSDModemEndPoint::toggleLimitedSysIndicationChangeReport(bool report)
{
  Log::getInstance().d("[DSDModemEndPoint]: toggleLimitedSysIndicationChangeReport "+std::to_string((int)report));
  if (mReportLimitedSysIndicationChange != report) {
    mReportLimitedSysIndicationChange = report;

    dsd_system_status_change_req_msg_v01  qmiReq;
    dsd_system_status_change_resp_msg_v01 qmiResp;
    memset(&qmiReq, 0, sizeof(qmiReq));
    memset(&qmiResp, 0, sizeof(qmiResp));

    qmiReq.limit_so_mask_change_ind_valid = true;
    qmiReq.limit_so_mask_change_ind = report;

    auto retVal = sendRawSync(QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                          (void *)&qmiReq, sizeof(qmiReq),
                          (void *)&qmiResp, sizeof(qmiResp),
                          QCRIL_DATA_QMI_TIMEOUT);
    if (retVal != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPoint::toggleLimitedSysIndicationChangeReport] Failed to register");
    }

    if (mReportLimitedSysIndicationChange) {
      dsd_get_system_status_resp_msg_v01  sysRespMsg;
      dsd_system_status_ind_msg_v01 *ind_data = nullptr;
      memset(&sysRespMsg, 0, sizeof(sysRespMsg));
      retVal = sendRawSync(QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                            nullptr, 0,
                            (void *)&sysRespMsg, sizeof(sysRespMsg),
                            QCRIL_DATA_QMI_TIMEOUT);
      if (retVal == QMI_NO_ERR) {
        Log::getInstance().d("[DSDModemEndPoint::toggleLimitedSysIndicationChangeReport] success to query");
        ind_data = (dsd_system_status_ind_msg_v01 *)((char *)&sysRespMsg +
                                                    offsetof(dsd_get_system_status_resp_msg_v01,
                                                    avail_sys_valid));

        auto indMsg = std::make_shared<rildata::DsdSystemStatusMessage>(*ind_data);
        indMsg->broadcast();
      }
      else {
        Log::getInstance().d("[DSDModemEndPoint::toggleLimitedSysIndicationChangeReport] Failed to query");
      }
    }
  }

  return 0;
}
