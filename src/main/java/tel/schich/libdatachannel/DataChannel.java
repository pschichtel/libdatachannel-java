package tel.schich.libdatachannel;

import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelLabel;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelProtocol;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelReliability;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcGetDataChannelStream;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcReceiveMessage;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcReceiveMessageInto;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSendMessage;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetMessageCallback;
import static tel.schich.libdatachannel.Util.ensureDirect;
import static tel.schich.libdatachannel.Util.wrapError;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Optional;
import java.util.concurrent.Executor;

/**
 * An RTC data channel, created from a {@link PeerConnection}.
 */
public class DataChannel extends Channel {
    public final EventListenerContainer<DataChannelCallback.Message> onMessage;

    DataChannel(final PeerConnection peer, final long channelHandle, final Executor executor) {
        super(peer, channelHandle, executor, DataChannel::delete);
        this.onMessage = new EventListenerContainer<>("ChannelMessage", set -> rtcSetMessageCallback(channelHandle, set), executor);
    }

    private void sendMessage(ByteBuffer data, int offset, int length) {
        wrapError("sendMessage", rtcSendMessage(handle, data, offset, length));
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
        data.put((byte) 0);
        data.flip();
        sendMessage(data, data.position(), -1);
    }

    @Override
    protected void postClose() {
        onMessage.close();
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
        return Optional.ofNullable(rtcReceiveMessage(handle));
    }

    /**
     * Returns a pending message if available.
     * <p>
     * This method may only be called if the {@link #onMessage} callback is not set.
     * </p>
     *
     * @param buffer the buffer to receive the message into
     * @return the bytes received or the negative size of the message if the buffer was too small
     */
    public int receiveMessage(ByteBuffer buffer) {
        ensureDirect(buffer);
        return rtcReceiveMessageInto(handle, buffer, buffer.position(), buffer.remaining());
    }

    /**
     * Returns the stream ID of this channel
     * 0
     *
     * @return the stream id
     */
    public int streamId() {
        return wrapError("rtcGetDataChannelStream", rtcGetDataChannelStream(handle));
    }

    /**
     * Returns the label of this channel
     *
     * @return the label
     */
    public String label() {
        return rtcGetDataChannelLabel(handle);
    }

    /**
     * Returns the protocol of this channel
     *
     * @return the protocol
     */
    public String protocol() {
        return rtcGetDataChannelProtocol(handle);
    }

    /**
     * Returns the configured {@link DataChannelReliability} of this channel
     *
     * @return the reliability
     */
    public DataChannelReliability reliability() {
        return rtcGetDataChannelReliability(handle);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof DataChannel)) return false;
        DataChannel channel = (DataChannel) o;
        return handle == channel.handle;
    }

    @Override
    protected native void close(long handle);

    @Override
    protected native boolean isClosed(long handle);

    @Override
    protected native boolean isOpen(long handle);

    @Override
    protected native long maxMessageSize(long handle);

    @Override
    protected native long bufferedAmount(long handle);

    @Override
    protected native long availableAmount(long handle);

    private static native void delete(long handle);
}
