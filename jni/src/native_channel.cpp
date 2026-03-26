#include "callback.hpp"
#include "jni-c-to-java.h"
#include "rtc/datachannel.hpp"
#include "util.hpp"

#include <cstdint>
#include <jni-java-to-c.h>
#include <jni.h>
#include <rtc/rtc.h>


JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetOpenCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::DataChannel, void()>(
        env,
        channelHandle,
        listener,
        set,
        [](auto dc, auto cb) { dc->onOpen(std::move(cb)); },
        [channelHandle](JNIEnv* local_env, jobject listener) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onChannelOpen(local_env, listener, channelHandle);
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetClosedCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::DataChannel, void()>(
        env,
        channelHandle,
        listener,
        set,
        [](auto dc, auto cb) { dc->onClosed(std::move(cb)); },
        [channelHandle](JNIEnv* local_env, jobject listener) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onChannelClosed(local_env, listener, channelHandle);
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetErrorCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::DataChannel, void(std::string error)>(
        env,
        channelHandle,
        listener,
        set,
        [](auto dc, auto cb) { dc->onError(std::move(cb)); },
        [channelHandle](JNIEnv* local_env, jobject listener, const std::string& error) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onChannelError_cstr(local_env, listener, channelHandle, error.c_str());
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetMessageCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::DataChannel, void(rtc::message_variant data)>(
        env,
        channelHandle,
        listener,
        set,
        [](auto dc, auto cb) { dc->onMessage(std::move(cb)); },
        [channelHandle](JNIEnv* local_env, jobject listener, rtc::message_variant message) {
            std::visit(rtc::overloaded {
                [&local_env, &listener, &channelHandle](const std::string& s) {
                    call_tel_schich_libdatachannel_PeerConnectionListener_onChannelTextMessage_cstr(local_env, listener, channelHandle, s.c_str());
                },
                [&local_env, &listener, &channelHandle](const rtc::binary& b) {
                    // TODO copy and pass arond as std::shared_ptr
                    const void* address = std::data(b);
                    jobject buffer = local_env->NewDirectByteBuffer(const_cast<void*>(address), b.size());
                    call_tel_schich_libdatachannel_PeerConnectionListener_onChannelBinaryMessage(local_env, listener, channelHandle, buffer);
                    local_env->DeleteLocalRef(buffer);
                }
            }, message);
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetBufferedAmountLowCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::DataChannel, void()>(
        env,
        channelHandle,
        listener,
        set,
        [](auto dc, auto cb) { dc->onBufferedAmountLow(std::move(cb)); },
        [channelHandle](JNIEnv* local_env, jobject listener) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onChannelBufferedAmountLow(local_env, listener, channelHandle);
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetAvailableCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::DataChannel, void()>(
        env,
        channelHandle,
        listener,
        set,
        [](auto dc, auto cb) { dc->onAvailable(std::move(cb)); },
        [channelHandle](JNIEnv* local_env, jobject listener) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onChannelAvailable(local_env, listener, channelHandle);
        }
    );
}

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

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcClose(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    return rtcClose(channelHandle);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcDeleteDataChannel(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    util::delete_handle<rtc::DataChannel>(env, channelHandle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcIsClosed(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    return rtcIsClosed(channelHandle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcIsOpen(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    return rtcIsOpen(channelHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcMaxMessageSize(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    return rtcMaxMessageSize(channelHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetBufferedAmountLowThreshold(JNIEnv* env, jclass clazz, const jlong channelHandle, const jint amount) {
    return rtcSetBufferedAmountLowThreshold(channelHandle, amount);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSendMessage(JNIEnv* env, jclass clazz, const jlong channelHandle, jobject data, const jint offset, const jint length) {
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

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcReceiveMessage(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    const auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    auto message = channel->receive();
    if (!message) {
        throw_tel_schich_libdatachannel_exception_NotAvailableException_cstr(env, "No message available!");
        return nullptr;
    }

    std::visit(
        rtc::overloaded{
            [&](rtc::binary b) {
                // TODO implement
            },
            [&](std::string s) {
                // TODO implement
            },
        },
        *message
    );

    return nullptr;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcReceiveMessageInto(JNIEnv* env, jclass clazz, const jlong channelHandle, jobject buffer, const jint offset, const jint capacity) {
                // TODO implement
    return 0;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetAvailableAmount(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    return channel->availableAmount();
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetBufferedAmount(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    return channel->bufferedAmount();
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelStream(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    const auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    if (const auto stream = channel->stream()) {
        return *stream;
    }
    throw_tel_schich_libdatachannel_exception_NotAvailableException_cstr(env, "Stream not available!");
    return EXCEPTION_THROWN;
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelLabel(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    return util::get_string_for_java(env, [&channel] {
        return channel->label();
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelProtocol(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    return util::get_string_for_java(env, [&channel] {
        return channel->protocol();
    });
}

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelReliability(JNIEnv* env, jclass clazz, const jlong channelHandle) {

    auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    auto reliability = channel->reliability();

    return create_tel_schich_libdatachannel_DataChannelReliability(env, reliability.unordered, reliability.unreliable, reliability.maxPacketLifeTime, (jint) reliability.maxRetransmits);
}