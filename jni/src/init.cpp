#include "global_jvm.hpp"
#include "jni-c-to-java.h"

#include <jni.h>
#include <mutex>
#include <pthread.h>
#include <rtc/rtc.h>

#define JNI_VERSION JNI_VERSION_1_6

static std::mutex lifecycle_mutex;
static JavaVM* global_jvm = nullptr;
static pthread_key_t thread_key;
static bool thread_key_initialized = false;
static bool jvm_unloading = true;

namespace {
void stop_jvm_access() {
    const std::lock_guard lock(lifecycle_mutex);
    jvm_unloading = true;
    global_jvm = nullptr;
}

void delete_thread_key() {
    const std::lock_guard lock(lifecycle_mutex);
    if (thread_key_initialized) {
        pthread_key_delete(thread_key);
        thread_key_initialized = false;
    }
}
}

void detach_thread(void* value) {
    const auto jvm = static_cast<JavaVM*>(value);
    if (jvm != nullptr) {
        jvm->DetachCurrentThread();
    }
}

JNIEnv* get_jni_env() {
    const std::lock_guard lock(lifecycle_mutex);
    if (global_jvm == nullptr || jvm_unloading || !thread_key_initialized) {
        return nullptr;
    }

    JNIEnv* env = nullptr;
    JavaVM* jvm = global_jvm;
    jint result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION);
    if (result == JNI_EDETACHED) {
        result = jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);
        if (result == JNI_OK) {
            if (pthread_setspecific(thread_key, jvm) != 0) {
                jvm->DetachCurrentThread();
                return nullptr;
            }
        }
    }
    if (result != JNI_OK || env == nullptr) {
        return nullptr;
    }
    return env;
}

void logger_callback(const rtcLogLevel level, const char* message) {
    if (message == nullptr) {
        return;
    }
    JNIEnv* env = get_jni_env();
    if (env != nullptr) {
        call_tel_schich_libdatachannel_LibDataChannel_log_cstr(env, level, message);
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
    if (jvm == nullptr || pthread_key_create(&thread_key, detach_thread) != 0) {
        return JNI_ERR;
    }
    {
        const std::lock_guard lock(lifecycle_mutex);
        thread_key_initialized = true;
    }

    JNIEnv* env = nullptr;
    if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION) != JNI_OK || env == nullptr) {
        delete_thread_key();
        return JNI_ERR;
    }

    module_OnLoad(env);
    if (env->ExceptionCheck()) {
        module_OnUnload(env);
        delete_thread_key();
        return JNI_ERR;
    }

    {
        const std::lock_guard lock(lifecycle_mutex);
        global_jvm = jvm;
        jvm_unloading = false;
    }
    rtcInitLogger(RTC_LOG_VERBOSE, &logger_callback);
    rtcPreload();
    if (env->ExceptionCheck()) {
        stop_jvm_access();
        rtcInitLogger(RTC_LOG_NONE, nullptr);
        rtcCleanup();
        module_OnUnload(env);
        delete_thread_key();
        return JNI_ERR;
    }
    return JNI_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* jvm, void* reserved) {
    stop_jvm_access();
    rtcInitLogger(RTC_LOG_NONE, nullptr);
    rtcCleanup();

    JNIEnv* env = nullptr;
    if (jvm != nullptr && jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION) == JNI_OK && env != nullptr) {
        module_OnUnload(env);
    }
    delete_thread_key();
}
