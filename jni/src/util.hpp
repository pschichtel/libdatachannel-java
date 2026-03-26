#pragma once

#include "global_jvm.hpp"

#include <functional>
#include <jni-c-to-java.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#define EXCEPTION_THROWN (-999)

jint wrap_error(JNIEnv* env, const char* message, int result);

void throw_native_exception(JNIEnv* env, const char* msg);

#define THROW_FAILED_GET_STR(env, expr) throw_native_exception(env, "failed to get string for " #expr)

namespace util {
    std::string getJavaString(JNIEnv* env, jstring s);

    template <typename F> auto wrapResult(JNIEnv* env, F func) -> decltype(func()) {
        try {
            return func();
        } catch (const std::invalid_argument &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
            return nullptr;
        } catch (const std::exception &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
            return nullptr;
        }
    }

    template <typename F> void wrap(JNIEnv* env, F func) {
        try {
            func();
        } catch (const std::invalid_argument &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
        } catch (const std::exception &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
        }
    }

    template<typename Object>
    jlong handlize(JNIEnv* env, std::shared_ptr<Object> obj) {
        if (obj == nullptr) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Can't handlize null pointer!");
            return 0;
        }
        // this basically leaks memory, it will eventually be freed through Java's cleaner.
        return reinterpret_cast<jlong>(new std::shared_ptr<Object>(obj));
    }

    template<typename Object>
    std::shared_ptr<Object> dehandlize(JNIEnv* env, jlong handle) {
        if (handle == 0) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Can't dehandlize null pointer!");
            return nullptr;
        }
        return *reinterpret_cast<std::shared_ptr<Object>*>(handle);
    }

    template<typename Object>
    void delete_handle(JNIEnv* env, jlong handle) {
        if (handle == 0) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Can't delete null pointer!");
            return;
        }
        delete reinterpret_cast<std::shared_ptr<Object>*>(handle);
    }

    jstring get_string_for_java(JNIEnv* env, const std::function<std::optional<std::string>()>& func);

    template <typename Object, typename CallbackSignature, typename Bridge>
    void setup_rtc_callback(
        JNIEnv* env,
        jlong handle,
        jobject listener,
        jboolean set,
        std::function<void(Object*, std::function<CallbackSignature>)> setter,
        Bridge bridge
    ) {
        const std::shared_ptr<Object> object_ref = dehandlize<Object>(env, handle);

        if (set) {
            auto vm = get_jvm_from_env(env);
            if (vm == nullptr) {
                throw_tel_schich_libdatachannel_exception_FailureException_cstr(env, "Failed to get JavaVM reference!");
                return;
            }

            std::shared_ptr<_jobject> listener_ref(
                env->NewGlobalRef(listener),
                [vm](jobject obj) {
                    const auto local_env = get_jni_env_from_jvm(vm);
                    if (local_env != nullptr) {
                        local_env->DeleteGlobalRef(obj);
                    }
                }
            );
            setter(object_ref.get(), [vm, listener_ref, bridge](auto... args) {
                const auto local_env = get_jni_env_from_jvm(vm);
                if (local_env != nullptr) {
                    bridge(local_env, listener_ref.get(), args...);
                }
            });
        } else {
            setter(object_ref.get(), nullptr);
        }
    }
}