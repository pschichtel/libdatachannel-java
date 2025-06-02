#include "util.h"
#include <jni-c-to-java.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <jni-java-to-c.h>

jstring get_dynamic_string(JNIEnv *env, const char* func_name, get_dynamic_string_func func, int handle) {
    int size = wrap_error(env, "", func(handle, NULL, -1));
    char* memory = malloc(size);
    if (memory == NULL) {
        throw_native_exception(env, "Failed to allocate memory for string");
        return NULL;
    }
    wrap_error(env, func_name, func(handle, memory, size));
    jstring result = (*env)->NewStringUTF(env, memory);
    free(memory);
    return result;
}

jint wrap_error(JNIEnv* env, const char* message, int result) {
    if (result > 0) {
        return result;
    }
    switch (result) {
        case RTC_ERR_SUCCESS:
            return RTC_ERR_SUCCESS;
        case RTC_ERR_INVALID:
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, message);
            return EXCEPTION_THROWN;
        case RTC_ERR_FAILURE:
            throw_tel_schich_libdatachannel_exception_FailureException_cstr(env, message);
            return EXCEPTION_THROWN;
        case RTC_ERR_NOT_AVAIL:
            throw_tel_schich_libdatachannel_exception_NotAvailableException_cstr(env, message);
            return EXCEPTION_THROWN;
        case RTC_ERR_TOO_SMALL:
            throw_tel_schich_libdatachannel_exception_TooSmallException_cstr(env, message);
            return EXCEPTION_THROWN;
        default:
            throw_tel_schich_libdatachannel_exception_UnknownFailureException_cstr(env, result, message);
            return EXCEPTION_THROWN;
    }
}

void throw_native_exception(JNIEnv *env, char *msg) {
    // It is necessary to get the errno before any Java or JNI function is called, as it
    // may become changed due to the VM operations.
    int errorNumber = errno;

    throw_tel_schich_libdatachannel_exception_NativeOperationException_cstr(env, msg, errorNumber, strerror(errorNumber));
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannelNative_rtcFree(JNIEnv *env, jclass clazz, jlong address) {
    void* ptr = (void*)(intptr_t)address;
    free(ptr);
}