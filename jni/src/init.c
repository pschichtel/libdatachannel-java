#include <rtc/rtc.h>
#include <jni.h>
#include <pthread.h>
#include "global_jvm.h"
#include "jni-c-to-java.h"

static JavaVM* global_JVM;
static pthread_key_t thread_key;

void detach_thread() {
    JavaVM* jvm = pthread_getspecific(thread_key);
    if (jvm != NULL) {
        (*jvm)->DetachCurrentThread(jvm);
    }
}

JNIEnv* get_jni_env_from_jvm(JavaVM* jvm) {
    JNIEnv* env;
    jint result = (*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_10);
    if (result == JNI_EDETACHED) {
        result = (*jvm)->AttachCurrentThreadAsDaemon(jvm, (void**)&env, NULL);
        if (result == JNI_OK) {
            pthread_setspecific(thread_key, jvm);
        }
    }
    if (result != JNI_OK) {
        return NULL;
    }
    return env;
}

JNIEnv* get_jni_env() {
    // make sure it's initialized
    if (global_JVM == NULL) {
        return NULL;
    }
    return get_jni_env_from_jvm(global_JVM);
}

void logger_callback(rtcLogLevel level, const char *message) {
    if (message == NULL) {
        return;
    }
    JNIEnv* env = get_jni_env();
    if (env != NULL) {
        call_tel_schich_libdatachannel_LibDataChannel_log_cstr(env, level, message);
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    pthread_key_create(&thread_key, detach_thread);
    global_JVM = jvm;
    rtcInitLogger(RTC_LOG_VERBOSE, &logger_callback);
    rtcPreload();
    return JNI_VERSION_10;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    rtcCleanup();
    global_JVM = NULL;
}