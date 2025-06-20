#ifndef LIBDATACHANNEL_JNI_CALLBACK_H
#define LIBDATACHANNEL_JNI_CALLBACK_H

#include <jni.h>

struct jvm_callback {
    JavaVM* vm;
    jobject instance;
};

struct jvm_callback* allocate_callback(JNIEnv* env, jobject callback);

void free_callback(JNIEnv* env, struct jvm_callback* callback);

#endif//LIBDATACHANNEL_JNI_CALLBACK_H