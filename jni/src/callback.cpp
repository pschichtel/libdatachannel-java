#include "callback.hpp"
#include "global_jvm.hpp"

jvm_callback* allocate_callback(JNIEnv* env, jobject callback) {
    const auto cb = new jvm_callback;
    env->GetJavaVM(&cb->vm);
    cb->instance = env->NewGlobalRef(callback);
    return cb;
}

void free_callback(JNIEnv* env, const jvm_callback* callback) {
    env->DeleteGlobalRef(callback->instance);
    delete callback;
}
