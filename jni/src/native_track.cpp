#include "rtc/track.hpp"
#include "util.hpp"

#include <jni-java-to-c.h>
#include <jni.h>
#include <rtc/rtc.h>

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddTrack(JNIEnv* env, jclass clazz, const jint peerHandle, jstring sdp) {
    const char* chars = nullptr;
    if (sdp != nullptr) {
        chars = env->GetStringUTFChars(sdp, nullptr);
        if (chars == nullptr) {
            THROW_FAILED_GET_STR(env, sdp);
            return EXCEPTION_THROWN;
        }
    }
    const int result = rtcAddTrack(peerHandle, chars);
    rtcSetUserPointer(result, rtcGetUserPointer(peerHandle));
    if (sdp != nullptr) {
        env->ReleaseStringUTFChars(sdp, chars);
    }
    return result;
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddTrackEx(JNIEnv* env, jclass clazz, const jint peerHandle, jint direction, jint codec) {
    const rtcTrackInit init = {
            .direction = static_cast<rtcDirection>(direction),
            .codec = static_cast<rtcCodec>(codec),
            .payloadType = 0,
            .ssrc = 0,
            .mid = nullptr,
            .name = nullptr,
            .msid = nullptr,
            .trackId = nullptr,
            .profile = nullptr,
    };
    return rtcAddTrackEx(peerHandle, &init);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetTrackDescription(JNIEnv* env, jclass clazz, const jlong trackHandle) {
    auto track = util::dehandlize<rtc::Track>(env, trackHandle);
    return util::get_string_for_java(env, [&track] {
        return track->description();
    });
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetTrackDirection(JNIEnv* env, jclass clazz, const jlong trackHandle) {
    auto track = util::dehandlize<rtc::Track>(env, trackHandle);
    return static_cast<jint>(track->direction());
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcGetTrackMid(JNIEnv* env, jclass clazz, const jlong trackHandle) {
    auto track = util::dehandlize<rtc::Track>(env, trackHandle);
    return util::get_string_for_java(env, [&track] {
        return track->mid();
    });
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcDeleteTrack(JNIEnv* env, jclass clazz, const jlong trackHandle) {
    util::delete_handle<rtc::Track>(env, trackHandle);
}
