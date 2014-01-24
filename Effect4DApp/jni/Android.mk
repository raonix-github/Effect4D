LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp .cc

LOCAL_SRC_FILES := \
	hauto/ttaplayer.cc \
	hauto/ttamotor.cc \
	hauto/ttaio.cc \
	hauto/ttalight.cc \
	hauto/ttalight_dasan.cc \
	hauto/hauto.cc

LOCAL_SRC_FILES += \
	system/system.cc

LOCAL_SRC_FILES += \
	libtta/tta.c \
	libtta/tta_rs485.c \
	libtta/tta_tcp.c \
	helpers.cc \
	thread.cc \
	io/ha210.c \
	ha210_main.cc

LOCAL_C_INCLUDES += \
	$(LOCA_PATH)/hauto \
	$(LOCA_PATH)/libtta \
	$(ANDROID_NDK_ROOT)\source\cxx-stl\stlport\stlport \
	$(JNI_H_INCLUDE)

LOCAL_CFLAGS += -fexceptions

LOCAL_LDLIBS := \
	-llog -lstdc++

LOCAL_MODULE := libha210

include $(BUILD_SHARED_LIBRARY)
