package tel.schich.libdatachannel;

import org.eclipse.jdt.annotation.Nullable;

import java.nio.ByteBuffer;

class LibDataChannelNative {
    static {
        LibDataChannel.initialize();
    }

    static native long rtcCreatePeerConnection(String @Nullable [] iceServers, @Nullable String proxyServer, @Nullable String bindAddress, int certificateType, int iceTransportPolicy, boolean enableIceTcp, boolean enableIceUdpMux, boolean disableAutoNegotiation, boolean forceMediaTransport, short portRangeBegin, short portRangeEnd, int mtu, int maxMessageSize);
    static native int setupPeerConnectionListener(long peerHandle, PeerConnectionListener listener);
    static native int rtcClosePeerConnection(long peerHandle);
    static native void rtcDeletePeerConnection(long peerHandle);

    static native void rtcSetLocalDescription(long peerHandle, @Nullable String type);
    static native String rtcGetLocalDescription(long peerHandle);
    static native String rtcGetLocalDescriptionType(long peerHandle);
    static native void rtcSetRemoteDescription(long peerHandle, String sdp, @Nullable String type);
    static native String rtcGetRemoteDescription(long peerHandle);
    static native String rtcGetRemoteDescriptionType(long peerHandle);
    static native void rtcAddRemoteCandidate(long peerHandle, String candidate, @Nullable String mediaId);
    static native String rtcGetLocalAddress(long peerHandle);
    static native String rtcGetRemoteAddress(long peerHandle);
    static native CandidatePair rtcGetSelectedCandidatePair(long peerHandle);
    static native void rtcSetLocalDescriptionCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetLocalCandidateCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetStateChangeCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetIceStateChangeCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetGatheringStateChangeCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetSignalingStateChangeCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetDataChannelCallback(long peerHandle, PeerConnectionListener listener, boolean set);
    static native void rtcSetTrackCallback(long peerHandle, PeerConnectionListener listener, boolean set);

    static native int rtcAddTrack(long peerHandle, String sdp);
    static native int rtcAddTrackEx(long peerHandle, int direction, int codec);
    static native String rtcGetTrackDescription(long trackHandle);
    static native int rtcGetTrackDirection(long trackHandle);
    static native String rtcGetTrackMid(long trackHandle);
    static native void rtcDeleteTrack(long trackHandle);

    static native int rtcGetMaxDataChannelStream(long peerHandle);
    static native int rtcGetRemoteMaxMessageSize(long peerHandle);
    static native long rtcCreateDataChannelEx(long peerHandle, String label, boolean unordered, boolean unreliable, long maxPacketLifeTime, int maxRetransmits, @Nullable String protocol, boolean negotiated, int stream, boolean manualStream);
    static native int rtcClose(long channelHandle);
    static native void rtcDeleteDataChannel(long channelHandle);
    static native boolean rtcIsClosed(long channelHandle);
    static native boolean rtcIsOpen(long channelHandle);
    static native int rtcMaxMessageSize(long channelHandle);
    static native int rtcSetBufferedAmountLowThreshold(long channelHandle, int amount);
    static native int rtcSendMessage(long channelHandle, ByteBuffer data, int offset, int length);
    @Nullable
    static native ByteBuffer rtcReceiveMessage(long channelHandle);
    static native int rtcReceiveMessageInto(long channelHandle, ByteBuffer buffer, int offset, int capacity);
    static native int rtcGetAvailableAmount(long channelHandle);
    static native int rtcGetBufferedAmount(long channelHandle);
    static native int rtcGetDataChannelStream(long channelHandle);
    static native String rtcGetDataChannelLabel(long channelHandle);
    static native String rtcGetDataChannelProtocol(long channelHandle);
    static native DataChannelReliability rtcGetDataChannelReliability(long channelHandle);
    static native int rtcSetOpenCallback(long channelHandle, boolean set);
    static native int rtcSetClosedCallback(long channelHandle, boolean set);
    static native int rtcSetErrorCallback(long channelHandle, boolean set);
    static native int rtcSetMessageCallback(long channelHandle, boolean set);
    static native int rtcSetBufferedAmountLowCallback(long channelHandle, boolean set);
    static native int rtcSetAvailableCallback(long channelHandle, boolean set);
}
