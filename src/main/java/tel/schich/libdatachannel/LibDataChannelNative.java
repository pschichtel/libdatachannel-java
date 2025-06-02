package tel.schich.libdatachannel;

import org.eclipse.jdt.annotation.Nullable;

import java.nio.ByteBuffer;

class LibDataChannelNative {
    static {
        LibDataChannel.initialize();
    }

    static native int rtcCreatePeerConnection(String @Nullable [] iceServers, @Nullable String proxyServer, @Nullable String bindAddress, int certificateType, int iceTransportPolicy, boolean enableIceTcp, boolean enableIceUdpMux, boolean disableAutoNegotiation, boolean forceMediaTransport, short portRangeBegin, short portRangeEnd, int mtu, int maxMessageSize);
    static native int setupPeerConnectionListener(int peerHandle, PeerConnectionListener listener);
    static native int rtcClosePeerConnection(int peerHandle);
    static native int rtcDeletePeerConnection(int peerHandle);

    static native int rtcSetLocalDescription(int peerHandle, String type);
    static native String rtcGetLocalDescription(int peerHandle);
    static native String rtcGetLocalDescriptionType(int peerHandle);
    static native int rtcSetRemoteDescription(int peerHandle, String sdp, @Nullable String type);
    static native String rtcGetRemoteDescription(int peerHandle);
    static native String rtcGetRemoteDescriptionType(int peerHandle);
    static native int rtcAddRemoteCandidate(int peerHandle, String candidate, @Nullable String mediaId);
    static native String rtcGetLocalAddress(int peerHandle);
    static native String rtcGetRemoteAddress(int peerHandle);
    static native CandidatePair rtcGetSelectedCandidatePair(int peerHandle);
    static native int rtcSetLocalDescriptionCallback(int peerHandle, boolean set);
    static native int rtcSetLocalCandidateCallback(int peerHandle, boolean set);
    static native int rtcSetStateChangeCallback(int peerHandle, boolean set);
    static native int rtcSetIceStateChangeCallback(int peerHandle, boolean set);
    static native int rtcSetGatheringStateChangeCallback(int peerHandle, boolean set);
    static native int rtcSetSignalingStateChangeCallback(int peerHandle, boolean set);
    static native int rtcSetDataChannelCallback(int peerHandle, boolean set);
    static native int rtcSetTrackCallback(int peerHandle, boolean set);

    static native int rtcAddTrack(int peerHandle, String sdp);
    static native int rtcAddTrackEx(int peerHandle, int direction, int codec);
    static native String rtcGetTrackDescription(int trackHandle);
    static native int rtcGetTrackDirection(int trackHandle);
    static native String rtcGetTrackMid(int trackHandle);
    static native int rtcDeleteTrack(int trackHandle);

    static native int rtcGetMaxDataChannelStream(int peerHandle);
    static native int rtcGetRemoteMaxMessageSize(int peerHandle);
    static native int rtcCreateDataChannelEx(int peerHandle, String label, boolean unordered, boolean unreliable, long maxPacketLifeTime, int maxRetransmits, @Nullable String protocol, boolean negotiated, int stream, boolean manualStream);
    static native int rtcClose(int channelHandle);
    static native int rtcDeleteDataChannel(int channelHandle);
    static native boolean rtcIsClosed(int channelHandle);
    static native boolean rtcIsOpen(int channelHandle);
    static native int rtcMaxMessageSize(int channelHandle);
    static native int rtcSetBufferedAmountLowThreshold(int channelHandle, int amount);
    static native int rtcSendMessage(int channelHandle, ByteBuffer data, int offset, int length);
    @Nullable
    static native ByteBuffer rtcReceiveMessage(int channelHandle);
    static native int rtcReceiveMessageInto(int channelHandle, ByteBuffer buffer, int offset, int capacity);
    static native int rtcGetAvailableAmount(int channelHandle);
    static native int rtcGetBufferedAmount(int channelHandle);
    static native int rtcGetDataChannelStream(int channelHandle);
    static native String rtcGetDataChannelLabel(int channelHandle);
    static native String rtcGetDataChannelProtocol(int channelHandle);
    static native DataChannelReliability rtcGetDataChannelReliability(int channelHandle);
    static native int rtcSetOpenCallback(int channelHandle, boolean set);
    static native int rtcSetClosedCallback(int channelHandle, boolean set);
    static native int rtcSetErrorCallback(int channelHandle, boolean set);
    static native int rtcSetMessageCallback(int channelHandle, boolean set);
    static native int rtcSetBufferedAmountLowCallback(int channelHandle, boolean set);
    static native int rtcSetAvailableCallback(int channelHandle, boolean set);
    static native void rtcFree(long address);
}
