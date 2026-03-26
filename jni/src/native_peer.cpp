#include "callback.hpp"
#include "rtc/peerconnection.hpp"
#include "util.hpp"

#include <jni-c-to-java.h>
#include <jni-java-to-c.h>
#include <jni.h>
#include <memory>
#include <utility>

template <typename CallbackSignature, typename Bridge>
void setup_peer_connection_callback(
    JNIEnv* env,
    jlong peerHandle,
    jobject listener,
    jboolean set,
    const std::function<void(rtc::PeerConnection*, std::function<CallbackSignature>)>& setter,
    Bridge bridge
) {
    const auto peerConnection = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle)->get();

    if (set) {
        auto vm = get_jvm_from_env(env);
        if (vm == nullptr) {
            throw_tel_schich_libdatachannel_exception_FailureException_cstr(env, "Failed to get JavaVM reference!");
            return;
        }

        std::shared_ptr<_jobject> listener_ref(
            env->NewGlobalRef(listener),
            [vm](jobject obj) {
                const auto local_env = get_jni_env_from_jvm(vm);
                if (local_env != nullptr) {
                    local_env->DeleteGlobalRef(obj);
                }
            }
        );
        setter(peerConnection, [vm, listener_ref, bridge](auto... args) {
            const auto local_env = get_jni_env_from_jvm(vm);
            if (local_env != nullptr) {
                bridge(local_env, listener_ref.get(), args...);
            }
        });
    } else {
        setter(peerConnection, nullptr);
    }
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetLocalDescriptionCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(rtc::Description)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onLocalDescription(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const rtc::Description& desc) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onLocalDescription_cstr(
                local_env, listener, std::string(desc).c_str(), desc.typeString().c_str()
            );
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetStateChangeCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(rtc::PeerConnection::State)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onStateChange(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const rtc::PeerConnection::State& state) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onStateChange(local_env, listener, static_cast<jint>(state));
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetIceStateChangeCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(rtc::PeerConnection::IceState)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onIceStateChange(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const rtc::PeerConnection::IceState& state) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onIceStateChange(local_env, listener, static_cast<jint>(state));
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetGatheringStateChangeCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(rtc::PeerConnection::GatheringState)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onGatheringStateChange(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const rtc::PeerConnection::GatheringState& state) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onGatheringStateChange(local_env, listener, static_cast<jint>(state));
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetSignalingStateChangeCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(rtc::PeerConnection::SignalingState)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onSignalingStateChange(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const rtc::PeerConnection::SignalingState& state) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onSignalingStateChange(local_env, listener, static_cast<jint>(state));
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetDataChannelCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(std::shared_ptr<rtc::DataChannel>)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onDataChannel(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const std::shared_ptr<rtc::DataChannel>& channel) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onDataChannel(local_env, listener, util::handlize(local_env, channel));
        }
    );
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcSetTrackCallback(JNIEnv* env, jclass clazz, jlong peerHandle, jobject listener, jboolean set) {
    util::setup_rtc_callback<rtc::PeerConnection, void(std::shared_ptr<rtc::Track>)>(
        env,
        peerHandle,
        listener,
        set,
        [](auto pc, auto cb) { pc->onTrack(std::move(cb)); },
        [](JNIEnv* local_env, jobject listener, const std::shared_ptr<rtc::Track>& track) {
            call_tel_schich_libdatachannel_PeerConnectionListener_onTrack(local_env, listener, util::handlize(local_env, track));
        }
    );
}

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
    return util::handlize(env, peerConnection);
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
    util::delete_handle<rtc::PeerConnection>(env, peerHandle);
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
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();
    return util::get_string_for_java(env, [&]() -> std::optional<std::string> {
        if (const auto desc = peer_connection->localDescription(); desc.has_value()) {
            return std::string(desc.value());
        }
        return nullptr;
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalDescriptionType(JNIEnv* env, jclass clazz, const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();
    return util::get_string_for_java(env, [&]() -> std::optional<std::string> {
        if (const auto desc = peer_connection->localDescription(); desc.has_value()) {
            return desc.value().typeString();
        }
        return nullptr;
    });
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
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();
    return util::get_string_for_java(env, [&]() -> std::optional<std::string> {
        if (const auto desc = peer_connection->remoteDescription(); desc.has_value()) {
            return std::string(desc.value());
        }
        return nullptr;
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteDescriptionType(JNIEnv* env, jclass clazz, const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();
    return util::get_string_for_java(env, [&]() -> std::optional<std::string> {
        if (const auto desc = peer_connection->remoteDescription(); desc.has_value()) {
            return desc.value().typeString();
        }
        return nullptr;
    });
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddRemoteCandidate(JNIEnv* env, jclass clazz, const jint peerHandle, jstring candidate, jstring mediaId) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    rtc::Candidate c(util::getJavaString(env, candidate), util::getJavaString(env, mediaId));

    util::wrap(env, [persistentPtr, &c] {
        persistentPtr->get()->addRemoteCandidate(c);
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetLocalAddress(JNIEnv* env, jclass clazz, const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();
    return util::get_string_for_java(env, [&] {
        return peer_connection->localAddress();
    });
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetRemoteAddress(JNIEnv* env, jclass clazz, const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();
    return util::get_string_for_java(env, [&]() -> std::optional<std::string> {
        return peer_connection->remoteAddress();
    });
}

JNIEXPORT jobject JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetSelectedCandidatePair(JNIEnv* env, jclass clazz, const jint peerHandle) {
    const auto persistentPtr = reinterpret_cast<std::shared_ptr<rtc::PeerConnection>*>(peerHandle);
    const auto peer_connection = persistentPtr->get();

    rtc::Candidate local;
    rtc::Candidate remote;
    return util::wrapResult(env, [&]() -> jobject {
        if (!peer_connection->getSelectedCandidatePair(&local, &remote)) {
            throw_tel_schich_libdatachannel_exception_NotAvailableException_cstr(env, "No candidate pair selected!");
            return nullptr;
        }
        return call_tel_schich_libdatachannel_CandidatePair_parse_cstr(env, std::string(local).c_str(), std::string(remote).c_str());
    });

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