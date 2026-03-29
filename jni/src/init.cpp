#include "jni-c-to-java.h"
#include "rtc/global.hpp"
#include "util.hpp"

#include <atomic>
#include <chrono>
#include <jni.h>
#include <pthread.h>

using namespace std::chrono_literals;

static pthread_key_t thread_key;
static std::atomic jvm_unloading(false);

void detach_thread(void*) {
    if (const auto jvm = static_cast<JavaVM*>(pthread_getspecific(thread_key)); jvm != nullptr) {
        jvm->DetachCurrentThread();
    }
}

JNIEnv* util::get_jni_env_from_jvm(JavaVM* const vm) {
    JNIEnv* env;
    jint result = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION);
    if (result == JNI_EDETACHED) {
        result = vm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);
        if (result == JNI_OK) {
            pthread_setspecific(thread_key, vm);
        }
    }
    if (result != JNI_OK) {
        return nullptr;
    }
    return env;
}

JavaVM* util::get_jvm_from_env(JNIEnv* const env) {
    JavaVM* jvm = nullptr;

    if (const jint result = env->GetJavaVM(&jvm); result != JNI_OK) {
        // handle error (should not normally happen)
        return nullptr;
    }

    return jvm;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* const vm, void* const reserved) {
    pthread_key_create(&thread_key, detach_thread);
    jvm_unloading.store(false);
    JNIEnv* env = util::get_jni_env_from_jvm(vm);
    module_OnLoad(env);
    rtc::InitLogger(rtc::LogLevel::Verbose, [vm](rtc::LogLevel level, const std::string& message) {
        if (JNIEnv* local_env = util::get_jni_env_from_jvm(vm); local_env != nullptr) {
            call_tel_schich_libdatachannel_LibDataChannel_log_cstr(local_env, static_cast<jint>(level), message.c_str());
        }
    });
    rtc::Preload();
    return util::JNI_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* const vm, void* const reserved) {
    jvm_unloading.store(true);
    if (const auto env = util::get_jni_env_from_jvm(vm); env != nullptr) {
	    if (rtc::Cleanup().wait_for(10s) == std::future_status::timeout) {
            call_tel_schich_libdatachannel_LibDataChannel_log_cstr(env, static_cast<jint>(rtc::LogLevel::Error), "Cleanup timed out!");
	    }
        module_OnUnload(env);
    }
}