#include "jni-c-to-java.h"
#include "native_channel.hpp"
#include "rtc/datachannel.hpp"
#include "rtc/peerconnection.hpp"
#include "util.hpp"

#include <jni-java-to-c.h>
#include <jni.h>

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetOpenCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    channel::set_open_callback<rtc::DataChannel>(env, channelHandle, listener, set);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetClosedCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    channel::set_closed_callback<rtc::DataChannel>(env, channelHandle, listener, set);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetErrorCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    channel::set_error_callback<rtc::DataChannel>(env, channelHandle, listener, set);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetMessageCallback(JNIEnv* env, jclass clazz, jlong channelHandle, jobject listener, jboolean set) {
    util::setup_callback<rtc::DataChannel, void(rtc::message_variant data)>(
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
                    // TODO copy and pass around as std::shared_ptr
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
    util::setup_callback<rtc::DataChannel, void()>(
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
    channel::set_available_callback<rtc::DataChannel>(env, channelHandle, listener, set);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetMaxDataChannelStream(JNIEnv* env, jclass clazz, const jlong peerHandle) {
    const auto peer = util::dehandlize<rtc::PeerConnection>(env, peerHandle);
    return peer->maxDataChannelId();
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteMaxMessageSize(JNIEnv* env, jclass clazz, const jlong peerHandle) {
    const auto peer = util::dehandlize<rtc::PeerConnection>(env, peerHandle);
    return peer->remoteMaxMessageSize();
}

JNIEXPORT jlong JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcCreateDataChannelEx(JNIEnv* env, jclass clazz, const jlong peerHandle,
                                                                           jstring label, const jboolean unordered,
                                                                           const jboolean unreliable, const jlong maxPacketLifeTime,
                                                                           const jint maxRetransmits, jstring protocol,
                                                                           const jboolean negotiated, const jint stream,
                                                                           const jboolean manualStream) {
    rtc::DataChannelInit dci = {
        .negotiated = static_cast<bool>(negotiated),
        .id = static_cast<bool>(manualStream) ? std::make_optional(static_cast<uint16_t>(stream)) : std::nullopt,
        .protocol = util::getJavaString(env, protocol),
    };
    dci.reliability.unordered = static_cast<bool>(unordered);
    if (static_cast<bool>(unreliable)) {
        if (const auto maximumPacketLifeTime = static_cast<unsigned int>(maxPacketLifeTime); maximumPacketLifeTime > 0) {
            dci.reliability.maxPacketLifeTime.emplace(std::chrono::milliseconds(maximumPacketLifeTime));
        } else {
            dci.reliability.maxRetransmits.emplace(static_cast<unsigned int>(maxRetransmits));
        }
    }

    const auto peer = util::dehandlize<rtc::PeerConnection>(env, peerHandle);
    const auto data_channel = peer->createDataChannel(util::getJavaString(env, label), std::move(dci));

    return util::handlize(env, data_channel);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetBufferedAmountLowThreshold(JNIEnv* env, jclass clazz, const jlong channelHandle, const jint amount) {
    const auto channel = util::dehandlize<rtc::Channel>(env, channelHandle);
    channel->setBufferedAmountLowThreshold(amount);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSendMessage(JNIEnv* env, jclass clazz, const jlong channelHandle, jobject data, const jint offset, const jint length) {
    if (data == nullptr) {
        return RTC_ERR_SUCCESS;
    }
    const char* buffer = static_cast<char*>(env->GetDirectBufferAddress(data));
    if (buffer == nullptr) {
        return RTC_ERR_SUCCESS;
    }
    const char* buffer_offset = buffer + offset;
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

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelStream(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    const auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    if (const auto stream = channel->stream()) {
        return *stream;
    }
    throw_tel_schich_libdatachannel_exception_NotAvailableException_cstr(env, "Stream not available!");
    return util::EXCEPTION_THROWN;
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelLabel(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    const auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    return util::get_string_for_java(env, [&channel] {
        return channel->label();
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelProtocol(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    const auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    return util::get_string_for_java(env, [&channel] {
        return channel->protocol();
    });
}

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetDataChannelReliability(JNIEnv* env, jclass clazz, const jlong channelHandle) {
    const auto channel = util::dehandlize<rtc::DataChannel>(env, channelHandle);
    const auto reliability = channel->reliability();
    bool unreliable = false;
    unsigned int maxPacketLifeTime = 0;
    unsigned int maxRetransmits = 0;
    if (reliability.maxPacketLifeTime) {
        unreliable = true;
        maxPacketLifeTime = static_cast<unsigned int>(reliability.maxPacketLifeTime->count());
    } else if (reliability.maxRetransmits) {
        unreliable = true;
        maxRetransmits = *reliability.maxRetransmits;
    } else {
        unreliable = false;
    }

    return create_tel_schich_libdatachannel_DataChannelReliability(env, reliability.unordered, unreliable, maxPacketLifeTime, static_cast<jint>(maxRetransmits));
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_DataChannel_close(JNIEnv *env, jobject instance, const jlong handle) {
    channel::channel_close<rtc::DataChannel>(env, handle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_DataChannel_isClosed(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::is_closed<rtc::DataChannel>(env, handle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_DataChannel_isOpen(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::is_open<rtc::DataChannel>(env, handle);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_DataChannel_maxMessageSize(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::max_message_size<rtc::DataChannel>(env, handle);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_DataChannel_bufferedAmount(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::buffered_amount<rtc::DataChannel>(env, handle);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_DataChannel_availableAmount(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::available_amount<rtc::DataChannel>(env, handle);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_DataChannel_delete(JNIEnv *env, jclass clazz, const jlong handle) {
    util::delete_handle<rtc::DataChannel>(env, handle);
}