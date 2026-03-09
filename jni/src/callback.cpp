#include "callback.hpp"
#include "global_jvm.hpp"
#include <new>

jvm_callback* allocate_callback(JNIEnv* env, jobject callback) {
    try {
        const auto cb = new jvm_callback;
        env->GetJavaVM(&cb->vm);
        cb->instance = env->NewGlobalRef(callback);
        if (cb->instance == nullptr) {
            delete cb;
            return nullptr;
        }
        return cb;
    } catch (std::bad_alloc&) {
        return nullptr;
    }
}

void free_callback(JNIEnv* env, const jvm_callback* callback) {
    env->DeleteGlobalRef(callback->instance);
    delete callback;
}
