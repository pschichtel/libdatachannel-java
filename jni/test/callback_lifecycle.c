#include "callback_lifecycle.h"

#include <errno.h>
#include <jni.h>
#include <pthread.h>
#include <time.h>

static pthread_mutex_t callback_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t callback_condition = PTHREAD_COND_INITIALIZER;
static bool callback_armed;
static bool callback_entered;
static bool callback_released;
static bool callback_should_dispatch;

static struct timespec deadline_after_millis(jlong timeout_millis) {
    struct timespec deadline;
    timespec_get(&deadline, TIME_UTC);
    deadline.tv_sec += timeout_millis / 1000;
    deadline.tv_nsec += (timeout_millis % 1000) * 1000000;
    if (deadline.tv_nsec >= 1000000000) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000;
    }
    return deadline;
}

bool wait_for_signaling_state_callback_test(void) {
    pthread_mutex_lock(&callback_mutex);
    if (!callback_armed) {
        pthread_mutex_unlock(&callback_mutex);
        return true;
    }

    callback_entered = true;
    pthread_cond_broadcast(&callback_condition);
    while (!callback_released) {
        pthread_cond_wait(&callback_condition, &callback_mutex);
    }

    bool should_dispatch = callback_should_dispatch;
    callback_armed = false;
    callback_entered = false;
    callback_released = false;
    callback_should_dispatch = false;
    pthread_mutex_unlock(&callback_mutex);
    return should_dispatch;
}

JNIEXPORT void JNICALL
Java_tel_schich_libdatachannel_PeerCallbackLifecycleTest_armSignalingStateCallback(JNIEnv* env, jclass clazz) {
    pthread_mutex_lock(&callback_mutex);
    callback_armed = true;
    callback_entered = false;
    callback_released = false;
    callback_should_dispatch = false;
    pthread_mutex_unlock(&callback_mutex);
}

JNIEXPORT jboolean JNICALL
Java_tel_schich_libdatachannel_PeerCallbackLifecycleTest_awaitSignalingStateCallback(JNIEnv* env, jclass clazz,
                                                                                    jlong timeout_millis) {
    struct timespec deadline = deadline_after_millis(timeout_millis);
    pthread_mutex_lock(&callback_mutex);
    while (!callback_entered) {
        int result = pthread_cond_timedwait(&callback_condition, &callback_mutex, &deadline);
        if (result == ETIMEDOUT) {
            pthread_mutex_unlock(&callback_mutex);
            return JNI_FALSE;
        }
    }
    pthread_mutex_unlock(&callback_mutex);
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_tel_schich_libdatachannel_PeerCallbackLifecycleTest_releaseSignalingStateCallback(JNIEnv* env, jclass clazz,
                                                                                      jboolean dispatch) {
    pthread_mutex_lock(&callback_mutex);
    callback_should_dispatch = dispatch == JNI_TRUE;
    callback_released = true;
    pthread_cond_broadcast(&callback_condition);
    pthread_mutex_unlock(&callback_mutex);
}
