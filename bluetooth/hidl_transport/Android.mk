LOCAL_DIR_PATH:= $(call my-dir)
ifeq ($(BOARD_HAVE_BLUETOOTH_QCOM),true)

ifeq ($(TARGET_USE_QTI_BT_CONFIGSTORE),true)
LOCAL_PATH := $(LOCAL_DIR_PATH)
include $(LOCAL_PATH)/btconfigstore/1.0/default/Android.mk
endif # TARGET_USE_QTI_BT_CONFIGSTORE

endif # BOARD_HAVE_BLUETOOTH_QCOM
