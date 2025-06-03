#include "callback.h"
#include "util.h"
#include "global_jvm.h"
#include <stdlib.h>

struct jvm_callback* allocate_callback(JNIEnv* env, jobject callback) {
    struct jvm_callback* cb = malloc(sizeof(struct jvm_callback));
    if (cb == NULL) {
        THROW_FAILED_MALLOC(env, cb);
        return NULL;
    }
    (*env)->GetJavaVM(env, &cb->vm);
    cb->instance = (*env)->NewGlobalRef(env, callback);
    return cb;
}

void free_callback(JNIEnv* env, struct jvm_callback* callback) {
    (*env)->DeleteGlobalRef(env, callback->instance);
    free(callback);
}