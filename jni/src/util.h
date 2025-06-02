#ifndef LIBDATACHANNEL_JNI_UTIL_H
#define LIBDATACHANNEL_JNI_UTIL_H

#include <rtc/rtc.h>
#include <jni-c-to-java.h>
#include "global_jvm.h"

#define EXCEPTION_THROWN (-999)

typedef int(RTC_API *get_dynamic_string_func)(int peerHandle, char* buffer, int size);

#define GET_DYNAMIC_STRING(env, func, handle) get_dynamic_string(env, #func, func, handle)

jstring get_dynamic_string(JNIEnv *env, const char* func_name, get_dynamic_string_func func, int handle);

#define WRAP_ERROR(env, expr) wrap_error(env, #expr, (expr))

jint wrap_error(JNIEnv* env, const char* message, int result);

void throw_native_exception(JNIEnv *env, char *msg);

#define THROW_FAILED_GET_STR(env, expr) throw_native_exception(env, "failed to get string for " #expr)

#define THROW_FAILED_MALLOC(env, expr) throw_native_exception(env, "failed to malloc for " #expr)

#define DISPATCH_JNI(target, args...) \
    struct jvm_callback* cb = ptr; \
    JNIEnv* env = get_jni_env(); \
    if (env == NULL) return; \
    target(env, cb->instance, args); \
    release_jni_env()

#define SETUP_HANDLER(peer, api, target) \
    if (WRAP_ERROR(env, api(peer, target)) == EXCEPTION_THROWN) return EXCEPTION_THROWN

#define SET_CALLBACK_INTERFACE_IMPL(api, target) \
JNIEXPORT jint JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_##api(JNIEnv *env, jclass clazz, jint handle, jboolean set) { \
    return api(handle, set ? target : NULL); \
}
#endif //LIBDATACHANNEL_JNI_UTIL_H
