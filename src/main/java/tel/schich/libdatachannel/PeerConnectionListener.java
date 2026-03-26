package tel.schich.libdatachannel;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tel.schich.jniaccess.JNIAccess;

import java.nio.ByteBuffer;
import java.util.function.BiConsumer;
import java.util.function.Function;

class PeerConnectionListener {
    private static final Logger LOGGER = LoggerFactory.getLogger(PeerConnectionListener.class);

    private final PeerConnection peer;

    public PeerConnectionListener(final PeerConnection peer) {
        this.peer = peer;
    }

    @JNIAccess
    void onLocalDescription(String sdp, String type) {
        final SessionDescriptionType mappedType = SessionDescriptionType.of(type);
        if (mappedType == null) {
            LOGGER.error("Unknown SDP type {}!", type);
            return;
        }
        peer.onLocalDescription.invoke(h -> h.handleDescription(peer, sdp, mappedType));
    }

    @JNIAccess
    void onLocalCandidate(String candidate, String mediaId) {
        peer.onLocalCandidate.invoke(h -> h.handleCandidate(peer, candidate, mediaId));
    }

    @JNIAccess
    void onStateChange(int state) {
        final PeerState mappedState = PeerState.of(state);
        if (mappedState == null) {
            LOGGER.error("Unknown state {}!", state);
            return;
        }
        peer.onStateChange.invoke(h -> h.handleChange(peer, mappedState));
    }

    @JNIAccess
    void onIceStateChange(int iceState) {
        final IceState mappedState = IceState.of(iceState);
        if (mappedState == null) {
            LOGGER.error("Unknown ICE state {}!", iceState);
            return;
        }
        peer.onIceStateChange.invoke(h -> h.handleChange(peer, mappedState));
    }

    @JNIAccess
    void onGatheringStateChange(int gatheringState) {
        final GatheringState mappedState = GatheringState.of(gatheringState);
        if (mappedState == null) {
            LOGGER.error("Unknown gathering state {}!", gatheringState);
            return;
        }
        peer.onGatheringStateChange.invoke(h -> h.handleChange(peer, mappedState));
    }

    @JNIAccess
    void onSignalingStateChange(int signalingState) {
        final SignalingState mappedState = SignalingState.of(signalingState);
        if (mappedState == null) {
            LOGGER.error("Unknown signaling state {}!", signalingState);
            return;
        }
        peer.onSignalingStateChange.invoke(h -> h.handleChange(peer, mappedState));
    }

    @JNIAccess
    void onDataChannel(long channelHandle) {
        final DataChannel channel = peer.newChannel(channelHandle);
        peer.onDataChannel.invoke(h -> h.handleChannel(peer, channel));
    }

    @JNIAccess
    void onTrack(long trackHandle) {
        final Track state = peer.newTrack(trackHandle);
        peer.onTrack.invoke(h -> h.handleTrack(peer, state));
    }

    private <T> void invokeWithChannel(int handle, Function<DataChannel, EventListenerContainer<T>> listeners, BiConsumer<T, DataChannel> consumer) {
        final DataChannel channel = peer.channel(handle);
        if (channel == null) {
            LOGGER.warn("Received event for unknown data channel {}!", handle);
            return;
        }
        listeners.apply(channel).invoke(h -> consumer.accept(h, channel));
    }

    @JNIAccess
    void onChannelOpen(int channelHandle) {
        invokeWithChannel(channelHandle, s -> s.onOpen, DataChannelCallback.Open::onOpen);
    }

    @JNIAccess
    void onChannelClosed(int channelHandle) {
        invokeWithChannel(channelHandle, s -> s.onClosed, DataChannelCallback.Closed::onClosed);
    }

    @JNIAccess
    void onChannelError(int channelHandle, String error) {
        invokeWithChannel(channelHandle, s -> s.onError, (h, ch) -> h.onError(ch, error));
    }

    @JNIAccess
    void onChannelTextMessage(int channelHandle, String message) {
        invokeWithChannel(channelHandle, s -> s.onMessage, (h, ch) -> h.onText(ch, message));
    }

    @JNIAccess
    void onChannelBinaryMessage(int channelHandle, ByteBuffer message) {
        invokeWithChannel(channelHandle, s -> s.onMessage, (h, ch) -> h.onBinary(ch, message));
    }

    @JNIAccess
    void onChannelBufferedAmountLow(int channelHandle) {
        invokeWithChannel(channelHandle, s -> s.onBufferedAmountLow, DataChannelCallback.BufferedAmountLow::onBufferedAmountLow);
    }

    @JNIAccess
    void onChannelAvailable(int channelHandle) {
        invokeWithChannel(channelHandle, s -> s.onAvailable, DataChannelCallback.Available::onAvailable);
    }
}
