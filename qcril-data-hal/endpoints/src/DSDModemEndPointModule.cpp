/******************************************************************************
#  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include <string.h>
#include "DSDModemEndPointModule.h"
#include <modules/qmi/QmiIndMessage.h>
#include "sync/SetApnInfoSyncMessage.h"
#include "UnSolMessages/DsdSystemStatusMessage.h"
#include "UnSolMessages/SetApnPreferredSystemResultMessage.h"
#include "UnSolMessages/IntentToChangeApnPreferredSystemMessage.h"
#include "sync/RegisterForSystemStatusSyncMessage.h"
#include "sync/TurnOnAPAssistIWLANSyncMessage.h"
#include "sync/RegisterForAPAsstIWlanIndsSyncMessage.h"
#include "sync/TriggerDDSSwitchSyncMessage.h"
#include "UnSolMessages/CurrentDDSIndMessage.h"
#include "UnSolMessages/DDSSwitchResultIndMessage.h"
#include "UnSolMessages/DDSSwitchTimeoutMessage.h"
#include "MessageCommon.h"

#define TAG "DSD EndpointM"

using namespace rildata;
#include "UnSolMessages/DataNrIconTypeIndMessage.h"

DSDModemEndPointModule::DSDModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner), ddsSwitchRequestTimer(TimeKeeper::no_timer)
{
  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(SetApnInfoSyncMessage, DSDModemEndPointModule::handleSetApnInfoSync),
      {REG_MSG("DSDModemEndPoint_QMI_IND"),
        std::bind(&DSDModemEndPointModule::handleQmiDsdIndMessage, this, _1)},
      HANDLER(RegisterForSystemStatusSyncMessage, DSDModemEndPointModule::handleRegisterForSystemStatusSync),
      HANDLER(RegisterForAPAsstIWlanIndsSyncMessage, DSDModemEndPointModule::handleRegisterForAPAsstIWlanIndsSync),
      HANDLER(TurnOnAPAssistIWLANSyncMessage, DSDModemEndPointModule::handleTurnOnAPAssistIWLANSync),
      HANDLER(TriggerDDSSwitchSyncMessage, DSDModemEndPointModule::handleTriggerDDSSwitchSyncMessage),
  };
}

DSDModemEndPointModule::~DSDModemEndPointModule()
{
  mLooper = nullptr;
}

void DSDModemEndPointModule::init()
{
    ModemEndPointModule::init();
    mStackId[0] = 0;
}

qmi_idl_service_object_type DSDModemEndPointModule::getServiceObject()
{
  return dsd_get_service_object_v01();
}

bool DSDModemEndPointModule::handleQmiBinding
(
  qcril_instance_id_e_type instanceId, int8_t stackId)
{
  Log::getInstance().d(
        "[DSDModemEndPointModule] handleQmiBinding");

  (void)instanceId;
  if (stackId < 0)
    return false;

  dsd_bind_subscription_req_msg_v01 bind_request;
  dsd_bind_subscription_resp_msg_v01 bind_resp;
  memset(&bind_request, 0, sizeof(bind_request));
  memset(&bind_resp, 0, sizeof(bind_resp));

  if (stackId == 0) {
    bind_request.bind_subs = DSD_PRIMARY_SUBS_V01;
  } else if (stackId == 1) {
    bind_request.bind_subs = DSD_SECONDARY_SUBS_V01;
  } else if (stackId == 2) {
    bind_request.bind_subs = DSD_TERTIARY_SUBS_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_DSD_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&bind_request, sizeof(bind_request),
         (void*)&bind_resp, sizeof(bind_resp),
         ModemEndPointModule::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d(
        "[DSDModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);
  return false;
}

void DSDModemEndPointModule::handleQmiDsdIndMessage
(
  std::shared_ptr<Message> msg
)
{
  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  Log::getInstance().d("[DSDModemEndPointModule]: Handling msg = " + msg->dump());

  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    indicationHandler(indData->msgId, indData->indData, indData->indSize);
  }
  else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void DSDModemEndPointModule::indicationHandler
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
)
{
  (void)decoded_payload_len;
  Log::getInstance().d("[DSDModemEndPointModule]: indicationHandler ENTRY");

  if(decoded_payload == nullptr) {
    Log::getInstance().d("decoded_payload is nullptr");
  }
  else {
    Log::getInstance().d("msg_id = "+ std::to_string(msg_id));
    switch(msg_id)
    {
      case QMI_DSD_SYSTEM_STATUS_IND_V01:
        processSystemStatusInd((dsd_system_status_ind_msg_v01 *)decoded_payload);
        break;

      case QMI_DSD_INTENT_TO_CHANGE_APN_PREF_SYS_IND_V01:
        processIntentToChangeApnPrefSysInd((dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *)decoded_payload);
        break;

      case QMI_DSD_AP_ASST_APN_PREF_SYS_RESULT_IND_V01:
        processApAsstApnPrefSysResultInd((dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *)decoded_payload);
        break;

      case QMI_DSD_SWITCH_DDS_IND_V01:
        processQmiDdsSwitchInd((dsd_switch_dds_ind_msg_v01*)decoded_payload);
        break;

      case QMI_DSD_CURRENT_DDS_IND_V01:
        processQmiCurrentDdsInd((dsd_current_dds_ind_msg_v01*)decoded_payload);
        break;

      case QMI_DSD_UI_INFO_IND_V01:
        processUiInfoInd((dsd_ui_info_ind_msg_v01 *)decoded_payload);
        break;

      default:
        Log::getInstance().d("Unexpected dsd indication, ignoring = "+ std::to_string(msg_id));
        break;
    }
  }
}

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param ind_data [description]
 */
void DSDModemEndPointModule::processSystemStatusInd(dsd_system_status_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processSystemStatusInd");

  if (!ind_data) {
    Log::getInstance().d("[DSDModemEndPointModule]: invalid input");
    return;
  }

  auto msg = std::make_shared<rildata::DsdSystemStatusMessage>(*ind_data);
  msg->broadcast();

  // TODO: eveluate data registration state
}

/**
 * @brief This indication is from modem that preferred RAT is changed
 * @details Modem wants to handoff to target RAT and inform to QNP to update
 *          preferred RAT change to Transport Manager
 *
 * @param ind_data [description]
 */
void DSDModemEndPointModule::processIntentToChangeApnPrefSysInd(dsd_intent_to_change_apn_pref_sys_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[NetworkServiceHandler]: processQmiDsdIntentToChangeApnPrefSysInd = "+std::to_string(ind_data->apn_pref_sys_len));

  int numOfApns = ind_data->apn_pref_sys_len;
  for(int i=0 ; i<numOfApns ; i++) {
    //ind_data->apn_pref_sys[i].apn_name;
    // TODO: Search APN type for apn name
    // if data for APN name is connected, change the state to HANDOVER

    //ind_data->apn_pref_sys[i].pref_sys;
  }

  auto msg = std::make_shared<rildata::IntentToChangeApnPreferredSystemMessage>(*ind_data);
  msg->broadcast();
}

/**
 *
 */
void DSDModemEndPointModule::processApAsstApnPrefSysResultInd(dsd_ap_asst_apn_pref_sys_result_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[NetworkServiceHandler]: processApAsstApnPrefSysResultInd = "+std::to_string(ind_data->result_info.result));

  auto msg = std::make_shared<rildata::SetApnPreferredSystemResultMessage>(*ind_data);
  msg->broadcast();
}

void DSDModemEndPointModule::processQmiDdsSwitchInd(dsd_switch_dds_ind_msg_v01 *ind)
{
  if (ind == nullptr) {
    Log::getInstance().d("[DSDModemEndPointModule]: invalid input");
    return;
  }
  Log::getInstance().d("[DSDModemEndPointModule]:"
    "QMI_DSD_SWITCH_DDS_IND_V01: result = " + std::to_string(ind->dds_switch_result));
  auto msg = std::make_shared<DDSSwitchResultIndMessage>();
  switch (ind->dds_switch_result) {
    case DSD_DDS_SWITCH_ALLOWED_V01:
      Log::getInstance().d("[DSDModemEndPointModule]:DsdUnsolicitedIndHdlr: dds switch allowed");
      msg->setAllowed(true);
      break;
    case DSD_DDS_SWITCH_NOT_ALLOWED_V01:
      Log::getInstance().d("[DSDModemEndPointModule]:DsdUnsolicitedIndHdlr: dds switch not allowed");
      msg->setAllowed(false);
      break;
    case DSD_DDS_SWITCH_FAILED_V01:
      Log::getInstance().d("[DSDModemEndPointModule]:DsdUnsolicitedIndHdlr: dds switch failed");
      msg->setFailed(true);
      break;
    default:
      Log::getInstance().d("[DSDModemEndPointModule]:DsdUnsolicitedIndHdlr: dds switch result invalid");
      msg->setFailed(true);
      break;
  }
  msg->broadcast();
  if (ddsSwitchRequestPending) {
    TimeKeeper::getInstance().clear_timer(ddsSwitchRequestTimer);
    ddsSwitchRequestPending = false;
  }
}

void DSDModemEndPointModule::processQmiCurrentDdsInd(dsd_current_dds_ind_msg_v01 *ind)
{
  if (ind == nullptr) {
    Log::getInstance().d("[DSDModemEndPointModule]: invalid input");
    return;
  }
  Log::getInstance().d("[DSDModemEndPointModule]:"
        "QMI_DSD_CURRENT_DDS_IND_V01: dds=" + std::to_string(ind->dds));
  dsd_dds_switch_type switchType = static_cast<dsd_dds_switch_type>(ind->dds_switch_type);
  switch (ind->dds) {
    case DSD_PRIMARY_SUBS_V01:
    {
      auto msg = std::make_shared<CurrentDDSIndMessage>(0, switchType);
      msg->broadcast();
      break;
    }
    case DSD_SECONDARY_SUBS_V01:
    {
      auto msg = std::make_shared<CurrentDDSIndMessage>(1, switchType);
      msg->broadcast();
      break;
    }
    default:
      break;
  }
}

void DSDModemEndPointModule::processUiInfoInd(dsd_ui_info_ind_msg_v01 *ind_data)
{
  Log::getInstance().d("[DSDModemEndPointModule]: processUiInfoInd ENTRY");
  if (ind_data->global_ui_info_valid) {
    if (0 != (QMI_DSD_UI_MASK_3GPP_5G_UWB_V01 & ind_data->global_ui_info.ui_mask)) {
      auto msg = std::make_shared<rildata::DataNrIconTypeIndMessage>(rildata::NrIconEnum_t::UWB);
      Log::getInstance().d("[DSDModemEndPointModule]: DataNrIconType UWB");
      msg->broadcast();
    } else if (0 != (QMI_DSD_UI_MASK_3GPP_5G_BASIC_V01 & ind_data->global_ui_info.ui_mask)) {
      auto msg = std::make_shared<rildata::DataNrIconTypeIndMessage>(rildata::NrIconEnum_t::BASIC);
      Log::getInstance().d("[DSDModemEndPointModule]: DataNrIconType BASIC");
      msg->broadcast();
    } else {
      auto msg = std::make_shared<rildata::DataNrIconTypeIndMessage>(rildata::NrIconEnum_t::NONE);
      Log::getInstance().d("[DSDModemEndPointModule]: DataNrIconType NONE");
      msg->broadcast();
    }
  } else {
    Log::getInstance().d("[DSDModemEndPointModule]: DataNrIconType TLV not valid");
  }
}

boolean DSDModemEndPointModule::isApnTypeFound
(
  const RIL_ApnTypes &apn_type, const uint8_t &apn_types_mask
)
{
  if ((apn_type & apn_types_mask) == apn_type)
  {
    Log::getInstance().d("apn type %x was enabled" + std::to_string(apn_type));
    return TRUE;
  }
  return FALSE;
}

void DSDModemEndPointModule::handleSetApnInfoSync(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: handleSetApnInfoSync : Handling msg = " + m->dump());
  auto msg= std::static_pointer_cast<SetApnInfoSyncMessage>(m);
  if (msg != NULL)
  {
    uint8_t ril_apn_types_mask = 0;
    std::string apn_name("");
    msg->getParams(apn_name, ril_apn_types_mask);
    Log::getInstance().d("[DSDModemEndPointModule]: apn_name= " +apn_name+"apn_types= %d" +std::to_string(ril_apn_types_mask));

    do
    {
      if (ril_apn_types_mask <= 0)
      {
        Log::getInstance().d("Invalid inputs");
        msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
      }

      if (isApnTypeFound(RIL_APN_TYPE_ALL, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_DEFAULT_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_IMS_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_MMS_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_DUN_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_SUPL_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_HIPRI_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_FOTA_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_CBS_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_IA_V01);
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_EMERGENCY_V01);
        break;
      }
      if (isApnTypeFound(RIL_APN_TYPE_DEFAULT, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_DEFAULT_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_IMS, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_IMS_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_MMS, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_MMS_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_DUN, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_DUN_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_SUPL, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_SUPL_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_HIPRI, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_HIPRI_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_FOTA, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_FOTA_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_CBS, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_CBS_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_IA, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_IA_V01);
      }
      if (isApnTypeFound(RIL_APN_TYPE_EMERGENCY, ril_apn_types_mask))
      {
        sendApnInfoQmi(apn_name, DSD_APN_TYPE_EMERGENCY_V01);
      }
    } while(0);
    msg->sendResponse(msg, Message::Callback::Status::SUCCESS,nullptr);
  } else
  {
    Log::getInstance().d("Msg received is NULL");
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }
}

void DSDModemEndPointModule::handleTriggerDDSSwitchSyncMessage(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: handleTriggerDDSSwitchSyncMessage : Handling msg = " + m->dump());
  auto msg= std::static_pointer_cast<TriggerDDSSwitchSyncMessage>(m);
  if (msg != NULL)
  {
    dsd_switch_dds_resp_msg_v01 respMsg = qmiDDSSwitchRequest(msg->getSubId(), msg->getSwitchType());
    SetPreferredDataModemResult_t resp;
    switch (respMsg.resp.error) {
      case QMI_ERR_NONE_V01:
        resp = SetPreferredDataModemResult_t::NO_ERROR;
        break;
      case QMI_ERR_INVALID_ARG_V01:
        resp = SetPreferredDataModemResult_t::INVALID_ARG;
        break;
      case QMI_ERR_MISSING_ARG_V01:
        resp = SetPreferredDataModemResult_t::INVALID_ARG;
        break;
      case QMI_ERR_INVALID_OPERATION_V01:
        resp = SetPreferredDataModemResult_t::INVALID_OPERATION;
        break;
      default:
        resp = SetPreferredDataModemResult_t::QMI_ERROR;
        break;
    }
    if (resp == SetPreferredDataModemResult_t::NO_ERROR) {
      if (ddsSwitchRequestPending) {
        TimeKeeper::getInstance().clear_timer(ddsSwitchRequestTimer);
      }
      ddsSwitchRequestPending = true;
      ddsSwitchRequestTimer = TimeKeeper::getInstance().set_timer(
          std::bind(&DSDModemEndPointModule::onDDSSwitchRequestExpired, this, std::placeholders::_1),
          nullptr,
          DDS_SWITCH_REQUEST_TIMEOUT);
      Log::getInstance().d("Started timer for dds switch. Return Success");
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, std::make_shared<SetPreferredDataModemResult_t>(resp));
    } else {
      Log::getInstance().d("Failed to switch dds");
      msg->sendResponse(msg, Message::Callback::Status::FAILURE, std::make_shared<SetPreferredDataModemResult_t>(resp));
    }
  } else
  {
    Log::getInstance().d("Msg received is NULL");
  }
}

void DSDModemEndPointModule::onDDSSwitchRequestExpired(void *)
{
  Log::getInstance().d("DSDModemEndPointModule::onDDSSwitchRequestExpired ENTRY");
  DDSTimeOutSwitchType type = DDSTimeOutSwitchType::DDSTimeOutSwitch;
  auto msg = std::make_shared<DDSSwitchTimeoutMessage>(type);
  msg->broadcast();
}

void DSDModemEndPointModule::sendApnInfoQmi
(
  const std::string &apn_name,
  dsd_apn_type_enum_v01 apn_type
)
{
  Log::getInstance().d("DSDModemEndPointModule::sendApnInfoQmi ENTRY");
  if (mQmiSvcClient != nullptr)
  {
    dsd_set_apn_info_req_msg_v01 data_setting_set_apn_param;
    dsd_set_apn_info_resp_msg_v01 data_setting_set_apn_param_resp;

    memset(&data_setting_set_apn_param, 0, sizeof(data_setting_set_apn_param));
    memset(&data_setting_set_apn_param_resp, 0,
      sizeof(dsd_set_apn_info_resp_msg_v01));
    strlcpy( &data_setting_set_apn_param.apn_info.apn_name[0],
             apn_name.c_str(), QMI_DSD_MAX_APN_LEN_V01);

    // indicate that the apn_invalid flag is set
    data_setting_set_apn_param.apn_invalid_valid = FALSE;
    data_setting_set_apn_param.apn_info.apn_type = apn_type;

    int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                   QMI_DSD_SET_APN_INFO_REQ_V01,
                                   (void *)&data_setting_set_apn_param,
                                   sizeof(data_setting_set_apn_param),
                                   (void *)&data_setting_set_apn_param_resp,
                                   sizeof(dsd_set_apn_info_resp_msg_v01),
                                   QCRIL_DATA_QMI_TIMEOUT);
    if (rc != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPointModule] Failed to send"
        "QMI_DSD_SET_APN_INFO_REQ_V01 for apn_name:"+apn_name+
        ", type:"+std::to_string(apn_type)+"with rc ="+std::to_string(rc));
    } else
    {
      Log::getInstance().d("[DSDModemEndPointModule] sendApnInfoQmi::Successfully sent"
        "QMI_DSD_SET_APN_INFO_REQ_V01 for type ="+ std::to_string(apn_type));
    }
  }
}

void DSDModemEndPointModule::handleTurnOnAPAssistIWLANSync(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: handleTurnOnAPAssistIWLANSync : Handling msg = " + m->dump());

  auto msg= std::static_pointer_cast<TurnOnAPAssistIWLANSyncMessage>(m);

  if (mQmiSvcClient != nullptr)
  {
    do
    {
        dsd_set_capability_req_msg_v01 cap_req;
        dsd_set_capability_resp_msg_v01 cap_resp;

        memset(&cap_req, 0, sizeof(cap_req));
        memset(&cap_resp, 0, sizeof(cap_resp));

        //Send AP capability
        cap_req.ap_asst_mode_valid = true;
        cap_req.ap_asst_mode = DSD_AP_ASST_APN_PREF_SYS_ON_V01;

        int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                     QMI_DSD_SET_CAPABILITY_REQ_V01,
                                     (void *)&cap_req,
                                     sizeof(cap_req),
                                     (void *)&cap_resp,
                                     sizeof(cap_resp),
                                     QCRIL_DATA_QMI_TIMEOUT);
        if ((rc != QMI_NO_ERR) || (cap_resp.resp.result == QMI_RESULT_FAILURE_V01 )){
          Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_SET_CAPABILITY_REQ_V01");
          break;
        } else
        {
          Log::getInstance().d("[DSDModemEndPointModule] QMI_DSD_SET_CAPABILITY_REQ_V01::Successfully sent");
        }

        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
        return;
    }
    while (0);
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }

}

void DSDModemEndPointModule::handleRegisterForSystemStatusSync(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: handleRegisterForSystemStatusSync : Handling msg = " + m->dump());

  auto msg= std::static_pointer_cast<RegisterForSystemStatusSyncMessage>(m);

  if (mQmiSvcClient != nullptr)
  {
    do
    {
        dsd_get_system_status_resp_msg_v01  sys_resp_msg;
        dsd_system_status_ind_msg_v01 *ind_data = NULL;

        dsd_system_status_change_req_msg_v01 system_status_change_req;
        dsd_system_status_change_resp_msg_v01 system_status_change_resp;

        memset(&sys_resp_msg, 0, sizeof(sys_resp_msg));
        memset(&system_status_change_req, 0, sizeof(system_status_change_req));
        memset(&system_status_change_resp, 0, sizeof(system_status_change_resp));

        /* Query the current system status from QMI-DSD service */

        int rc = qmi_client_send_msg_sync( mQmiSvcClient,
                                     QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                     NULL,
                                     0,
                                     &sys_resp_msg,
                                     sizeof(sys_resp_msg),
                                     QCRIL_DATA_QMI_TIMEOUT);

        if (QMI_NO_ERR != rc) {
          QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d",rc);
          break;
        }
        else if (QMI_NO_ERR != sys_resp_msg.resp.result) {
          QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d",
                                     sys_resp_msg.resp.error);
          break;
        }

        /* Process the system status response */
        ind_data = (dsd_system_status_ind_msg_v01 *)((char *)&sys_resp_msg +
                                                  offsetof(dsd_get_system_status_resp_msg_v01,
                                                  avail_sys_valid));

        auto indMsg = std::make_shared<rildata::DsdSystemStatusMessage>(*ind_data);
        indMsg->broadcast();

        system_status_change_req.report_null_bearer_reason_valid = true;
        system_status_change_req.report_null_bearer_reason = true;
        rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                     QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                                     (void *)&system_status_change_req,
                                     sizeof(system_status_change_req),
                                     (void *)&system_status_change_resp,
                                     sizeof(system_status_change_resp),
                                     QCRIL_DATA_QMI_TIMEOUT);
        if ((rc != QMI_NO_ERR) || (system_status_change_resp.resp.result == QMI_RESULT_FAILURE_V01 )) {
          Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01");
          break;
        } else
        {
          Log::getInstance().d("[DSDModemEndPointModule] QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01::Successfully sent");
        }

        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
        return;
    }
    while (0);
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }

}

void DSDModemEndPointModule::handleRegisterForAPAsstIWlanIndsSync(std::shared_ptr<Message> m)
{
  Log::getInstance().d("[DSDModemEndPointModule]: handleRegisterForAPAsstIWlanIndsSysSync : Handling msg = " + m->dump());

  auto msg= std::static_pointer_cast<RegisterForAPAsstIWlanIndsSyncMessage>(m);

  if (mQmiSvcClient != nullptr)
  {
    do
    {
        dsd_indication_register_req_msg_v01 ind_req;
        dsd_indication_register_resp_msg_v01 ind_resp;

        memset(&ind_req, 0, sizeof(ind_req));
        memset(&ind_resp, 0, sizeof(ind_resp));

        ind_req.report_intent_to_change_pref_sys_valid = true;
        ind_req.report_intent_to_change_pref_sys = msg->getParams();

        ind_req.report_ap_asst_apn_pref_sys_result_valid = true;
        ind_req.report_ap_asst_apn_pref_sys_result = msg->getParams();

        int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                     QMI_DSD_INDICATION_REGISTER_REQ_V01,
                                     (void *)&ind_req,
                                     sizeof(ind_req),
                                     (void *)&ind_resp,
                                     sizeof(ind_resp),
                                     QCRIL_DATA_QMI_TIMEOUT);

        if ((rc != QMI_NO_ERR) || (ind_resp.resp.result == QMI_RESULT_FAILURE_V01 )) {
          Log::getInstance().d("[DSDModemEndPointModule] Failed to send QMI_DSD_INDICATION_REGISTER_REQ_V01");
          break;
        } else
        {
          Log::getInstance().d("[DSDModemEndPointModule] QMI_DSD_INDICATION_REGISTER_REQ_V01::Successfully sent");
        }
        msg->sendResponse(msg, Message::Callback::Status::SUCCESS, nullptr);
        return;
    }
    while (0);
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  }

}

dsd_switch_dds_resp_msg_v01 DSDModemEndPointModule::qmiDDSSwitchRequest(int subId, dsd_dds_switch_type_enum_v01 switch_type)
{
  Log::getInstance().d("DSDModemEndPointModule::qmiDDSSwitchRequest ENTRY");

  dsd_switch_dds_req_msg_v01 reqMsg;
  dsd_switch_dds_resp_msg_v01 respMsg;

  memset(&reqMsg, 0, sizeof(reqMsg));
  memset(&respMsg, 0, sizeof(respMsg));

  switch (subId) {
    case 0:
      reqMsg.subscription = DSD_PRIMARY_SUBS_V01;
      break;
    case 1:
      reqMsg.subscription = DSD_SECONDARY_SUBS_V01;
      break;
    default:
      respMsg.resp.error = QMI_ERR_INVALID_ARG_V01;
      return respMsg;
  }

  reqMsg.dds_switch_type_valid = true;
  reqMsg.dds_switch_type = switch_type;

  if (mQmiSvcClient != nullptr)
  {

    int rc = qmi_client_send_msg_sync(mQmiSvcClient,
                                      QMI_DSD_SWITCH_DDS_REQ_V01,
                                      (void *)&reqMsg,
                                      sizeof(reqMsg),
                                      (void *)&respMsg,
                                      sizeof(respMsg),
                                      QCRIL_DATA_QMI_TIMEOUT);
    if (rc != QMI_NO_ERR) {
      Log::getInstance().d("[DSDModemEndPointModule] Failed to send"
        "QMI_DSD_SWITCH_DDS_REQ_V01 with rc =" + std::to_string(rc));
    } else
    {
      Log::getInstance().d("[DSDModemEndPointModule] qmiDDSSwitchRequest::Successfully sent"
        "QMI_DSD_SWITCH_DDS_REQ_V01");
    }
  }
  return respMsg;
}
