#include "util.hpp"
#include <cerrno>
#include <jni-c-to-java.h>
#include <jni-java-to-c.h>
#include <cstdlib>
#include <cstring>

jstring get_dynamic_string(JNIEnv* env, const char* func_name, const get_dynamic_string_func func, const int handle) {
    const int size = wrap_error(env, "", func(handle, nullptr, -1));
    const auto memory = static_cast<char*>(malloc(size));
    if (memory == nullptr) {
        throw_native_exception(env, "Failed to allocate memory for string");
        return nullptr;
    }
    wrap_error(env, func_name, func(handle, memory, size));
    jstring result = env->NewStringUTF(memory);
    free(memory);
    return result;
}

jint wrap_error(JNIEnv* env, const char* message, const int result) {
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

void throw_native_exception(JNIEnv* env, const char* msg) {
    // It is necessary to get the errno before any Java or JNI function is called, as it
    // may become changed due to the VM operations.
    const int errorNumber = errno;

    throw_tel_schich_libdatachannel_exception_NativeOperationException_cstr(env, msg, errorNumber, strerror(errorNumber));
}

JNIEXPORT void JNICALL Java_tel_schich_libdatachannel_LibDataChannel_freeMemory(JNIEnv* env, jclass clazz, const jlong address) {
    const auto ptr = reinterpret_cast<void*>(static_cast<intptr_t>(address));
    free(ptr);
}