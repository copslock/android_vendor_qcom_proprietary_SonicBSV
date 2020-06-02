/*===========================================================================

  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#pragma once
#include "MessageCommon.h"

#define REQUEST_SOURCE_INCLUDED

using namespace std;
namespace rildata {

enum class RequestSource_t {
  RADIO = 0,
  IWLAN = 1,
};

struct SetupDataCallResponse_t {
  ResponseError_t respErr;
  DataCallResult_t call;
};

class SetupDataCallRequestMessage : public SolicitedMessage<SetupDataCallResponse_t>,
                          public add_message_id<SetupDataCallRequestMessage> {
private:
  int32_t mSerial;
  RequestSource_t mRequestSource;
  AccessNetwork_t mAccessNetwork;
  DataProfileId_t mProfileId;
  string mApn;
  string mProtocol;
  string mRoamingProtocol;
  ApnAuthType_t mAuthType;
  string mUsername;
  string mPassword;
  DataProfileInfoType_t mDataProfileInfoType;
  int32_t mMaxConnsTime;
  int32_t mMaxConns;
  int32_t mWaitTime;
  bool mEnableProfile;
  ApnTypes_t mSupportedApnTypesBitmap;
  RadioAccessFamily_t mBearerBitmap;
  int32_t mMtu;
  bool mPreferred;
  bool mPersistent;
  bool mRoamingAllowed;
  DataRequestReason_t mReason;
  vector<string> mAddresses;
  vector<string> mDnses;
  shared_ptr<function<void(uint16_t)>> mAcknowlegeRequestCb;

public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.data.SetupDataCallRequestMessage";
  SetupDataCallRequestMessage() = delete;
  SetupDataCallRequestMessage(
    const int32_t serial,
    const RequestSource_t requestSource,
    const AccessNetwork_t accessNetwork,
    const DataProfileId_t profileId,
    const string apn,
    const string protocol,
    const string roamingProtocol,
    const ApnAuthType_t authType,
    const string username,
    const string password,
    const DataProfileInfoType_t dataProfileInfoType,
    const int32_t maxConnsTime,
    const int32_t maxConns,
    const int32_t waitTime,
    const bool enableProfile,
    const ApnTypes_t supportedApnTypesBitmap,
    const RadioAccessFamily_t bearerBitmap,
    const int32_t mtu,
    const bool preferred,
    const bool persistent,
    const bool roamingAllowed,
    const DataRequestReason_t reason,
    const vector<string> addresses,
    const vector<string> dnses,
    const shared_ptr<function<void(uint16_t)>> ackCb
    ):SolicitedMessage<SetupDataCallResponse_t>(get_class_message_id()) {
    mName = MESSAGE_NAME;
    mSerial = serial;
    mRequestSource = requestSource;
    mAccessNetwork = accessNetwork;
    mProfileId = profileId;
    mApn = apn;
    mProtocol = protocol;
    mRoamingProtocol = roamingProtocol;
    mAuthType = authType;
    mUsername = username;
    mPassword = password;
    mDataProfileInfoType = dataProfileInfoType;
    mMaxConnsTime = maxConnsTime;
    mMaxConns = maxConns;
    mWaitTime = waitTime;
    mEnableProfile = enableProfile;
    mSupportedApnTypesBitmap = supportedApnTypesBitmap;
    mBearerBitmap = bearerBitmap;
    mMtu = mtu;
    mPreferred = preferred;
    mPersistent = persistent;
    mRoamingAllowed = roamingAllowed;
    mReason = reason;
    mAddresses = addresses;
    mDnses = dnses;
    mAcknowlegeRequestCb = ackCb;
  }
  ~SetupDataCallRequestMessage() = default;

  string dump(){return mName;}
  int32_t getSerial() {return mSerial;}
  RequestSource_t getRequestSource() {return mRequestSource;}
  AccessNetwork_t getAccessNetwork() {return mAccessNetwork;}
  DataProfileId_t getProfileId() {return mProfileId;}
  string getApn() {return mApn;}
  string getProtocol() {return mProtocol;}
  string getRoamingProtocol() {return mRoamingProtocol;}
  ApnAuthType_t getAuthType() {return mAuthType;}
  string getUsername() {return mUsername;}
  string getPassword() {return mPassword;}
  DataProfileInfoType_t getDataProfileInfoType() {return mDataProfileInfoType;}
  int32_t getMaxConnsTime() {return mMaxConnsTime;}
  int32_t getMaxConns() {return mMaxConns;}
  int32_t getWaitTime() {return mWaitTime;}
  bool getEnableProfile() {return mEnableProfile;}
  ApnTypes_t getSupportedApnTypesBitmap() {return mSupportedApnTypesBitmap;}
  RadioAccessFamily_t getBearerBitmap() {return mBearerBitmap;}
  int32_t getMtu() {return mMtu;}
  bool getPreferred() {return mPreferred;}
  bool getPersistent() {return mPersistent;}
  bool getRoamingAllowed() {return mRoamingAllowed;}
  DataRequestReason_t getDataRequestReason() {return mReason;}
  vector<string> getAddresses() {return mAddresses;}
  vector<string> getDnses() {return mDnses;}
  shared_ptr<function<void(uint16_t)>> getAcknowlegeRequestCb() {return mAcknowlegeRequestCb;}
};

}
