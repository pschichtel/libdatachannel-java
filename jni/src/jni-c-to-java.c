#include "jni-c-to-java.h"
#include "bindings_mng.h"
#include <string.h>  
#include "util.h"


BIND_CLASS(tel_schich_libdatachannel_LibDataChannel);
BIND_CLASS(tel_schich_libdatachannel_PeerConnectionListener);
BIND_CLASS(tel_schich_libdatachannel_CandidatePair);
BIND_CLASS(tel_schich_libdatachannel_DataChannelReliability);
BIND_CLASS(tel_schich_libdatachannel_exception_TooSmallException);
BIND_CLASS(tel_schich_libdatachannel_exception_UnknownFailureException);
BIND_CLASS(tel_schich_libdatachannel_exception_FailureException);
BIND_CLASS(tel_schich_libdatachannel_exception_NotAvailableException);
BIND_CLASS(tel_schich_libdatachannel_exception_NativeOperationException);
BIND_CLASS(tel_schich_libdatachannel_exception_InvalidException);

// Initialize method bindings for LibDataChannel
BIND_STATIC_METHOD(tel_schich_libdatachannel_LibDataChannel, registerForCleanup, "(Ljava/lang/Object;J)V");
BIND_STATIC_METHOD(tel_schich_libdatachannel_LibDataChannel, log, "(ILjava/lang/String;)V");

// Initialize method bindings for DataChannelReliability constructor
BIND_METHOD(tel_schich_libdatachannel_DataChannelReliability, constructor, "(ZZJI)V");

// Initialize method bindings for exception constructors
BIND_METHOD(tel_schich_libdatachannel_exception_TooSmallException, constructor, "(Ljava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_exception_UnknownFailureException, constructor, "(ILjava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_exception_FailureException, constructor, "(Ljava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_exception_NotAvailableException, constructor, "(Ljava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_exception_NativeOperationException, constructor, "(Ljava/lang/String;ILjava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_exception_InvalidException, constructor, "(Ljava/lang/String;)V");

// Initialize method bindings for PeerConnectionListener
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onLocalDescription, "(Ljava/lang/String;Ljava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onLocalCandidate, "(Ljava/lang/String;Ljava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onStateChange, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onIceStateChange, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onGatheringStateChange, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onSignalingStateChange, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onDataChannel, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onTrack, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelOpen, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelClosed, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelError, "(ILjava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelTextMessage, "(ILjava/lang/String;)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelBinaryMessage, "(ILjava/nio/ByteBuffer;)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelBufferedAmountLow, "(I)V");
BIND_METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelAvailable, "(I)V");

// Initialize method binding for CandidatePair
BIND_STATIC_METHOD(tel_schich_libdatachannel_CandidatePair, parse, "(Ljava/lang/String;Ljava/lang/String;)Ltel/schich/libdatachannel/CandidatePair;");

#define _init(env) \
    LOAD_CLASS(env,tel_schich_libdatachannel_LibDataChannel); \
    LOAD_CLASS(env,tel_schich_libdatachannel_PeerConnectionListener); \
    LOAD_CLASS(env,tel_schich_libdatachannel_CandidatePair); \
    LOAD_CLASS(env,tel_schich_libdatachannel_DataChannelReliability); \
    LOAD_CLASS(env,tel_schich_libdatachannel_exception_TooSmallException); \
    LOAD_CLASS(env,tel_schich_libdatachannel_exception_UnknownFailureException); \
    LOAD_CLASS(env,tel_schich_libdatachannel_exception_FailureException); \
    LOAD_CLASS(env,tel_schich_libdatachannel_exception_NotAvailableException); \
    LOAD_CLASS(env,tel_schich_libdatachannel_exception_NativeOperationException); \
    LOAD_CLASS(env,tel_schich_libdatachannel_exception_InvalidException); \
    \
    LOAD_METHOD(env,tel_schich_libdatachannel_LibDataChannel, registerForCleanup); \
    LOAD_METHOD(env,tel_schich_libdatachannel_LibDataChannel, log); \
    \
    LOAD_METHOD(env,tel_schich_libdatachannel_DataChannelReliability, constructor); \
    \
    LOAD_METHOD(env,tel_schich_libdatachannel_exception_TooSmallException, constructor); \
    LOAD_METHOD(env,tel_schich_libdatachannel_exception_UnknownFailureException, constructor); \
    LOAD_METHOD(env,tel_schich_libdatachannel_exception_FailureException, constructor); \
    LOAD_METHOD(env,tel_schich_libdatachannel_exception_NotAvailableException, constructor); \
    LOAD_METHOD(env,tel_schich_libdatachannel_exception_NativeOperationException, constructor); \
    LOAD_METHOD(env,tel_schich_libdatachannel_exception_InvalidException, constructor); \
    \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onLocalDescription); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onLocalCandidate); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onStateChange); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onIceStateChange); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onGatheringStateChange); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onSignalingStateChange); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onDataChannel); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onTrack); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelOpen); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelClosed); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelError); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelTextMessage); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelBinaryMessage); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelBufferedAmountLow); \
    LOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelAvailable); \
    \
    LOAD_METHOD(env,tel_schich_libdatachannel_CandidatePair, parse);

#define _destroy(env) \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_CandidatePair, parse); \
    \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelAvailable); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelBufferedAmountLow); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelBinaryMessage); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelTextMessage); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelError); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelClosed); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onChannelOpen); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onTrack); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onDataChannel); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onSignalingStateChange); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onGatheringStateChange); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onIceStateChange); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onStateChange); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onLocalCandidate); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_PeerConnectionListener, onLocalDescription); \
    \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_exception_InvalidException, constructor); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_exception_NativeOperationException, constructor); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_exception_NotAvailableException, constructor); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_exception_FailureException, constructor); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_exception_UnknownFailureException, constructor); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_exception_TooSmallException, constructor); \
    \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_DataChannelReliability, constructor); \
    \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_LibDataChannel, log); \
    UNLOAD_METHOD(env,tel_schich_libdatachannel_LibDataChannel, registerForCleanup); \
    \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_exception_InvalidException); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_exception_NativeOperationException); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_exception_NotAvailableException); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_exception_FailureException); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_exception_UnknownFailureException); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_exception_TooSmallException); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_DataChannelReliability); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_CandidatePair); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_PeerConnectionListener); \
    UNLOAD_CLASS(env,tel_schich_libdatachannel_LibDataChannel);

void initializeBindings(JNIEnv *env){
    _init(env);
}
void destroyBindings(JNIEnv *env){
    _destroy(env);
}


void call_tel_schich_libdatachannel_LibDataChannel_log(JNIEnv *env, jint level, jstring message) {
    (*env)->CallStaticVoidMethod(
        env, 
        CLASS(tel_schich_libdatachannel_LibDataChannel), 
        STATIC_METHOD(tel_schich_libdatachannel_LibDataChannel, log), 
        level, 
        message
    );
}

void call_tel_schich_libdatachannel_LibDataChannel_log_cstr(JNIEnv *env, jint level, const char* c_message) {
    jstring message = (*env)->NewStringUTF(env, c_message);
    call_tel_schich_libdatachannel_LibDataChannel_log(env, level, message);
    (*env)->DeleteLocalRef(env, message);
}


void call_tel_schich_libdatachannel_LibDataChannel_registerForCleanup(
    JNIEnv *env, 
    jobject buffer, 
    jlong nativeAddress
) {
    (*env)->CallStaticVoidMethod(
        env, 
        CLASS(tel_schich_libdatachannel_LibDataChannel), 
        STATIC_METHOD(tel_schich_libdatachannel_LibDataChannel, registerForCleanup), 
        buffer, 
        nativeAddress
    );
}
// Exception throwing methods

void throw_tel_schich_libdatachannel_exception_TooSmallException(JNIEnv *env, jstring message) {
    jthrowable t = (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_exception_TooSmallException), 
        METHOD(tel_schich_libdatachannel_exception_TooSmallException, constructor), 
        message
    );
    (*env)->Throw(env, t);
}

void throw_tel_schich_libdatachannel_exception_TooSmallException_cstr(JNIEnv *env, const char* c_message) {
    (*env)->ThrowNew(env, CLASS(tel_schich_libdatachannel_exception_TooSmallException), c_message);
}

void throw_tel_schich_libdatachannel_exception_UnknownFailureException(JNIEnv *env, jint code, jstring message) {
    jthrowable t = (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_exception_UnknownFailureException), 
        METHOD(tel_schich_libdatachannel_exception_UnknownFailureException, constructor), 
        code, 
        message
    );
    (*env)->Throw(env, t);
}

void throw_tel_schich_libdatachannel_exception_UnknownFailureException_cstr(JNIEnv *env, jint code, const char* c_message) {
    jstring message = (*env)->NewStringUTF(env, c_message);
    throw_tel_schich_libdatachannel_exception_UnknownFailureException(env, code, message);
    (*env)->DeleteLocalRef(env, message);
}

void throw_tel_schich_libdatachannel_exception_FailureException(JNIEnv *env, jstring message) {
    jthrowable t = (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_exception_FailureException), 
        METHOD(tel_schich_libdatachannel_exception_FailureException, constructor), 
        message
    );
    (*env)->Throw(env, t);
}

void throw_tel_schich_libdatachannel_exception_FailureException_cstr(JNIEnv *env, const char* c_message) {
    (*env)->ThrowNew(env, CLASS(tel_schich_libdatachannel_exception_FailureException), c_message);
}

void throw_tel_schich_libdatachannel_exception_NotAvailableException(JNIEnv *env, jstring message) {
    jthrowable t = (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_exception_NotAvailableException), 
        METHOD(tel_schich_libdatachannel_exception_NotAvailableException, constructor), 
        message
    );
    (*env)->Throw(env, t);
}

void throw_tel_schich_libdatachannel_exception_NotAvailableException_cstr(JNIEnv *env, const char* c_message) {
    (*env)->ThrowNew(env, CLASS(tel_schich_libdatachannel_exception_NotAvailableException), c_message);
}

void throw_tel_schich_libdatachannel_exception_NativeOperationException(
    JNIEnv *env, 
    jstring message, 
    jint errorNumber, 
    jstring strerror
) {
    jthrowable t = (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_exception_NativeOperationException), 
        METHOD(tel_schich_libdatachannel_exception_NativeOperationException, constructor), 
        message, 
        errorNumber, 
        strerror
    );
    (*env)->Throw(env, t);
}

void throw_tel_schich_libdatachannel_exception_NativeOperationException_cstr(
    JNIEnv *env, 
    const char* c_message, 
    jint errorNumber, 
    const char* c_strerror
) {
    jstring message = (*env)->NewStringUTF(env, c_message);
    jstring strerror = (*env)->NewStringUTF(env, c_strerror);
    throw_tel_schich_libdatachannel_exception_NativeOperationException(env, message, errorNumber, strerror);
    (*env)->DeleteLocalRef(env, message);
    (*env)->DeleteLocalRef(env, strerror);
}

void throw_tel_schich_libdatachannel_exception_InvalidException(JNIEnv *env, jstring message) {
    jthrowable t = (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_exception_InvalidException), 
        METHOD(tel_schich_libdatachannel_exception_InvalidException, constructor), 
        message
    );
    (*env)->Throw(env, t);
}

void throw_tel_schich_libdatachannel_exception_InvalidException_cstr(JNIEnv *env, const char* c_message) {
    (*env)->ThrowNew(env, CLASS(tel_schich_libdatachannel_exception_InvalidException), c_message);
}

// Object creation methods

jobject create_tel_schich_libdatachannel_DataChannelReliability(
    JNIEnv *env, 
    jboolean unordered, 
    jboolean unreliable, 
    jlong maxPacketLifeTime, 
    jint maxRetransmits
) {
    return (*env)->NewObject(
        env, 
        CLASS(tel_schich_libdatachannel_DataChannelReliability), 
        METHOD(tel_schich_libdatachannel_DataChannelReliability, constructor), 
        unordered, 
        unreliable, 
        maxPacketLifeTime, 
        maxRetransmits
    );
}

// PeerConnectionListener methods

void call_tel_schich_libdatachannel_PeerConnectionListener_onLocalDescription(
    JNIEnv *env, 
    jobject instance, 
    jstring sdp, 
    jstring type
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onLocalDescription), 
        sdp, 
        type
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onLocalDescription_cstr(
    JNIEnv *env, 
    jobject instance, 
    const char* c_sdp, 
    const char* c_type
) {
    jstring sdp = (*env)->NewStringUTF(env, c_sdp);
    jstring type = (*env)->NewStringUTF(env, c_type);
    call_tel_schich_libdatachannel_PeerConnectionListener_onLocalDescription(env, instance, sdp, type);
    (*env)->DeleteLocalRef(env, sdp);
    (*env)->DeleteLocalRef(env, type);
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onLocalCandidate(
    JNIEnv *env, 
    jobject instance, 
    jstring candidate, 
    jstring mediaId
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onLocalCandidate), 
        candidate, 
        mediaId
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onLocalCandidate_cstr(
    JNIEnv *env, 
    jobject instance, 
    const char* c_candidate, 
    const char* c_mediaId
) {
    jstring candidate = (*env)->NewStringUTF(env, c_candidate);
    jstring mediaId = (*env)->NewStringUTF(env, c_mediaId);
    call_tel_schich_libdatachannel_PeerConnectionListener_onLocalCandidate(env, instance, candidate, mediaId);
    (*env)->DeleteLocalRef(env, candidate);
    (*env)->DeleteLocalRef(env, mediaId);
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onStateChange(
    JNIEnv *env, 
    jobject instance, 
    jint state
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onStateChange), 
        state
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onIceStateChange(
    JNIEnv *env, 
    jobject instance, 
    jint iceState
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onIceStateChange), 
        iceState
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onGatheringStateChange(
    JNIEnv *env, 
    jobject instance, 
    jint gatheringState
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onGatheringStateChange), 
        gatheringState
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onSignalingStateChange(
    JNIEnv *env, 
    jobject instance, 
    jint signalingState
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onSignalingStateChange), 
        signalingState
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onDataChannel(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onDataChannel), 
        channelHandle
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onTrack(
    JNIEnv *env, 
    jobject instance, 
    jint trackHandle
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onTrack), 
        trackHandle
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelOpen(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelOpen), 
        channelHandle
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelClosed(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelClosed), 
        channelHandle
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelError(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle, 
    jstring error
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelError), 
        channelHandle, 
        error
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelError_cstr(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle, 
    const char* c_error
) {
    jstring error = (*env)->NewStringUTF(env, c_error);
    call_tel_schich_libdatachannel_PeerConnectionListener_onChannelError(env, instance, channelHandle, error);
    (*env)->DeleteLocalRef(env, error);
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelTextMessage(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle, 
    jstring message
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelTextMessage), 
        channelHandle, 
        message
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelTextMessage_cstr(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle, 
    const char* c_message
) {
    jstring message = (*env)->NewStringUTF(env, c_message);
    call_tel_schich_libdatachannel_PeerConnectionListener_onChannelTextMessage(env, instance, channelHandle, message);
    (*env)->DeleteLocalRef(env, message);
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelBinaryMessage(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle, 
    jobject message
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelBinaryMessage), 
        channelHandle, 
        message
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelBufferedAmountLow(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelBufferedAmountLow), 
        channelHandle
    );
}

void call_tel_schich_libdatachannel_PeerConnectionListener_onChannelAvailable(
    JNIEnv *env, 
    jobject instance, 
    jint channelHandle
) {
    (*env)->CallVoidMethod(
        env, 
        instance, 
        METHOD(tel_schich_libdatachannel_PeerConnectionListener, onChannelAvailable), 
        channelHandle
    );
}

jobject call_tel_schich_libdatachannel_CandidatePair_parse(
    JNIEnv *env, 
    jstring local, 
    jstring remote
) {
    return (*env)->CallStaticObjectMethod(
        env, 
        CLASS(tel_schich_libdatachannel_CandidatePair), 
        STATIC_METHOD(tel_schich_libdatachannel_CandidatePair, parse), 
        local, 
        remote
    );
}

jobject call_tel_schich_libdatachannel_CandidatePair_parse_cstr(
    JNIEnv *env, 
    const char* c_local, 
    const char* c_remote
) {
    jstring local = (*env)->NewStringUTF(env, c_local);
    jstring remote = (*env)->NewStringUTF(env, c_remote);
    jobject result = call_tel_schich_libdatachannel_CandidatePair_parse(env, local, remote);
    (*env)->DeleteLocalRef(env, local);
    (*env)->DeleteLocalRef(env, remote);
    return result;
}