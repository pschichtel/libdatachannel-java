package tel.schich.libdatachannel;

import java.io.Closeable;
import java.util.Objects;
import java.util.concurrent.Executor;
import java.util.function.Consumer;

import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetAvailableCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetBufferedAmountLowCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetBufferedAmountLowThreshold;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetClosedCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetErrorCallback;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcSetOpenCallback;

public abstract class Channel implements Closeable {
    private final PeerConnection peer;
    final long handle;

    public final EventListenerContainer<DataChannelCallback.Open> onOpen;
    public final EventListenerContainer<DataChannelCallback.Closed> onClosed;
    public final EventListenerContainer<DataChannelCallback.Error> onError;
    public final EventListenerContainer<DataChannelCallback.BufferedAmountLow> onBufferedAmountLow;
    public final EventListenerContainer<DataChannelCallback.Available> onAvailable;

    public Channel(final PeerConnection peer, final long handle, final Executor executor, final Consumer<Long> deleter) {
        this.peer = peer;
        this.handle = handle;

        this.onOpen = new EventListenerContainer<>("ChannelOpen", set -> rtcSetOpenCallback(handle, set), executor);
        this.onClosed = new EventListenerContainer<>("ChannelClosed", set -> rtcSetClosedCallback(handle, set), executor);
        this.onError = new EventListenerContainer<>("ChannelError", set -> rtcSetErrorCallback(handle, set), executor);
        this.onBufferedAmountLow = new EventListenerContainer<>("ChannelBufferedAmountLow", set -> rtcSetBufferedAmountLowCallback(handle, set), executor);
        this.onAvailable = new EventListenerContainer<>("ChannelAvailable", set -> rtcSetAvailableCallback(handle, set), executor);

        LibDataChannel.CLEANER.register(this, () -> {
            // make sure not to capture `this` here, that would be a memory leak
            deleter.accept(handle);
        });
    }

    /**
     * Returns the {@link PeerConnection} this channel belongs to
     *
     * @return the peer connection
     */
    public final PeerConnection peer() {
        return peer;
    }

    @Override
    public final void close() {
        close(handle);
        peer.dropChannelState(handle);

        onOpen.close();
        onClosed.close();
        onError.close();
        onBufferedAmountLow.close();
        onAvailable.close();
        postClose();
    }

    protected abstract void close(long handle);
    protected abstract boolean isClosed(long handle);
    protected abstract boolean isOpen(long handle);
    protected abstract long maxMessageSize(long handle);
    protected abstract long bufferedAmount(long handle);
    protected abstract long availableAmount(long handle);

    protected void postClose() {

    }

    /**
     * Returns whether the channel exists and is closed (not open and not connecting), false otherwise
     *
     * @return true if closed
     */
    public boolean isClosed() {
        return isClosed(handle);
    }

    /**
     * Returns whether the channel exists and is open, false otherwise
     *
     * @return true if open
     */
    public boolean isOpen() {
        return isOpen(handle);
    }

    /**
     * Returns the maximum message size for data channels on the peer connection as negotiated with the remote peer.
     *
     * @return the maximum message size
     */
    public long maxMessageSize() {
        return maxMessageSize(handle);
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
        rtcSetBufferedAmountLowThreshold(handle, amount);
    }

    /**
     * Returns the current buffered amount, i.e., the total size of currently buffered messages waiting to be actually sent in the channel.
     * <p>
     * This does not account for the data buffered at the transport level.
     * </p>
     *
     * @return the buffered amount
     */
    public long bufferedAmount() {
        return bufferedAmount(handle);
    }

    /**
     * Returns the available amount, i.e., the total size of messages pending reception with {@link #receiveMessage}.
     * <p>
     * This may only be called if the {@link #onMessage} callback is not set.
     * </p>
     *
     * @return the available amount
     */
    public long availableAmount() {
        return availableAmount(handle);
    }

    @Override
    public final int hashCode() {
        return Objects.hashCode(handle);
    }
}
