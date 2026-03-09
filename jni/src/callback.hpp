#pragma once

#include <jni.h>

struct jvm_callback {
    JavaVM* vm;
    jobject instance;
};

#ifdef __cplusplus
extern "C" {
#endif

jvm_callback* allocate_callback(JNIEnv* env, jobject callback);

void free_callback(JNIEnv* env, const jvm_callback* callback);

#ifdef __cplusplus
}
#endif
