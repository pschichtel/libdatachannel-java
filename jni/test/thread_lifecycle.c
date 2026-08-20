#include "../src/global_jvm.h"
#include "../src/util.h"

#include <jni.h>
#include <pthread.h>

struct thread_result {
    jobject thread;
    char* error;
};

static void* attach_and_terminate(void* data) {
    struct thread_result* result = data;
    JNIEnv* env = get_jni_env();
    if (env == NULL) {
        result->error = "Failed to attach native test thread";
        return NULL;
    }

    jclass thread_class = (*env)->FindClass(env, "java/lang/Thread");
    if (thread_class == NULL) {
        (*env)->ExceptionClear(env);
        result->error = "Failed to find java.lang.Thread";
        return NULL;
    }

    jmethodID current_thread = (*env)->GetStaticMethodID(env, thread_class, "currentThread", "()Ljava/lang/Thread;");
    if (current_thread == NULL) {
        (*env)->ExceptionClear(env);
        result->error = "Failed to find Thread.currentThread";
        return NULL;
    }

    jobject thread = (*env)->CallStaticObjectMethod(env, thread_class, current_thread);
    if (thread == NULL || (*env)->ExceptionCheck(env)) {
        (*env)->ExceptionClear(env);
        result->error = "Failed to get current native thread";
        return NULL;
    }

    result->thread = (*env)->NewGlobalRef(env, thread);
    if (result->thread == NULL) {
        if ((*env)->ExceptionCheck(env)) {
            (*env)->ExceptionClear(env);
        }
        result->error = "Failed to retain current native thread";
    }
    return NULL;
}

JNIEXPORT jobject JNICALL
Java_tel_schich_libdatachannel_NativeThreadLifecycleTest_attachAndTerminateNativeThread(JNIEnv* env, jclass clazz) {
    struct thread_result result = {0};
    pthread_t thread;
    if (pthread_create(&thread, NULL, attach_and_terminate, &result) != 0) {
        throw_native_exception(env, "Failed to create native test thread");
        return NULL;
    }
    if (pthread_join(thread, NULL) != 0) {
        throw_native_exception(env, "Failed to join native test thread");
        return NULL;
    }
    if (result.error != NULL) {
        throw_native_exception(env, result.error);
        return NULL;
    }

    jobject java_thread = (*env)->NewLocalRef(env, result.thread);
    (*env)->DeleteGlobalRef(env, result.thread);
    return java_thread;
}
