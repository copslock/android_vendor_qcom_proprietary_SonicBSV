#pragma once
/******************************************************************************
#  Copyright (c) 2018-19 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <framework/Log.h>
#include "qdp_adapter.h"
#include "MessageCommon.h"
#include "BaseProfile.h"

#define QDP_PROFILE_ID_INVALID -1

#define DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(t) ((t == RadioAccessFamily_t::ONE_X_RTT)|| \
                                                (t == RadioAccessFamily_t::IS95A) || \
                                                (t == RadioAccessFamily_t::EVDO_B) || \
                                                (t == RadioAccessFamily_t::EVDO_A) || \
                                                (t == RadioAccessFamily_t::EVDO_0) )

/**
 * @brief      Profile for Call Setup procedure.
 */
class CallSetupProfile : BaseProfile {
public:
   CallSetupProfile(params_t p): BaseProfile(p),
                                 umtsProfileId(QDP_PROFILE_ID_INVALID),
                                 cdmaProfileId(QDP_PROFILE_ID_INVALID),
                                 isEmergencyCall(false), emergencyCallApn("") {
    Log::getInstance().d("[CallSetupProfile]: CallSetupProfile");
   }
   ~CallSetupProfile(){
    Log::getInstance().d("[CallSetupProfile]: ~CallSetupProfile");
   };

   virtual void dump(std::string padding, std::ostream& os);

   /**
    * @brief      Look up params to see whether it matches any profile.
    *             Most parameters are matched strictly.
    *             IP addresses may have loose matching.
    *
    *
    * @return     true if successful, false otherwise
    */
   bool lookup();
   /**
    * @brief      Check for any available emergency profile on the modem.
    *             If there are multiple, pick one matching the apn name.
    *             If there are none, create one
    *
    *
    * @return     true if successful, false otherwise
    */
   bool emergencyLookup();
   void setProfileMtu(unsigned int mtu) {params.mtu=mtu;};
   unsigned int getProfileMtu() {return params.mtu;};

   void setProtocol(std::string protocol) {params.protocol = protocol;};
   std::string getProtocol() {return params.protocol;};
   void setRoamingProtocol(std::string protocol) {params.roamingProtocol = protocol;};
   std::string getRoamingProtocol() {return params.roamingProtocol;};

   std::string getApn() {return params.apn;};
   std::string getAuthType() {return params.authType;};
   std::string getUsername() {return params.username;};
   std::string getPassword() {return params.password;};
   void setUmtsProfileId(int profileId) {umtsProfileId = profileId;};
   int getUmtsProfileId() {return umtsProfileId;};
   void setCdmaProfileId(int profileId) {cdmaProfileId = profileId;};
   int getCdmaProfileId() {return cdmaProfileId;};
   uint8_t getRadioTech() {return params.radioTech;};
   uint32_t getApnTypes() {return params.apnTypeBitmask;};
   bool getIsEmergencyCall() {return isEmergencyCall;};
   void setIsEmergencyCall(bool flag) {isEmergencyCall = flag;};
   string getEmergencyCallApn() {return emergencyCallApn;};
   void setEmergencyCallApn(string apn) {emergencyCallApn = apn;};
   int32_t getProfileId() { return params.profileId; };
   void setAvailableRadioRAT(rildata::RadioAccessFamily_t radioRAT) { availableRadioRAT = radioRAT; };
   rildata::RadioAccessFamily_t getAvailableRadioRAT() { return availableRadioRAT; };
private:
  int umtsProfileId; //returned by qdp lookup
  int cdmaProfileId; //returned by qdp lookup
  bool isEmergencyCall;
  string emergencyCallApn;
  rildata::RadioAccessFamily_t availableRadioRAT;
};
