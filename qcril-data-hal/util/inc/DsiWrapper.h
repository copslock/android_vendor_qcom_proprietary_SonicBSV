/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#pragma once

#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "dsi_netctrl.h"
#include <framework/Log.h>
#include "legacy/qcril_data.h"

#define DS_CALL_INFO_ADDR_PCSCF_MAX_LEN (200)

#define AddressFormatIPv4WithSubnetMask(data,mask,str,len)    \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d/%d%c",                             \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        mask, 0 );

#define AddressFormatIPv6WithPrefixLength(data,mask,str,len) \
        (void)snprintf( str, len,                                            \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x:"       \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x/%d%c",  \
                        ((char*)data)[ 0],((char*)data)[ 1],                 \
                        ((char*)data)[ 2],((char*)data)[ 3],                 \
                        ((char*)data)[ 4],((char*)data)[ 5],                 \
                        ((char*)data)[ 6],((char*)data)[ 7],                 \
                        ((char*)data)[ 8],((char*)data)[ 9],                 \
                        ((char*)data)[10],((char*)data)[11],                 \
                        ((char*)data)[12],((char*)data)[13],                 \
                        ((char*)data)[14],((char*)data)[15], mask, 0 );

#define AddressFormatIPv4(data,str,len)                          \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d%c",                                \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3], 0 );

#define AddressFormatIPv6(data,str,len)                          \
        (void)snprintf( str, len,                                       \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x:"  \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x%c",\
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        ((char*)data)[ 4],((char*)data)[ 5],            \
                        ((char*)data)[ 6],((char*)data)[ 7],            \
                        ((char*)data)[ 8],((char*)data)[ 9],            \
                        ((char*)data)[10],((char*)data)[11],            \
                        ((char*)data)[12],((char*)data)[13],            \
                        ((char*)data)[14],((char*)data)[15], 0 );

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param dsiHandle [description]
 */
void dsiStopDataCall(dsi_hndl_t dsiHandle);

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param DsiHandle [description]
 * @param deviceName [description]
 *
 * @return [description]
 */
int dsiGetDeviceName(dsi_hndl_t DsiHandle, std::string &deviceName);

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param DsiHandle [description]
 * @param ipAddresses [description]
 * @param gatewayAddresses [description]
 * @param dnsAddresses [description]
 * @return [description]
 */
unsigned int dsiGetAddresses(dsi_hndl_t DsiHandle, bool &v4Connected, bool &v6Connected,
    std::string &ipAddresses, std::string &gatewayAddresses, std::string &dnsAddresses);

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param DsiHandle [description]
 * @return [description]
 */
unsigned int dsiGetMtu(dsi_hndl_t DsiHandle);

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param DsiHandle [description]
 * @param pcscfAddresses [description]
 *
 * @return [description]
 */
int dsiGetPcscfAddresses(dsi_hndl_t DsiHandle, std::string &pcscfAddresses);


/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param DsiHandle [description]
 * @return [description]
 */
int dsiGetCallEndReason(dsi_hndl_t DsiHandle);

dsi_ce_reason_t dsiGetVerboseCallEndReason(dsi_hndl_t DsiHandle);

bool dsiGetDualIpCallEndReason(dsi_hndl_t DsiHandle, dsi_ce_reason_t &v4Reason,
    dsi_ce_reason_t &v6Reason);
