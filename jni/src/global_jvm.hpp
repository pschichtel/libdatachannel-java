#pragma once

#include <jni.h>

JavaVM* get_jvm_from_env(JNIEnv* env);
JNIEnv* get_jni_env_from_jvm(JavaVM* jvm);