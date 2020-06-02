/******************************************************************************
#  Copyright (c) 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <sstream>
#include "framework/Log.h"

#include "CallState.h"
#include "CallManager.h"

using namespace std;
using namespace SM;
using namespace rildata;

void DisconnectingState::enter(void) {
  Log::getInstance().d("[DisconnectingState] enter");
}

void DisconnectingState::exit(void) {
  Log::getInstance().d("[DisconnectingState] exit");
}

int DisconnectingState::handleEvent(const CallEventType &event){
  Log::getInstance().d("[DisconnectingState] handleEvent event id = "
    + to_string((int)event.callEvent));
  logBuffer.addLogWithTimestamp("[SM]: " + getName() + " handling " + event.getEventName());

  int nextState = _eDisconnecting;
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

  switch(event.callEvent) {
  case  _eDsiEvent:
    switch(netEvent) {
      case DSI_EVT_NET_NO_NET:
        Log::getInstance().d("[DisconnectingState] disconnected");
        pCallInstance->deactivateDataCallResponse();
        nextState = _eIdle;
        break;

      default:
        Log::getInstance().d("[DisconnectingState] Unhandled DSI net event = "+
                              to_string((int)netEvent));
        break;
    }
    break;

  default:
    Log::getInstance().d("[DisconnectingState] Unhandled call event = "+
                          to_string((int)event.callEvent));
    break;
  }

  return nextState;
}
