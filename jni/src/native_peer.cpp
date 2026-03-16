#include "callback.hpp"
#include "rtc/peerconnection.hpp"
#include "util.hpp"

#include <cstdlib>
#include <jni-c-to-java.h>
#include <jni-java-to-c.h>
#include <jni.h>
#include <memory>
#include <rtc/rtc.h>

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

JNIEXPORT jlong JNICALL
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
    rtc::Configuration c;
    if (iceServers != nullptr) {
        const int serverCount = env->GetArrayLength(iceServers);

        for (int i = 0; i < serverCount; i++) {
            const auto server = reinterpret_cast<jstring>(env->GetObjectArrayElement(iceServers, i));
            c.iceServers.emplace_back(util::getJavaString(env, server));
        }
    }

    if (proxyServer != nullptr) {
        c.proxyServer.emplace(util::getJavaString(env, proxyServer));
    }
    if (bindAddress != nullptr) {
        c.bindAddress = util::getJavaString(env, bindAddress);
    }

    if (portRangeBegin > 0 || portRangeEnd > 0) {
        c.portRangeBegin = portRangeBegin;
        c.portRangeEnd = portRangeEnd;
    }

    c.certificateType = static_cast<rtc::CertificateType>(certificateType);
    c.iceTransportPolicy = static_cast<rtc::TransportPolicy>(iceTransportPolicy);
    c.enableIceTcp = enableIceTcp;
    c.enableIceUdpMux = enableIceUdpMux;
    c.disableAutoNegotiation = disableAutoNegotiation;
    c.forceMediaTransport = forceMediaTransport;

    if (mtu > 0) {
        c.mtu = mtu;
    }

    if (maxMessageSize) {
        c.maxMessageSize = static_cast<size_t>(maxMessageSize);
    }

    const auto peerConnection = std::make_shared<rtc::PeerConnection>(std::move(c));
    return reinterpret_cast<jlong>(new std::shared_ptr(peerConnection));
}

JNIEXPORT void JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcClosePeerConnection(JNIEnv* env, jclass clazz, const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    util::wrap(env, [persistentPtr] {
        persistentPtr->get()->close();
    });
}

JNIEXPORT void JNICALL
Java_tel_schich_libdatachannel_LibDataChannelNative_rtcDeletePeerConnection(JNIEnv* env, jclass clazz,
                                                                            const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    delete persistentPtr;
}


JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetLocalDescription(JNIEnv* env, jclass clazz, const jlong peerHandle, jstring type) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto sdpType = type == nullptr
        ? rtc::Description::Type::Unspec
        : rtc::Description::stringToType(util::getJavaString(env, type));

    util::wrap(env, [persistentPtr, &sdpType] {
        persistentPtr->get()->setLocalDescription(sdpType);
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalDescription(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetLocalDescription, peerHandle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalDescriptionType(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetLocalDescriptionType, peerHandle);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetRemoteDescription(JNIEnv* env, jclass clazz, const jint peerHandle, jstring sdp, jstring type) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto sdpType = type == nullptr
        ? rtc::Description::Type::Unspec
        : rtc::Description::stringToType(util::getJavaString(env, type));
    const rtc::Description description(util::getJavaString(env, sdp), sdpType);

    util::wrap(env, [persistentPtr, &description] {
        persistentPtr->get()->setRemoteDescription(description);
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteDescription(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetRemoteDescription, peerHandle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteDescriptionType(JNIEnv* env, jclass clazz, const jint peerHandle) {
    return GET_DYNAMIC_STRING(env, rtcGetRemoteDescriptionType, peerHandle);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddRemoteCandidate(JNIEnv* env, jclass clazz, const jint peerHandle, jstring candidate, jstring mediaId) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    rtc::Candidate c(util::getJavaString(env, candidate), util::getJavaString(env, mediaId));

    util::wrap(env, [persistentPtr, &c] {
        persistentPtr->get()->addRemoteCandidate(c);
    });
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