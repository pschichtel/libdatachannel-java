#include "callback.hpp"
#include "util.hpp"
#include <jni-c-to-java.h>
#include <jni-java-to-c.h>
#include <jni.h>
#include <rtc/rtc.h>
#include <cstdlib>

void RTC_API handle_local_description(int pc, const char* sdp, const char* type, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onLocalDescription_cstr, sdp, type);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetLocalDescriptionCallback, handle_local_description)

void RTC_API handle_local_candidate(int pc, const char* candidate, const char* mediaId, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onLocalCandidate_cstr, candidate, mediaId);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetLocalCandidateCallback, handle_local_candidate)

void RTC_API handle_state_change(int pc, const rtcState state, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onStateChange, state);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetStateChangeCallback, handle_state_change)

void RTC_API handle_ice_state_change(int pc, const rtcIceState state, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onIceStateChange, state);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetIceStateChangeCallback, handle_ice_state_change)

void RTC_API handle_gathering_state_change(int pc, const rtcGatheringState state, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onGatheringStateChange, state);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetGatheringStateChangeCallback, handle_gathering_state_change)

void RTC_API handle_signaling_state_change(int pc, const rtcSignalingState state, void* ptr) {
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onSignalingStateChange, state);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetSignalingStateChangeCallback, handle_signaling_state_change)

void RTC_API handle_data_channel(int pc, const int channelHandle, void* ptr) {
    rtcSetUserPointer(channelHandle, ptr);
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onDataChannel, channelHandle);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetDataChannelCallback, handle_data_channel)

void RTC_API handle_track(int pc, const int trackHandle, void* ptr) {
    rtcSetUserPointer(trackHandle, ptr);
    DISPATCH_JNI(call_tel_schich_libdatachannel_PeerConnectionListener_onTrack, trackHandle);
}
SET_CALLBACK_INTERFACE_IMPL(rtcSetTrackCallback, handle_track)

JNIEXPORT jint JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcCreatePeerConnection(JNIEnv* env, jclass clazz,
                                                                            jobjectArray iceServers, jstring proxyServer,
                                                                            jstring bindAddress, const jint certificateType,
                                                                            const jint iceTransportPolicy,
                                                                            const jboolean enableIceTcp,
                                                                            const jboolean enableIceUdpMux,
                                                                            const jboolean disableAutoNegotiation,
                                                                            const jboolean forceMediaTransport,
                                                                            const jshort portRangeBegin, const jshort portRangeEnd,
                                                                            const jint mtu, const jint maxMessageSize) {
    rtcConfiguration config = {
            .iceServers = nullptr,
            .iceServersCount = 0,
            .proxyServer = nullptr,
            .bindAddress = nullptr,
            .certificateType = static_cast<rtcCertificateType>(certificateType),
            .iceTransportPolicy = static_cast<rtcTransportPolicy>(iceTransportPolicy),
            .enableIceTcp = static_cast<bool>(enableIceTcp),
            .enableIceUdpMux = static_cast<bool>(enableIceUdpMux),
            .disableAutoNegotiation = static_cast<bool>(disableAutoNegotiation),
            .forceMediaTransport = static_cast<bool>(forceMediaTransport),
            .portRangeBegin = static_cast<uint16_t>(portRangeBegin),
            .portRangeEnd = static_cast<uint16_t>(portRangeEnd),
            .mtu = mtu,
            .maxMessageSize = maxMessageSize,
    };

    jstring* serverStrings = nullptr;

    if (iceServers != nullptr) {
        config.iceServersCount = env->GetArrayLength(iceServers);
        if (config.iceServersCount > 0) {
            config.iceServers = static_cast<const char**>(malloc(sizeof(char*) * config.iceServersCount));
            serverStrings = static_cast<jstring*>(malloc(sizeof(jstring) * config.iceServersCount));
            if (config.iceServers == nullptr || serverStrings == nullptr) {
                free(config.iceServers);
                free(serverStrings);
                throw_native_exception(env, "Failed to allocate for ice servers!");
                return EXCEPTION_THROWN;
            }

            for (int i = 0; i < config.iceServersCount; i++) {
                serverStrings[i] = reinterpret_cast<jstring>(env->GetObjectArrayElement(iceServers, i));// we need a reference to release later
                config.iceServers[i] = env->GetStringUTFChars(serverStrings[i], nullptr);
                if (config.iceServers[i] == nullptr) {
                    // release everything and throw an exception
                    for (int j = 0; j < i; j++) {
                        env->ReleaseStringUTFChars(serverStrings[j], config.iceServers[j]);
                    }
                    free(config.iceServers);
                    free(serverStrings);
                    throw_native_exception(env, "Failed to get ice server string!");
                    return EXCEPTION_THROWN;
                }
            }
        }
    }

    if (proxyServer != nullptr) {
        config.proxyServer = env->GetStringUTFChars(proxyServer, nullptr);
    }
    if (bindAddress != nullptr) {
        config.bindAddress = env->GetStringUTFChars(bindAddress, nullptr);
    }

    const jint result = (jint) rtcCreatePeerConnection(&config);

    if (proxyServer != nullptr) {
        env->ReleaseStringUTFChars(proxyServer, config.proxyServer);
    }
    if (bindAddress != nullptr) {
        env->ReleaseStringUTFChars(bindAddress, config.bindAddress);
    }

    if (config.iceServers != nullptr && config.iceServersCount > 0) {
        if (serverStrings != nullptr) {
            for (int i = 0; i < config.iceServersCount; i++) {
                env->ReleaseStringUTFChars(serverStrings[i], config.iceServers[i]);
            }
        }
        free(config.iceServers);
        free(serverStrings);
    }

    return result;
}

JNIEXPORT jint JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcClosePeerConnection(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return rtcClosePeerConnection(peerHandle);
}

JNIEXPORT jint JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcDeletePeerConnection(JNIEnv* env, jclass clazz,
                                                                            const jint peerHandle) {
    const auto* callback = static_cast<jvm_callback*>(rtcGetUserPointer(peerHandle));
    if (callback != nullptr) {
        free_callback(env, callback);
    }

    return rtcDeletePeerConnection(peerHandle);
}


JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetLocalDescription(JNIEnv* env, jclass clazz, const jint peerHandle, jstring type) {
    const char* c_type;
    if (type == nullptr) {
        c_type = nullptr;
    } else {
        c_type = env->GetStringUTFChars(type, nullptr);
        if (c_type == nullptr) {
            THROW_FAILED_GET_STR(env, type);
            return EXCEPTION_THROWN;
        }
    }
    const int result = WRAP_ERROR(env, rtcSetLocalDescription(peerHandle, c_type));
    env->ReleaseStringUTFChars(type, c_type);
    return result;
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalDescription(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetLocalDescription, peerHandle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalDescriptionType(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetLocalDescriptionType, peerHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetRemoteDescription(JNIEnv* env, jclass clazz, const jint peerHandle, jstring sdp, jstring type) {
    const char* c_sdp = env->GetStringUTFChars(sdp, nullptr);
    if (c_sdp == nullptr) {
        THROW_FAILED_GET_STR(env, sdp);
        return EXCEPTION_THROWN;
    }
    const char* c_type = env->GetStringUTFChars(type, nullptr);
    if (c_type == nullptr) {
        env->ReleaseStringUTFChars(sdp, c_sdp);
        THROW_FAILED_GET_STR(env, type);
        return EXCEPTION_THROWN;
    }
    const int result = WRAP_ERROR(env, rtcSetRemoteDescription(peerHandle, c_sdp, c_type));
    env->ReleaseStringUTFChars(sdp, c_sdp);
    env->ReleaseStringUTFChars(type, c_type);
    return result;
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteDescription(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetRemoteDescription, peerHandle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteDescriptionType(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetRemoteDescriptionType, peerHandle);
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddRemoteCandidate(JNIEnv* env, jclass clazz, const jint peerHandle, jstring candidate, jstring mediaId) {
    const char* c_candidate = nullptr;
    if (candidate != nullptr) {
        c_candidate = env->GetStringUTFChars(candidate, nullptr);
        if (c_candidate == nullptr) {
            THROW_FAILED_GET_STR(env, candidate);
            return EXCEPTION_THROWN;
        }
    }
    const char* c_mediaId = nullptr;
    if (mediaId != nullptr) {
        c_mediaId = env->GetStringUTFChars(mediaId, nullptr);
        if (c_mediaId == nullptr) {
            if (c_candidate != nullptr) {
                env->ReleaseStringUTFChars(candidate, c_candidate);
            }
            THROW_FAILED_GET_STR(env, mediaId);
            return EXCEPTION_THROWN;
        }
    }

    const int result = rtcAddRemoteCandidate(peerHandle, c_candidate, c_mediaId);
    if (c_candidate != nullptr) {
        env->ReleaseStringUTFChars(candidate, c_candidate);
    }
    if (c_mediaId != nullptr) {
        env->ReleaseStringUTFChars(mediaId, c_mediaId);
    }

    return result;
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalAddress(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetLocalAddress, peerHandle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteAddress(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetRemoteAddress, peerHandle);
}

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetSelectedCandidatePair(JNIEnv* env, jclass clazz, const jint peerHandle) {
    constexpr int bufSize = 50;
    const auto local = static_cast<char*>(malloc(bufSize));
    if (local == nullptr) {
        THROW_FAILED_MALLOC(env, local);
        return nullptr;
    }
    const auto remote = static_cast<char*>(malloc(bufSize));
    if (remote == nullptr) {
        free(local);
        THROW_FAILED_MALLOC(env, remote);
        return nullptr;
    }

    const int result = rtcGetSelectedCandidatePair(peerHandle, local, bufSize, remote, bufSize);
    if (result < 0) {
        free(local);
        free(remote);
        WRAP_ERROR(env, result);
        return nullptr;
    }

    jobject candidatePair = call_tel_schich_libdatachannel_CandidatePair_parse_cstr(env, local, remote);

    free(local);
    free(remote);

    return candidatePair;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_setupPeerConnectionListener(JNIEnv* env, jclass clazz, const jint peerHandle, jobject listener) {
    jvm_callback* jvm_callback = allocate_callback(env, listener);
    if (jvm_callback == nullptr) {
        throw_native_exception(env, "Failed to allocate callback for PeerConnectionListener");
        return EXCEPTION_THROWN;
    }
    rtcSetUserPointer(peerHandle, jvm_callback);

    return RTC_ERR_SUCCESS;
}