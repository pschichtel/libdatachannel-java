#ifndef LIBDATACHANNEL_JNI_BINDINGS_MNG_H
#define LIBDATACHANNEL_JNI_BINDINGS_MNG_H
#include <jni.h>
#include <stdlib.h>


#define BIND_CLASS(name) \
    static jclass CLASS_##name = NULL; \
    void CREATE_CLASS_##name(JNIEnv *env) { \
        const char* classname = #name; \
        char* cname = strdup(classname); \
        if (cname == NULL) { \
            throw_native_exception(env, "Memory allocation failed for class name"); \
            return; \
        } \
        for (char* p = cname; *p; p++) { \
            if (*p == '_') *p = '/'; \
        } \
        jclass class = (*env)->FindClass(env, cname); \
        free(cname); \
        if (class == NULL) { \
            throw_native_exception(env, "Failed to find class " #name); \
            return; \
        } \
        CLASS_##name = (*env)->NewGlobalRef(env, class); \
    } \
    void DESTROY_CLASS_##name(JNIEnv *env) { \
        if (CLASS_##name != NULL) { \
            (*env)->DeleteGlobalRef(env, CLASS_##name); \
            CLASS_##name = NULL; \
        } \
    } 

#define BIND_METHOD(class_name, method_name, signature) \
    static jmethodID METHOD_##class_name##_##method_name = NULL; \
    void CREATE_METHOD_##class_name##_##method_name(JNIEnv *env) { \
        jclass class = CLASS_##class_name; \
        if (class == NULL) { \
            return; \
        } \
        jmethodID method_id; \
        if (strcmp(#method_name, "constructor") == 0) { \
            method_id = (*env)->GetMethodID(env, class, "<init>", signature); \
        } else { \
            method_id = (*env)->GetMethodID(env, class, #method_name, signature); \
        } \
        METHOD_##class_name##_##method_name = method_id; \
        if (METHOD_##class_name##_##method_name == NULL) { \
            return; \
        } \
    } \
    void DESTROY_METHOD_##class_name##_##method_name(JNIEnv *env) { \
        METHOD_##class_name##_##method_name = NULL; \
    }

#define BIND_STATIC_METHOD(class_name, method_name, signature) \
    static jmethodID METHOD_##class_name##_##method_name = NULL; \
    void CREATE_METHOD_##class_name##_##method_name(JNIEnv *env) { \
        jclass class = CLASS_##class_name; \
        if (class == NULL) { \
            throw_native_exception(env, "Failed to find class " #class_name); \
            return; \
        } \
        if (strcmp(#method_name, "constructor") == 0) { \
            throw_native_exception(env, "Static methods cannot be constructors"); \
        }  \
        jmethodID method_id = (*env)->GetStaticMethodID(env, class, #method_name, signature); \
        METHOD_##class_name##_##method_name = method_id; \
        if (METHOD_##class_name##_##method_name == NULL) { \
            throw_native_exception(env, "Failed to find method " #method_name " in class " #class_name); \
            return; \
        } \
    } \
    void DESTROY_METHOD_##class_name##_##method_name(JNIEnv *env) { \
        METHOD_##class_name##_##method_name = NULL; \
    } 

    
#define METHOD(class_name, method_name) \
    METHOD_##class_name##_##method_name


#define STATIC_METHOD(class_name, method_name) \
    METHOD_##class_name##_##method_name

#define CLASS(class_name) \
    CLASS_##class_name

#define LOAD_CLASS(env, class_name) \
    CREATE_CLASS_##class_name(env)

#define LOAD_METHOD(env, class_name, method_name) \
    CREATE_METHOD_##class_name##_##method_name(env)

#define UNLOAD_CLASS(env, class_name) \
    DESTROY_CLASS_##class_name(env)

#define UNLOAD_METHOD(env, class_name, method_name) \
    DESTROY_METHOD_##class_name##_##method_name(env)


void initializeBindings(JNIEnv *env);
void destroyBindings(JNIEnv *env);
#endif
