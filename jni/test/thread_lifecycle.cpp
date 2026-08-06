#include "../src/global_jvm.hpp"
#include "../src/util.hpp"

#include <jni.h>
#include <pthread.h>

namespace {
struct thread_result {
    jobject thread = nullptr;
    const char* error = nullptr;
};

void* attach_and_terminate(void* data) {
    auto* result = static_cast<thread_result*>(data);
    JNIEnv* env = get_jni_env();
    if (env == nullptr) {
        result->error = "Failed to attach native test thread";
        return nullptr;
    }

    jclass thread_class = env->FindClass("java/lang/Thread");
    if (thread_class == nullptr) {
        env->ExceptionClear();
        result->error = "Failed to find java.lang.Thread";
        return nullptr;
    }

    jmethodID current_thread = env->GetStaticMethodID(thread_class, "currentThread", "()Ljava/lang/Thread;");
    if (current_thread == nullptr) {
        env->ExceptionClear();
        result->error = "Failed to find Thread.currentThread";
        return nullptr;
    }

    jobject thread = env->CallStaticObjectMethod(thread_class, current_thread);
    if (thread == nullptr || env->ExceptionCheck()) {
        env->ExceptionClear();
        result->error = "Failed to get current native thread";
        return nullptr;
    }

    result->thread = env->NewGlobalRef(thread);
    if (result->thread == nullptr) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        result->error = "Failed to retain current native thread";
    }
    return nullptr;
}
}

extern "C" JNIEXPORT jobject JNICALL
Java_tel_schich_libdatachannel_NativeThreadLifecycleTest_attachAndTerminateNativeThread(JNIEnv* env, jclass clazz) {
    thread_result result;
    pthread_t thread;
    if (pthread_create(&thread, nullptr, attach_and_terminate, &result) != 0) {
        throw_native_exception(env, "Failed to create native test thread");
        return nullptr;
    }
    if (pthread_join(thread, nullptr) != 0) {
        throw_native_exception(env, "Failed to join native test thread");
        return nullptr;
    }
    if (result.error != nullptr) {
        throw_native_exception(env, result.error);
        return nullptr;
    }

    jobject java_thread = env->NewLocalRef(result.thread);
    env->DeleteGlobalRef(result.thread);
    return java_thread;
}
