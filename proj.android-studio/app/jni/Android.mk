LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/ChatScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/CustomTableViewCell.cpp \
                   $(LOCAL_PATH)/../../../Classes/LoginScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/UserScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/PomeloClient.cpp \
                   $(LOCAL_PATH)/../../../Classes/Utils/CallQueue.cpp \
                   $(LOCAL_PATH)/../../../Classes/VisibleRect.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
                $(LOCAL_PATH)/../../../Classes/3rd/jasson \
                $(LOCAL_PATH)/../../../Classes/Utils

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
