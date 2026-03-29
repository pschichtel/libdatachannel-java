#pragma once

#include <functional>
#include <jni-c-to-java.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace util {
    constexpr jint JNI_VERSION = JNI_VERSION_1_6;
    constexpr int EXCEPTION_THROWN = -999;
    std::string getJavaString(JNIEnv* env, jstring s);

    JavaVM* get_jvm_from_env(JNIEnv* env);
    JNIEnv* get_jni_env_from_jvm(JavaVM* vm);

    template <typename F> auto wrapResult(JNIEnv* const env, const F func) -> decltype(func()) {
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

    template <typename F> void wrap(JNIEnv* const env, const F func) {
        try {
            func();
        } catch (const std::invalid_argument &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
        } catch (const std::exception &e) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, e.what());
        }
    }

    template<typename Object>
    jlong handlize(JNIEnv* const env, const std::shared_ptr<Object> obj) {
        if (obj == nullptr) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Can't handlize null pointer!");
            return 0;
        }
        // this basically leaks memory, it will eventually be freed through Java's cleaner.
        return reinterpret_cast<jlong>(new std::shared_ptr<Object>(obj));
    }

    template<typename Object>
    std::shared_ptr<Object> dehandlize(JNIEnv* const env, const jlong handle) {
        if (handle == 0) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Can't dehandlize null pointer!");
            return nullptr;
        }
        return *reinterpret_cast<std::shared_ptr<Object>*>(handle);
    }

    template<typename Object>
    void delete_handle(JNIEnv* const env, const jlong handle) {
        if (handle == 0) {
            throw_tel_schich_libdatachannel_exception_InvalidException_cstr(env, "Can't delete null pointer!");
            return;
        }
        delete reinterpret_cast<std::shared_ptr<Object>*>(handle);
    }

    jstring get_string_for_java(JNIEnv* env, const std::function<std::optional<std::string>()>& func);

    template <typename Object, typename CallbackSignature, typename Bridge>
    void setup_callback(
        JNIEnv* env,
        const jlong handle,
        const jobject target,
        const jboolean set,
        std::function<void(Object*, std::function<CallbackSignature>)> setter,
        Bridge bridge
    ) {
        const std::shared_ptr<Object> object_ref = dehandlize<Object>(env, handle);

        if (set) {
            auto vm = get_jvm_from_env(env);
            if (vm == nullptr) [[unlikely]] {
                throw_tel_schich_libdatachannel_exception_FailureException_cstr(env, "Failed to get JavaVM reference!");
                return;
            }

            std::shared_ptr<_jobject> target_ref(
                env->NewGlobalRef(target),
                [vm](const jobject obj) {
                    if (const auto local_env = get_jni_env_from_jvm(vm); local_env != nullptr) {
                        local_env->DeleteGlobalRef(obj);
                    }
                }
            );
            setter(object_ref.get(), [vm, target_ref, bridge](auto... args) {
                if (const auto local_env = get_jni_env_from_jvm(vm); local_env != nullptr) {
                    bridge(local_env, target_ref.get(), args...);
                }
            });
        } else {
            setter(object_ref.get(), nullptr);
        }
    }
}