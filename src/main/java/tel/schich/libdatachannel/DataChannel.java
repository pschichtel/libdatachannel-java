package tel.schich.libdatachannel;

import static tel.schich.libdatachannel.LibDataChannelNative.rtcClose;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcDeleteDataChannel;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetAvailableAmount;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetBufferedAmount;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelLabel;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelProtocol;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelReliability;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelStream;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcIsClosed;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcIsOpen;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcMaxMessageSize;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcReceiveMessage;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcReceiveMessageInto;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSendMessage;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetAvailableCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetBufferedAmountLowCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetBufferedAmountLowThreshold;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetClosedCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetErrorCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetMessageCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetOpenCallback;
import static tel.schich.libdatachannel.Util.ensureDirect;
import static tel.schich.libdatachannel.Util.wrapError;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_INVALID;

import java.io.Closeable;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.Executor;

/**
 * An RTC data channel, created from a {@link PeerConnection}.
 */
public class DataChannel implements Closeable {
    private final PeerConnection peer;
    final int channelHandle;

    public final EventListenerContainer<DataChannelCallback.Open> onOpen;
    public final EventListenerContainer<DataChannelCallback.Closed> onClosed;
    public final EventListenerContainer<DataChannelCallback.Error> onError;
    public final EventListenerContainer<DataChannelCallback.Message> onMessage;
    public final EventListenerContainer<DataChannelCallback.BufferedAmountLow> onBufferedAmountLow;
    public final EventListenerContainer<DataChannelCallback.Available> onAvailable;

    DataChannel(final PeerConnection peer, final int channelHandle, final Executor executor) {
        this.peer = peer;
        this.channelHandle = channelHandle;

        this.onOpen = new EventListenerContainer<>("ChannelOpen", set -> rtcSetOpenCallback(channelHandle, set), executor);
        this.onClosed = new EventListenerContainer<>("ChannelClosed", set -> rtcSetClosedCallback(channelHandle, set), executor);
        this.onError = new EventListenerContainer<>("ChannelError", set -> rtcSetErrorCallback(channelHandle, set), executor);
        this.onMessage = new EventListenerContainer<>("ChannelMessage", set -> rtcSetMessageCallback(channelHandle, set), executor);
        this.onBufferedAmountLow = new EventListenerContainer<>("ChannelBufferedAmountLow", set -> rtcSetBufferedAmountLowCallback(channelHandle, set), executor);
        this.onAvailable = new EventListenerContainer<>("ChannelAvailable", set -> rtcSetAvailableCallback(channelHandle, set), executor);
    }

    /**
     * Returns the {@link PeerConnection} this channel belongs to
     *
     * @return the peer connection
     */
    public PeerConnection peer() {
        return peer;
    }

    private void sendMessage(ByteBuffer data, int offset, int length) {
        if( length < 0 || offset < 0 || offset + length > data.capacity()) {
            throw new IllegalArgumentException("Buffer access out of bounds");
        }

        wrapError("sendMessage", rtcSendMessage(channelHandle, data, offset, length));
    }

    /**
     * Sends a binary message in the channel
     *
     * @param data the data
     */
    public void sendMessage(ByteBuffer data) {
        ensureDirect(data);
        sendMessage(data, data.position(), data.remaining());
    }

    /**
     * Sends a text message in the channel
     *
     * @param message the message
     */
    public void sendMessage(String message) {
        byte[] bytes = message.getBytes(StandardCharsets.UTF_8);
        ByteBuffer data = ByteBuffer.allocateDirect(bytes.length + 1);
        data.put(bytes);
        data.put((byte)0);
        data.flip();
        sendMessage(data, data.position(), -1);
    }

    /**
     * Closes and deletes the channel
     * <p>
     * This function will block until all scheduled callbacks of this channel return (except the one this function might be called in)
     * </p>
     * No other callback will be called for channel after it returns.
     */
    @Override
    public void close() {
        if (rtcClose(channelHandle) != ERR_INVALID) {
            rtcDeleteDataChannel(channelHandle);
        }

        peer.dropChannelState(channelHandle);
        onOpen.close();
        onClosed.close();
        onError.close();
        onMessage.close();
        onBufferedAmountLow.close();
        onAvailable.close();
    }

    /**
     * Returns whether the channel exists and is closed (not open and not connecting), false otherwise
     *
     * @return true if closed
     */
    public boolean isClosed() {
        return rtcIsClosed(channelHandle);
    }

    /**
     * Returns whether the channel exists and is open, false otherwise
     *
     * @return true if open
     */
    public boolean isOpen() {
        return rtcIsOpen(channelHandle);
    }

    /**
     * Returns the maximum message size for data channels on the peer connection as negotiated with the remote peer.
     *
     * @return the maximum message size
     */
    public int maxMessageSize() {
        return wrapError("rtcMaxMessageSize", rtcMaxMessageSize(channelHandle));
    }

    /**
     * Changes the buffered amount threshold under which {@link DataChannelCallback.BufferedAmountLow} is called.
     * <p>
     * The callback is called when the buffered amount was strictly superior and gets equal to or lower than the threshold when a message is sent.
     * </p>
     * <p>
     * The initial threshold is 0, meaning the callback is called each time the buffered amount goes back to zero after being non-zero.
     * </p>
     *
     * @param amount the amount
     */
    public void bufferedAmountLowThreshold(int amount) {
        wrapError("rtcSetBufferedAmountLowThreshold", rtcSetBufferedAmountLowThreshold(channelHandle, amount));
    }

    /**
     * Returns a pending message if available.
     * <p>
     * The may only be called if the {@link #onMessage} callback is not set.
     * </p>
     *
     * @return the received message
     */
    public Optional<ByteBuffer> receiveMessage() {
        return Optional.ofNullable(rtcReceiveMessage(channelHandle));
    }

    /**
     * Returns a pending message if available.
     * <p>
     * The may only be called if the {@link #onMessage} callback is not set.
     * </p>
     *
     * @return the bytes received or the negative size of the message if the buffer was too small
     */
    public int receiveMessage(ByteBuffer buffer) {
        ensureDirect(buffer);
        return rtcReceiveMessageInto(channelHandle, buffer, buffer.position(), buffer.remaining());
    }

    /**
     * Returns the available amount, i.e. the total size of messages pending reception with {@link #receiveMessage}.
     * <p>
     * This may only be called if the {@link #onMessage} callback is not set.
     * </p>
     *
     * @return the available amount
     */
    public int availableAmount() {
        return wrapError("rtcGetAvailableAmount", rtcGetAvailableAmount(channelHandle));
    }

    /**
     * Returns the current buffered amount, i.e. the total size of currently buffered messages waiting to be actually sent in the channel.
     * <p>
     * This does not account for the data buffered at the transport level.
     * </p>
     *
     * @return the buffered amount
     */
    public int bufferedAmount() {
        return wrapError("rtcGetBufferedAmount", rtcGetBufferedAmount(channelHandle));
    }

    /**
     * Returns the stream ID of this channel
     *0
     * @return the stream id
     */
    public int streamId() {
        return wrapError("rtcGetDataChannelStream", rtcGetDataChannelStream(channelHandle));
    }

    /**
     * Returns the label of this channel
     *
     * @return the label
     */
    public String label() {
        return rtcGetDataChannelLabel(channelHandle);
    }

    /**
     * Returns the protocol of this channel
     *
     * @return the protocol
     */
    public String protocol() {
        return rtcGetDataChannelProtocol(channelHandle);
    }

    /**
     * Returns the configured {@link DataChannelReliability} of this channel
     *
     * @return the reliability
     */
    public DataChannelReliability reliability() {
        return rtcGetDataChannelReliability(channelHandle);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof DataChannel)) return false;
        DataChannel channel = (DataChannel) o;
        return channelHandle == channel.channelHandle;
    }

    @Override
    public int hashCode() {
        return Objects.hashCode(channelHandle);
    }
}
