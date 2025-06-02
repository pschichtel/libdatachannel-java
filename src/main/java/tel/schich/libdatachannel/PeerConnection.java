package tel.schich.libdatachannel;

import org.eclipse.jdt.annotation.Nullable;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static tel.schich.libdatachannel.LibDataChannelNative.rtcAddRemoteCandidate;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcAddTrack;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcAddTrackEx;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcClosePeerConnection;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcCreateDataChannelEx;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcCreatePeerConnection;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcDeletePeerConnection;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetLocalAddress;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetLocalDescription;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetLocalDescriptionType;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetMaxDataChannelStream;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetRemoteAddress;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetRemoteDescription;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetRemoteDescriptionType;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetRemoteMaxMessageSize;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetSelectedCandidatePair;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetDataChannelCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetGatheringStateChangeCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetIceStateChangeCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetLocalCandidateCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetLocalDescription;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetLocalDescriptionCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetRemoteDescription;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetSignalingStateChangeCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetStateChangeCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetTrackCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.setupPeerConnectionListener;
import static tel.schich.libdatachannel.Util.parseAddress;
import static tel.schich.libdatachannel.Util.wrapError;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_INVALID;

import java.io.Closeable;
import java.lang.ref.Cleaner;
import java.net.InetSocketAddress;
import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.Executor;

public class PeerConnection implements Closeable {
    private static final Logger LOGGER = LoggerFactory.getLogger(PeerConnection.class);

    final int peerHandle;
    private final Executor executor;
    private final ConcurrentMap<Integer, DataChannel> channels;
    private final ConcurrentMap<Integer, Track> tracks;
    private final Cleaner.Cleanable cleanable;
    final PeerConnectionListener listener;

    public final EventListenerContainer<PeerConnectionCallback.LocalDescription> onLocalDescription;
    public final EventListenerContainer<PeerConnectionCallback.LocalCandidate> onLocalCandidate;
    public final EventListenerContainer<PeerConnectionCallback.StateChange> onStateChange;
    public final EventListenerContainer<PeerConnectionCallback.IceStateChange> onIceStateChange;
    public final EventListenerContainer<PeerConnectionCallback.GatheringStateChange> onGatheringStateChange;
    public final EventListenerContainer<PeerConnectionCallback.SignalingStateChange> onSignalingStateChange;
    public final EventListenerContainer<PeerConnectionCallback.DataChannel> onDataChannel;
    public final EventListenerContainer<PeerConnectionCallback.Track> onTrack;

    private PeerConnection(int peerHandle, final Executor executor) {
        this.peerHandle = peerHandle;
        this.executor = executor;
        this.channels = new ConcurrentHashMap<>();
        this.tracks = new ConcurrentHashMap<>();
        this.listener = new PeerConnectionListener(this);

        this.onLocalDescription = new EventListenerContainer<>("LocalDescription", set -> rtcSetLocalDescriptionCallback(peerHandle, set), executor);
        this.onLocalCandidate = new EventListenerContainer<>("LocalCandidate", set -> rtcSetLocalCandidateCallback(peerHandle, set), executor);
        this.onStateChange = new EventListenerContainer<>("StateChange", set -> rtcSetStateChangeCallback(peerHandle, set), executor);
        this.onIceStateChange = new EventListenerContainer<>("IceStateChange", set -> rtcSetIceStateChangeCallback(peerHandle, set), executor);
        this.onGatheringStateChange = new EventListenerContainer<>("GatheringStateChange", set -> rtcSetGatheringStateChangeCallback(peerHandle, set), executor);
        this.onSignalingStateChange = new EventListenerContainer<>("SignalingStateChange", set -> rtcSetSignalingStateChangeCallback(peerHandle, set), executor);
        this.onDataChannel = new EventListenerContainer<>("DataChannel", set -> rtcSetDataChannelCallback(peerHandle, set), executor);
        this.onTrack = new EventListenerContainer<>("Track", set -> rtcSetTrackCallback(peerHandle, set), executor);

        this.cleanable = LibDataChannel.CLEANER.register(this, () -> {
            // make sure not to capture this here, that would be a memory leak
            if (rtcClosePeerConnection(peerHandle) != ERR_INVALID) {
                rtcDeletePeerConnection(peerHandle);
            }
        });
    }

    private static String @Nullable [] iceUrisToStrings(@Nullable Collection<URI> uris) {
        if (uris == null || uris.isEmpty()) {
            return null;
        }
        int index = 0;
        String[] strings = new String[uris.size()];
        for (URI server : uris) {
            strings[index++] = server.toASCIIString();
        }
        return strings;
    }

    /**
     * Creates a Peer Connection.
     *
     * Remember to {@link #close()} when done.
     *
     * @param config the peer configuration
     * @param executor the executor to run callbacks on
     * @return the peer connection
     */
    public static PeerConnection createPeer(PeerConnectionConfiguration config, Executor executor) {
        String proxyServer = null;
        if (config.proxyServer != null) {
            proxyServer = config.proxyServer.toASCIIString();
        }
        String bindAddress = null;
        if (config.bindAddress != null) {
            bindAddress = config.bindAddress.toString();
        }
        int result = rtcCreatePeerConnection(
                iceUrisToStrings(config.iceServers),
                proxyServer,
                bindAddress,
                config.certificateType.state,
                config.iceTransportPolicy.state,
                config.enableIceTcp,
                config.enableIceUdpMux,
                config.disableAutoNegotiation,
                config.forceMediaTransport,
                config.portRangeBegin,
                config.portRangeEnd,
                config.mtu,
                config.maxMessageSize);

        final PeerConnection peer = new PeerConnection(wrapError("rtcCreatePeerConnection", result), executor);
        setupPeerConnectionListener(peer.peerHandle, peer.listener);

        return peer;
    }

    public static PeerConnection createPeer(PeerConnectionConfiguration config) {
        return createPeer(config, Runnable::run);
    }

    @Nullable
    DataChannel channel(int channelHandle) {
        return channels.get(channelHandle);
    }

    DataChannel newChannel(int channelHandle) {
        return channels.computeIfAbsent(channelHandle, h -> new DataChannel(this, h, executor));
    }

    void dropChannelState(int channelHandle) {
        channels.remove(channelHandle);
    }

    Track newTrack(int trackHandle) {
        return tracks.computeIfAbsent(trackHandle, h -> new Track(this, h));
    }

    public void dropTrackState(int trackHandle) {
        tracks.remove(trackHandle);
    }

    /**
     * If it is not already closed, the Peer Connection is implicitly closed before being deleted. After this function has been called, pc must not be
     * used in a function call anymore. This function will block until all scheduled callbacks of pc return (except the one this function might be
     * called in) and no other callback will be called for pc after it returns.
     */
    @Override
    public void close() {
        try {
            closeChannels();
        } catch (Exception e) {
            LOGGER.warn("Failed to close channels of peer connection", e);
        }
        cleanable.clean();
        onLocalDescription.close();
        onLocalCandidate.close();
        onStateChange.close();
        onIceStateChange.close();
        onGatheringStateChange.close();
        onSignalingStateChange.close();
        onDataChannel.close();
        onTrack.close();
    }

    /**
     * Closes all Data Channels.
     */
    public void closeChannels() {
        for (final DataChannel ch : new ArrayList<>(this.channels.values())) {
            ch.close();
        }
        if (!this.channels.isEmpty()) {
            closeChannels();
        }
    }

    /**
     * Initiates the handshake process.
     * <p>
     * Following this call, the local description callback will be called with the local description, which must be sent to the remote peer by the
     * user's method of choice.
     * <p>
     * Note this call is implicit after {@link #setRemoteDescription} and {@link #createDataChannel} if
     * disableAutoNegotiation was not set on Peer Connection creation.
     *
     * @param type (optional): type of the description ("offer", "answer", "pranswer", or "rollback") or NULL for autodetection.
     */
    public void setLocalDescription(String type) {
        wrapError("rtcSetLocalDescription", rtcSetLocalDescription(peerHandle, type));
    }

    /**
     * Retrieves the current local description in SDP format.
     *
     * @return the current local description
     */
    public String localDescription() {
        return rtcGetLocalDescription(peerHandle);
    }

    /**
     * Retrieves the current local description type as string. See {@link #setLocalDescription}.
     *
     * @return the current local description type
     */
    public String localDescriptionType() {
        return rtcGetLocalDescriptionType(peerHandle);
    }

    /**
     * Sets the remote description received from the remote peer by the user's method of choice. The remote description may have candidates or not.
     * <p>
     * If the remote description is an offer and disableAutoNegotiation was not set in rtcConfiguration, the library will automatically answer by
     * calling rtcSetLocalDescription internally. Otherwise, the user must call it to answer the remote description.
     *
     * @param sdp  the remote Session Description Protocol
     * @param type (optional): type of the description ("offer", "answer", "pranswer", or "rollback") or NULL for autodetection.
     */
    public void setRemoteDescription(String sdp, @Nullable SessionDescriptionType type) {
        String typeString = null;
        if (type != null) {
            typeString = type.type;
        }
        wrapError("rtcSetRemoteDescription", rtcSetRemoteDescription(peerHandle, sdp, typeString));
    }

    /**
     * Retrieves the current remote description in SDP format.
     *
     * @return the current remote description
     */
    public String remoteDescription() {
        return rtcGetRemoteDescription(peerHandle);
    }


    /**
     * Retrieves the current remote description type as string.
     *
     * @return the current remote description type
     */
    @Nullable
    public SessionDescriptionType remoteDescriptionType() {
        return SessionDescriptionType.of(rtcGetRemoteDescriptionType(peerHandle));
    }

    /**
     * Adds a trickled remote candidate received from the remote peer by the user's method of choice.
     * The Peer Connection must have a remote description set.
     *
     * @param candidate a null-terminated SDP string representing the candidate (with or without the "a=" prefix)
     */
    public void addRemoteCandidate(String candidate) {
        addRemoteCandidate(candidate, null);
    }

    /**
     * Adds a trickled remote candidate received from the remote peer by the user's method of choice.
     * The Peer Connection must have a remote description set.
     *
     * @param candidate a null-terminated SDP string representing the candidate (with or without the "a=" prefix)
     * @param mediaId       (optional): a null-terminated string representing the mediaId of the candidate in the remote SDP description or NULL for
     *                  autodetection
     */
    public void addRemoteCandidate(String candidate, @Nullable String mediaId) {
        wrapError("rtcAddRemoteCandidate", rtcAddRemoteCandidate(peerHandle, candidate, mediaId));
    }

    /**
     * Retrieves the current local address, i.e. the network address of the currently selected local candidate. The address will have the format
     * "IP_ADDRESS:PORT", where IP_ADDRESS may be either IPv4 or IPv6. The call might fail if the PeerConnection is not in state RTC_CONNECTED, and
     * the address might change after connection.
     *
     * @return the local address
     */
    public InetSocketAddress localAddress() {
        return parseAddress(rtcGetLocalAddress(peerHandle));
    }

    /**
     * Retrieves the current remote address, i.e. the network address of the currently selected remote candidate. The address will have the format
     * "IP_ADDRESS:PORT", where IP_ADDRESS may be either IPv4 or IPv6. The call may fail if the state is not RTC_CONNECTED, and the address might
     * change after connection.
     *
     * @return the remote address
     */
    public InetSocketAddress remoteAddress() {
        return parseAddress(rtcGetRemoteAddress(peerHandle));
    }

    /**
     * Retrieves the currently selected candidate pair. The call may fail if the state is not RTC_CONNECTED, and the selected candidate pair might
     * change after connection.
     *
     * @return the selected candidate pair
     */
    public CandidatePair selectedCandidatePair() {
        return rtcGetSelectedCandidatePair(peerHandle);
    }


    /**
     * Retrieves the maximum stream ID a Data Channel may use. It is useful to create user-negotiated Data Channels with negotiated=true and
     * manualStream=true. The maximum is negotiated during connection, therefore the final value after connection might be lower than before
     * connection if the remote maximum is lower.
     *
     * @return maximum stream ID
     */
    public int maxDataChannelStream() {
        return rtcGetMaxDataChannelStream(peerHandle);
    }

    /**
     * Retrieves the maximum message size for data channels on the peer connection as negotiated with the remote peer.
     *
     * @return the maximum message size
     */
    public int remoteMaxMessageSize() {
        return rtcGetRemoteMaxMessageSize(peerHandle);
    }

    public void setAnswer(String sdp) {
        this.setRemoteDescription(sdp, SessionDescriptionType.ANSWER);
    }


    /**
     * Adds a Data Channel on a Peer Connection. The Peer Connection does not need to be connected, however, the Data Channel will be open only when
     * the Peer Connection is connected.
     * <p>
     * rtcDataChannel() is equivalent to rtcDataChannelEx() with settings set to ordered, reliable, non-negotiated, with automatic stream ID selection
     * (all flags set to false), and protocol set to an empty string.
     *
     * @param label a user-defined UTF-8 string representing the Data Channel name
     * @return the created data channel
     */
    public DataChannel createDataChannel(String label) {
        return createDataChannel(label, DataChannelInitSettings.DEFAULT);
    }

    /**
     * Adds a Data Channel on a Peer Connection. The Peer Connection does not need to be connected, however, the Data Channel will be open only when
     * the Peer Connection is connected.
     *
     * @param label a user-defined UTF-8 string representing the Data Channel name
     * @param init  a structure of initialization settings
     * @return the created data channel
     */
    public DataChannel createDataChannel(String label, DataChannelInitSettings init) {
        final DataChannelReliability reliability = init.reliability();
        int stream = init.stream().orElse(0);
        boolean manualStream = init.stream().isPresent();
        final int channelHandle = wrapError("rtcCreateDataChannelEx", rtcCreateDataChannelEx(peerHandle, label, reliability.isUnordered(), reliability.isUnreliable(), reliability.maxPacketLifeTime().toMillis(), reliability.maxRetransmits(), init.protocol().orElse(null), init.isNegotiated(), stream, manualStream));
        final DataChannel channel = new DataChannel(this, channelHandle, executor);
        this.channels.put(channelHandle, channel);
        return channel;
    }

    // Adds a new Track on a Peer Connection. The Peer Connection does not need to be connected, however, the Track will be open only when the Peer Connection is connected.
    // sdp: a null-terminated string specifying the corresponding media SDP. It must start with a m-line and include a mid parameter.
    public Track addTrack(String sdp) {
        final int trackHandle = wrapError("rtcAddTrack", rtcAddTrack(peerHandle, sdp));
        return new Track(this, trackHandle);
    }

    public Track addTrack(TrackInit init) {
        final int trackHandle = wrapError("rtcAddTrackEx", rtcAddTrackEx(peerHandle, init.direction().direction, init.codec().codec));
        return new Track(this, trackHandle);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof PeerConnection)) return false;
        PeerConnection that = (PeerConnection) o;
        return peerHandle == that.peerHandle;
    }

    @Override
    public int hashCode() {
        return Objects.hashCode(peerHandle);
    }
}
