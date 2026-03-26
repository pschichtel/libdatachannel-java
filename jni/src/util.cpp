#include "util.hpp"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <jni-c-to-java.h>
#include <jni-java-to-c.h>
#include <optional>
#include <string>

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

std::string util::getJavaString(JNIEnv* env, jstring s) {
    if (s == nullptr) {
        return "";
    }
    const auto chars = env->GetStringUTFChars(s, nullptr);
    const auto server = std::string(chars);
    env->ReleaseStringUTFChars(s, chars);
    return server;
}

jstring util::get_string_for_java(JNIEnv* env, const std::function<std::optional<std::string>()>& func) {
    return wrapResult(env, [&]() -> jstring {
        const auto result = func();
        if (!result.has_value()) {
            return nullptr;
        }

        return env->NewStringUTF(result.value().c_str());
    });
}