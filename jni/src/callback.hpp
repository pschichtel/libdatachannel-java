#ifndef LIBDATACHANNEL_JNI_CALLBACK_H
#define LIBDATACHANNEL_JNI_CALLBACK_H

#include <jni.h>

struct jvm_callback {
    JavaVM* vm;
    jobject instance;
};

#ifdef __cplusplus
extern "C" {
#endif

struct jvm_callback* allocate_callback(JNIEnv* env, jobject callback);

void free_callback(JNIEnv* env, const struct jvm_callback* callback);

#ifdef __cplusplus
}
#endif

#endif//LIBDATACHANNEL_JNI_CALLBACK_H