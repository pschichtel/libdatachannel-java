#include "native_channel.hpp"
#include "rtc/peerconnection.hpp"
#include "rtc/track.hpp"
#include "util.hpp"

#include <jni-java-to-c.h>
#include <jni.h>

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddTrack(JNIEnv* env, jclass clazz, const jlong peerHandle, jstring sdp) {
    if (sdp == nullptr) {
        throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "No SDP given!");
        return util::EXCEPTION_THROWN;
    }
    const auto peer = util::dehandlize<rtc::PeerConnection>(env, peerHandle);

    const rtc::Description::Media media{util::getJavaString(env, sdp)};
    const auto track = peer->addTrack(media);

    return util::handlize(env, track);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcAddTrackEx(JNIEnv* env, jclass clazz, const jlong peerHandle, jint direction, jint codec, jstring mid) {
    const auto peer = util::dehandlize<rtc::PeerConnection>(env, peerHandle);
    auto dir = static_cast<rtc::Description::Direction>(direction);

    std::string mediaId;
    if (mid) {
        mediaId = util::getJavaString(env, mid);
    } else {
        switch (codec) {
            case RTC_CODEC_AV1:
            case RTC_CODEC_H264:
            case RTC_CODEC_H265:
            case RTC_CODEC_VP8:
            case RTC_CODEC_VP9:
                mediaId = "video";
                break;
            case RTC_CODEC_OPUS:
            case RTC_CODEC_PCMU:
            case RTC_CODEC_PCMA:
            case RTC_CODEC_AAC:
                mediaId = "audio";
                break;
            default:
                mediaId = "video";
                break;
        }
    }

    constexpr int pt = 0;
    const std::optional<std::string> profile = std::nullopt;
    std::unique_ptr<rtc::Description::Media> description;
    switch (codec) {
        case RTC_CODEC_AV1:
        case RTC_CODEC_H264:
        case RTC_CODEC_H265:
        case RTC_CODEC_VP8:
        case RTC_CODEC_VP9: {
            auto video = std::make_unique<rtc::Description::Video>(mediaId, dir);
            switch (codec) {
                case RTC_CODEC_AV1:
                    video->addAV1Codec(pt, profile);
                    break;
                case RTC_CODEC_H264:
                    video->addH264Codec(pt, profile);
                    break;
                case RTC_CODEC_H265:
                    video->addH265Codec(pt, profile);
                    break;
                case RTC_CODEC_VP8:
                    video->addVP8Codec(pt, profile);
                    break;
                case RTC_CODEC_VP9:
                    video->addVP9Codec(pt, profile);
                    break;
                default:
                    throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Invalid codec given!");
                    return util::EXCEPTION_THROWN;
            }
            description = std::move(video);
            break;
        }
        case RTC_CODEC_OPUS:
        case RTC_CODEC_PCMU:
        case RTC_CODEC_PCMA:
        case RTC_CODEC_AAC:
        case RTC_CODEC_G722: {
            auto audio = std::make_unique<rtc::Description::Audio>(mediaId, dir);
            switch (codec) {
                case RTC_CODEC_OPUS:
                    audio->addOpusCodec(pt, profile);
                    break;
                case RTC_CODEC_PCMU:
                    audio->addPCMUCodec(pt, profile);
                    break;
                case RTC_CODEC_PCMA:
                    audio->addPCMACodec(pt, profile);
                    break;
                case RTC_CODEC_AAC:
                    audio->addAACCodec(pt, profile);
                    break;
                case RTC_CODEC_G722:
                    audio->addG722Codec(pt, profile);
                    break;
                default:
                    throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Invalid codec given!");
                    return util::EXCEPTION_THROWN;
            }
            description = std::move(audio);
            break;
        }
        default:
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Invalid codec given!");
            return util::EXCEPTION_THROWN;
    }
    const auto track = peer->addTrack(*description);
    return util::handlize(env, track);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcDeleteTrack(JNIEnv* env, jclass clazz, const jlong trackHandle) {
    util::delete_handle<rtc::Track>(env, trackHandle);
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_Track_close(JNIEnv *env, jobject instance, const jlong handle) {
    channel::channel_close<rtc::Track>(env, handle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_Track_isClosed(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::is_closed<rtc::Track>(env, handle);
}

JNIEXPORT jboolean JNICALL Java_tel_schich_libdatachannel_Track_isOpen(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::is_open<rtc::Track>(env, handle);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_Track_maxMessageSize(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::max_message_size<rtc::Track>(env, handle);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_Track_bufferedAmount(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::buffered_amount<rtc::Track>(env, handle);
}

JNIEXPORT jlong JNICALL Java_tel_schich_libdatachannel_Track_availableAmount(JNIEnv *env, jobject instance, const jlong handle) {
    return channel::available_amount<rtc::Track>(env, handle);
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_Track_getTrackDescription(JNIEnv *env, jclass clazz, const jlong trackHandle) {
    auto track = util::dehandlize<rtc::Track>(env, trackHandle);
    return util::get_string_for_java(env, [&track] {
        return track->description();
    });
}

JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_Track_getTrackDirection(JNIEnv *env, jclass clazz, const jlong trackHandle) {
    auto track = util::dehandlize<rtc::Track>(env, trackHandle);
    return static_cast<jint>(track->direction());
}

JNIEXPORT jstring JNICALL Java_tel_schich_libdatachannel_Track_getTrackMid(JNIEnv *env, jclass clazz, const jlong trackHandle) {
    auto track = util::dehandlize<rtc::Track>(env, trackHandle);
    return util::get_string_for_java(env, [&track] {
        return track->mid();
    });
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_Track_delete(JNIEnv *env, jclass clazz, const jlong handle) {
    util::delete_handle<rtc::Track>(env, handle);
}
