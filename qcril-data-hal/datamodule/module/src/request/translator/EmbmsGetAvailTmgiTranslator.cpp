#if 0
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
//#include <framework/Log.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

#include "request/RilRequestEmbmsGetAvailTmgiMessage.h"
#include "legacy/qcril_event_facade.h"

#include "qcril_qmi_oem_reqlist.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

class EmbmsGetAvailTmgiTranslator : public legacy_translator<QCRIL_REQ_HOOK_EMBMS_UNSOL_AVAILABLE_TMGI> {
public:
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
        qcril_request_params_type params {
            request.instance_id,
            QCRIL_DEFAULT_MODEM_ID, /* Always pass in dfault */
            request.req_id_android,
            request.req_id, /* event_id --> req_id */
            const_cast<void *>(request.payload), /* no deepcopy here, just pass it in to the message for that */
            request.payload_len,
            request.t,
            NULL
        };
        return std::make_shared<rildata::RilRequestEmbmsGetAvailTmgiMessage>(params);

    }
};


static EmbmsGetAvailTmgiTranslator EmbmsGetAvailTmgi_translator;

#if (QCRIL_RIL_VERSION >= 12)
    QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(HOOK_EMBMS_UNSOL_AVAILABLE_TMGI, embms_get_available_tmgi_req_msg_v01,
                                            embms_get_available_tmgi_resp_msg_v01, EmbmsGetAvailTmgi_translator);
    QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(HOOK_EMBMS_UNSOL_AVAILABLE_TMGI);
#else
    /*TODO*/
#endif
#endif
