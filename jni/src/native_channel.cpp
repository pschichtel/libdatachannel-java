#include "callback.hpp"
#include "jni-c-to-java.h"
#include "util.hpp"
#include <jni-java-to-c.h>
#include <jni.h>
#include <rtc/rtc.h>
#include <cstdint>
#include <cstdlib>

void RTC_API handle_channel_open(const int channelHandle, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onChannelOpen, channelHandle);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetOpenCallback, handle_channel_open)

void RTC_API handle_channel_closed(const int channelHandle, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onChannelClosed, channelHandle);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetClosedCallback, handle_channel_closed)

void RTC_API handle_channel_error(const int channelHandle, const char* error, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onChannelError_cstr, channelHandle, error);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetErrorCallback, handle_channel_error)

void RTC_API handle_channel_message(const int channelHandle, const char* message, const int size, void* ptr) {
    const auto cb = static_cast<jvm_callback*>(ptr);
    JNIEnv* env = get_jni_env();
    if (size < 0) {
        jstring text = env->NewStringUTF(message);
        call_tel_schich_libdatachannel_PeerConnectionListener_onChannelTextMessage(env, cb->instance, channelHandle, text);
        env->DeleteLocalRef(text);
    } else {
        jobject buffer = env->NewDirectByteBuffer((void*) message, size);
        call_tel_schich_libdatachannel_PeerConnectionListener_onChannelBinaryMessage(env, cb->instance, channelHandle, buffer);
        env->DeleteLocalRef(buffer);
    }
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetMessageCallback, handle_channel_message)

void RTC_API handle_channel_buffered_amount_low(const int channelHandle, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onChannelBufferedAmountLow, channelHandle);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetBufferedAmountLowCallback, handle_channel_buffered_amount_low)

void RTC_API handle_channel_available(const int channelHandle, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onChannelAvailable, channelHandle);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetAvailableCallback, handle_channel_available)

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetMaxDataChannelStream(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return rtcGetMaxDataChannelStream(peerHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteMaxMessageSize(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return rtcGetRemoteMaxMessageSize(peerHandle);
}

JNIEXPORT jint JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcCreateDataChannelEx(JNIEnv* env, jclass clazz, const jint peerHandle,
                                                                           jstring label, const jboolean unordered,
                                                                           const jboolean unreliable, const jlong maxPacketLifeTime,
                                                                           const jint maxRetransmits, jstring protocol,
                                                                           const jboolean negotiated, const jint stream,
                                                                           const jboolean manualStream) {
    rtcDataChannelInit init = {
            .reliability = {
                    .unordered = static_cast<bool>(unordered),
                    .unreliable = static_cast<bool>(unreliable),
                    .maxPacketLifeTime = static_cast<unsigned int>(maxPacketLifeTime),
                    .maxRetransmits = static_cast<unsigned int>(maxRetransmits),
            },
            .protocol = nullptr,
            .negotiated = static_cast<bool>(negotiated),
            .manualStream = static_cast<bool>(manualStream),
            .stream = static_cast<uint16_t>(stream),
    };
    const char* c_label = nullptr;
    if (label != nullptr) {
        c_label = env->GetStringUTFChars(label, nullptr);
        if (c_label == nullptr) {
            THROW_FAILED_GET_STR(env, label);
            return EXCEPTION_THROWN;
        }
    }
    if (protocol != nullptr) {
        init.protocol = env->GetStringUTFChars(protocol, nullptr);
        if (init.protocol == nullptr) {
            if (c_label != nullptr) {
                env->ReleaseStringUTFChars(label, c_label);
            }
            THROW_FAILED_GET_STR(env, protocol);
            return EXCEPTION_THROWN;
        }
    }

    const jint result = rtcCreateDataChannelEx(peerHandle, c_label, &init);

    if (c_label != nullptr) {
        env->ReleaseStringUTFChars(label, c_label);
    }
    if (init.protocol != nullptr) {
        env->ReleaseStringUTFChars(protocol, init.protocol);
    }

    if (result >= 0) {
        rtcSetUserPointer(result, rtcGetUserPointer(peerHandle));
    }

    return result;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcClose(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return rtcClose(channelHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcDeleteDataChannel(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return rtcDeleteDataChannel(channelHandle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcIsClosed(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return rtcIsClosed(channelHandle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcIsOpen(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return rtcIsOpen(channelHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcMaxMessageSize(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return rtcMaxMessageSize(channelHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetBufferedAmountLowThreshold(JNIEnv* env, jclass clazz, const jint channelHandle, const jint amount) {
    return rtcSetBufferedAmountLowThreshold(channelHandle, amount);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSendMessage(JNIEnv* env, jclass clazz, const jint channelHandle, jobject data, const jint offset, const jint length) {
    if (data == nullptr) {
        return RTC_ERR_SUCCESS;
    }
    char* buffer = static_cast<char*>(env->GetDirectBufferAddress(data));
    if (buffer == nullptr) {
        return RTC_ERR_SUCCESS;
    }
    char* buffer_offset = buffer + offset;
    return rtcSendMessage(channelHandle, buffer_offset, length);
}

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcReceiveMessage(JNIEnv* env, jclass clazz, const jint channelHandle) {
    int size = 0;
    WRAP_ERROR(env, rtcReceiveMessage(channelHandle, nullptr, &size));
    if (size == 0) {
        return nullptr;
    }
    char* buffer = static_cast<char*>(malloc(size));
    if (buffer == nullptr) {
        throw_native_exception(env, "Failed to allocate memory for message");
        return nullptr;
    }
    int result = rtcReceiveMessage(channelHandle, buffer, &size);
    if (result == RTC_ERR_NOT_AVAIL) {
        free(buffer);
        return nullptr;
    }
    if (result < 0) {
        free(buffer);
        WRAP_ERROR(env, result);
        return nullptr;
    }
    WRAP_ERROR(env, result);

    jobject byteBuffer = env->NewDirectByteBuffer(buffer, size);
    if (byteBuffer == nullptr) {
        free(buffer);
        throw_native_exception(env, "Failed to create direct byte buffer");
        return nullptr;
    }
    // ensure the buffer cleanup is managed
    call_tel_schich_libdatachannel_LibDataChannel_freeOnGarbageCollection(env, byteBuffer, (jlong) (intptr_t) buffer);
    return byteBuffer;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcReceiveMessageInto(JNIEnv* env, jclass clazz, const jint channelHandle, jobject buffer, const jint offset, const jint capacity) {
    int size = capacity;
    if (buffer == nullptr) {
        return 0;
    }
    const auto base = static_cast<char*>(env->GetDirectBufferAddress(buffer));
    if (base == nullptr) {
        return 0;
    }

    char* data = base + offset;

    const int result = rtcReceiveMessage(channelHandle, data, &size);
    if (result == RTC_ERR_NOT_AVAIL) {
        return 0;
    }
    if (result == RTC_ERR_TOO_SMALL) {
        return -size;
    }
    WRAP_ERROR(env, result);
    return size;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetAvailableAmount(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return WRAP_ERROR(env, rtcGetAvailableAmount(channelHandle));
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetBufferedAmount(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return WRAP_ERROR(env, rtcGetBufferedAmount(channelHandle));
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelStream(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return WRAP_ERROR(env, rtcGetDataChannelStream(channelHandle));
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelLabel(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetDataChannelLabel, channelHandle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelProtocol(JNIEnv* env, jclass clazz, const jint channelHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetDataChannelProtocol, channelHandle);
}

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelReliability(JNIEnv* env, jclass clazz, const jint channelHandle) {
    rtcReliability reliability;
    WRAP_ERROR(env, rtcGetDataChannelReliability(channelHandle, &reliability));
    return create_tel_schich_libdatachannel_DataChannelReliability(env, reliability.unordered, reliability.unreliable, reliability.maxPacketLifeTime, (jint) reliability.maxRetransmits);
}