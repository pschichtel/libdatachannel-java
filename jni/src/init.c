#include <rtc/rtc.h>
#include <jni.h>
#include "global_jvm.h"
#include "jni-c-to-java.h"

static JavaVM* global_JVM;

JNIEnv* get_jni_env() {
    // make sure it's initialized
    if (global_JVM == NULL) {
        return NULL;
    }
    JNIEnv* env;
    jint result = (*global_JVM)->GetEnv(global_JVM, (void **)&env, JNI_VERSION_10);
    if (result == JNI_EDETACHED) {
        result = (*global_JVM)->AttachCurrentThreadAsDaemon(global_JVM, (void**)&env, NULL);
    }
    if (result != JNI_OK) {
        return NULL;
    }
    return env;
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