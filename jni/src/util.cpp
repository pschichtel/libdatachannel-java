#include "util.hpp"
#include <functional>
#include <jni-java-to-c.h>
#include <optional>
#include <string>

std::string util::getJavaString(JNIEnv* const env, const jstring s) {
    if (s == nullptr) {
        return "";
    }
    const auto chars = env->GetStringUTFChars(s, nullptr);
    const auto server = std::string(chars);
    env->ReleaseStringUTFChars(s, chars);
    return server;
}

jstring util::get_string_for_java(JNIEnv* const env, const std::function<std::optional<std::string>()>& func) {
    return wrapResult(env, [&]() -> jstring {
        const auto result = func();
        if (!result.has_value()) {
            return nullptr;
        }

        return env->NewStringUTF(result.value().c_str());
    });
}