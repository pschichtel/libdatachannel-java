#include "global_jvm.hpp"
#include "jni-c-to-java.h"
#include "rtc/global.hpp"

#include <atomic>
#include <chrono>
#include <jni.h>
#include <pthread.h>

using namespace std::chrono_literals;

#define JNI_VERSION JNI_VERSION_1_6

static JavaVM* global_JVM;
static pthread_key_t thread_key;
static std::atomic jvm_unloading(false);

void detach_thread(void*) {
    const auto jvm = static_cast<JavaVM*>(pthread_getspecific(thread_key));
    if (jvm != nullptr) {
        jvm->DetachCurrentThread();
    }
}

JNIEnv* get_jni_env_from_jvm(JavaVM* jvm) {
    JNIEnv* env;
    jint result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION);
    if (result == JNI_EDETACHED) {
        result = jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);
        if (result == JNI_OK) {
            pthread_setspecific(thread_key, jvm);
        }
    }
    if (result != JNI_OK) {
        return nullptr;
    }
    return env;
}

JavaVM* get_jvm_from_env(JNIEnv* env) {
    JavaVM* jvm = nullptr;

    if (const jint result = env->GetJavaVM(&jvm); result != JNI_OK) {
        // handle error (should not normally happen)
        return nullptr;
    }

    return jvm;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
    pthread_key_create(&thread_key, detach_thread);
    global_JVM = jvm;
    jvm_unloading.store(false);
    JNIEnv* env = get_jni_env_from_jvm(jvm);
    module_OnLoad(env);
    rtc::InitLogger(rtc::LogLevel::Verbose, [&jvm](rtc::LogLevel level, const std::string& message) {
        JNIEnv* env = get_jni_env_from_jvm(jvm);
        if (env != nullptr) {
            call_tel_schich_libdatachannel_LibDataChannel_log_cstr(env, static_cast<jint>(level), message.c_str());
        }
    });
    rtc::Preload();
    return JNI_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* jvm, void* reserved) {
    jvm_unloading.store(true);
	auto env = get_jni_env_from_jvm(jvm);
    if (env != nullptr) {
	    if (rtc::Cleanup().wait_for(10s) == std::future_status::timeout) {
            call_tel_schich_libdatachannel_LibDataChannel_log_cstr(env, static_cast<jint>(rtc::LogLevel::Error), "Cleanup timed out!");
	    }
        module_OnUnload(env);
    }
}