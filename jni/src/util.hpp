#pragma once

#include "global_jvm.hpp"
#include <jni-c-to-java.h>
#include <rtc/rtc.h>
#include <stdexcept>
#include <string>

#define EXCEPTION_THROWN (-999)

typedef int(RTC_API* get_dynamic_string_func)(int peerHandle, char* buffer, int size);

#define GET_DYNAMIC_STRING(env, func, handle) get_dynamic_string(env, #func, func, handle)

jstring get_dynamic_string(JNIEnv* env, const char* func_name, get_dynamic_string_func func, int handle);

#define WRAP_ERROR(env, expr) wrap_error(env, #expr, (expr))

jint wrap_error(JNIEnv* env, const char* message, int result);

void throw_native_exception(JNIEnv* env, const char* msg);

#define THROW_FAILED_GET_STR(env, expr) throw_native_exception(env, "failed to get string for " #expr)

#define THROW_FAILED_MALLOC(env, expr) throw_native_exception(env, "failed to malloc for " #expr)

#define DISPATCH_JNI(target, args...)                          \
    if (ptr == nullptr) {                                      \
        return;                                                \
    }                                                          \
    struct jvm_callback* cb = static_cast<jvm_callback*>(ptr); \
    JNIEnv* env = get_jni_env();                               \
    if (env == nullptr) {                                      \
        return;                                                \
    }                                                          \
    target(env, cb->instance, args)

#define SETUP_HANDLER(peer, api, target) \
    if (WRAP_ERROR(env, api(peer, target)) == EXCEPTION_THROWN) return EXCEPTION_THROWN

#define SET_CALLBACK_INTERFACE_IMPL(api, target)                                                                                             \
    JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_##api(JNIEnv* env, jclass clazz, jint handle, jboolean set) { \
        return api(handle, set ? target : NULL);                                                                                             \
    }

namespace util {
    std::string getJavaString(JNIEnv* env, jstring s);

    template <typename F> void wrap(JNIEnv* env, F func) {
        try {
            func();
        } catch (const std::invalid_argument &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
        } catch (const std::exception &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
        }
    }
}