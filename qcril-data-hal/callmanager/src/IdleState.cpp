/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"

using namespace SM;
using namespace rildata;

void IdleState::enter(void) {
  Log::getInstance().d("[IdleState] enter");
}

void IdleState::exit(void) {
  Log::getInstance().d("[IdleState] exit");
}

int IdleState::handleEvent(const CallEventType &event) {
  Log::getInstance().d("IdleState::handleEvent event id = "
    + std::to_string((int)event.callEvent));
  logBuffer.addLogWithTimestamp("[SM]: " + getName() + " handling " + event.getEventName());

  int nextState = _eIdle;
  CallInfo *pCallInstance = nullptr;
  DsiEventDataType *dsiEvent = nullptr;
  dsi_net_evt_t netEvent = DSI_EVT_INVALID;

  if (event.callEvent == _eDsiEvent) {
    dsiEvent = (DsiEventDataType *)event.callEventData;
    netEvent = dsiEvent->netEvent;
    pCallInstance = (CallInfo *)dsiEvent->eventData;
  }
  else {
    pCallInstance = (CallInfo *)event.callEventData;
  }

  switch (event.callEvent) {
    case  _eConnectEvent:
      if (DSI_SUCCESS != dsi_start_data_call(pCallInstance->getDsiHandle())) {
        Log::getInstance().d("[IdleState] dsi_start_data_call failed");
        nextState = _eError;
        break;
      } else {
        nextState = _eConnecting;
        break;
      }
    default:
      Log::getInstance().d("[IdleState] Unhandled call event = " + std::to_string(event.callEvent));
      break;
  }

  return nextState;
}
