#pragma once

#include <jni.h>
#include "util.hpp"

namespace channel {
    template<typename Channel>
    void channel_close(JNIEnv* const env, const jlong handle) {
        const auto channel = util::dehandlize<Channel>(env, handle);
        channel->close();
    }

    template<typename Channel>
    jboolean is_closed(JNIEnv* const env, const jlong handle) {
        const auto channel = util::dehandlize<Channel>(env, handle);
        return channel->isClosed();
    }

    template<typename Channel>
    jboolean is_open(JNIEnv* const env, const jlong handle) {
        const auto channel = util::dehandlize<Channel>(env, handle);
        return channel->isOpen();
    }

    template<typename Channel>
    jlong max_message_size(JNIEnv* const env, const jlong handle) {
        const auto channel = util::dehandlize<Channel>(env, handle);
        return channel->maxMessageSize();
    }

    template<typename Channel>
    jlong buffered_amount(JNIEnv* const env, const jlong handle) {
        const auto channel = util::dehandlize<Channel>(env, handle);
        return channel->bufferedAmount();
    }

    template<typename Channel>
    jlong available_amount(JNIEnv* const env, const jlong handle) {
        const auto channel = util::dehandlize<Channel>(env, handle);
        return channel->availableAmount();
    }

    template<typename Channel>
    void set_open_callback(JNIEnv* const env, const jlong handle, jobject listener, const jboolean set) {
        util::setup_callback<Channel, void()>(
            env,
            handle,
            listener,
            set,
            [](auto dc, auto cb) { dc->onOpen(std::move(cb)); },
            [&handle](JNIEnv* const local_env, const jobject local_listener) {
                call_tel_schich_libdatachannel_PeerConnectionListener_onChannelOpen(local_env, local_listener, handle);
            }
        );
    }

    template<typename Channel>
    void set_closed_callback(JNIEnv* const env, const jlong handle, jobject listener, const jboolean set) {
        util::setup_callback<Channel, void()>(
            env,
            handle,
            listener,
            set,
            [](auto dc, auto cb) { dc->onClosed(std::move(cb)); },
            [&handle](JNIEnv* const local_env, const jobject local_listener) {
                call_tel_schich_libdatachannel_PeerConnectionListener_onChannelClosed(local_env, local_listener, handle);
            }
        );
    }

    template<typename Channel>
    void set_error_callback(JNIEnv* const env, const jlong handle, jobject listener, const jboolean set) {
        util::setup_callback<Channel, void(std::string error)>(
            env,
            handle,
            listener,
            set,
            [](auto dc, auto cb) { dc->onError(std::move(cb)); },
            [&handle](JNIEnv* const local_env, const jobject local_listener, const std::string& error) {
                call_tel_schich_libdatachannel_PeerConnectionListener_onChannelError_cstr(local_env, local_listener, handle, error.c_str());
            }
        );
    }

    template<typename Channel>
    void set_available_callback(JNIEnv* const env, const jlong channelHandle, jobject listener, const jboolean set) {
        util::setup_callback<Channel, void()>(
            env,
            channelHandle,
            listener,
            set,
            [](auto dc, auto cb) { dc->onAvailable(std::move(cb)); },
            [channelHandle](JNIEnv* const local_env, const jobject local_listener) {
                call_tel_schich_libdatachannel_PeerConnectionListener_onChannelAvailable(local_env, local_listener, channelHandle);
            }
        );
    }
}
