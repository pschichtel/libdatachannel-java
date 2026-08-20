package tel.schich.libdatachannel;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

import java.lang.ref.WeakReference;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcDeletePeerConnection;

class PeerCallbackLifecycleTest {
    private static final long CALLBACK_TIMEOUT_MILLIS = TimeUnit.SECONDS.toMillis(5);

    static {
        LibDataChannel.initialize();
    }

    private static native void armSignalingStateCallback();

    private static native boolean awaitSignalingStateCallback(long timeoutMillis);

    private static native void releaseSignalingStateCallback(boolean dispatch);

    @Test
    @Timeout(15)
    void retainsCallbackUntilPeerDeletionDrainsScheduledCallbacks() throws InterruptedException {
        BlockedPeer blockedPeer = createPeerWithBlockedCallback();
        AtomicReference<Throwable> deletionFailure = new AtomicReference<>();
        AtomicInteger deletionResult = new AtomicInteger();
        CountDownLatch deletionStarted = new CountDownLatch(1);
        Thread deletionThread = new Thread(() -> {
            deletionStarted.countDown();
            try {
                deletionResult.set(rtcDeletePeerConnection(blockedPeer.peerHandle));
            } catch (Throwable failure) {
                deletionFailure.set(failure);
            }
        }, "peer-deletion-test");

        boolean dispatchCallback = false;
        try {
            deletionThread.start();
            assertTrue(deletionStarted.await(1, TimeUnit.SECONDS));
            awaitDeletionBlock(deletionThread);

            forceGarbageCollection();
            assertNotNull(blockedPeer.listener.get(),
                    "peer deletion released its callback before the blocked native callback returned");
            dispatchCallback = true;
        } finally {
            releaseSignalingStateCallback(dispatchCallback);
            deletionThread.join(CALLBACK_TIMEOUT_MILLIS);
        }

        assertFalse(deletionThread.isAlive(), "peer deletion did not finish after the callback returned");
        assertNull(deletionFailure.get());
        assertEquals(0, deletionResult.get());
        assertEquals(1, blockedPeer.callbackCount.get());
    }

    private static BlockedPeer createPeerWithBlockedCallback() {
        PeerConnectionConfiguration configuration = PeerConnectionConfiguration.DEFAULT
                .withDisableAutoNegotiation(true);
        PeerConnection peer = PeerConnection.createPeer(configuration);
        DataChannel channel = peer.createDataChannel("lifecycle-test");
        AtomicInteger callbackCount = new AtomicInteger();
        peer.onSignalingStateChange.register((ignoredPeer, ignoredState) -> callbackCount.incrementAndGet());

        armSignalingStateCallback();
        peer.setLocalDescription("offer");
        boolean callbackEntered = awaitSignalingStateCallback(CALLBACK_TIMEOUT_MILLIS);
        if (!callbackEntered) {
            releaseSignalingStateCallback(false);
        }
        assertTrue(callbackEntered, "the native signaling callback did not start");

        channel.close();
        return new BlockedPeer(peer.peerHandle, new WeakReference<>(peer.listener), callbackCount);
    }

    private static void awaitDeletionBlock(Thread deletionThread) throws InterruptedException {
        long deadline = System.nanoTime() + TimeUnit.SECONDS.toNanos(1);
        while (deletionThread.isAlive() && deletionThread.getState() == Thread.State.RUNNABLE
                && System.nanoTime() < deadline) {
            Thread.sleep(10);
        }
        assertTrue(deletionThread.isAlive(), "peer deletion returned before the native callback was released");
    }

    private static void forceGarbageCollection() throws InterruptedException {
        for (int attempt = 0; attempt < 10; attempt++) {
            System.gc();
            System.runFinalization();
            Thread.sleep(20);
        }
    }

    private static final class BlockedPeer {
        private final int peerHandle;
        private final WeakReference<PeerConnectionListener> listener;
        private final AtomicInteger callbackCount;

        private BlockedPeer(int peerHandle, WeakReference<PeerConnectionListener> listener,
                            AtomicInteger callbackCount) {
            this.peerHandle = peerHandle;
            this.listener = listener;
            this.callbackCount = callbackCount;
        }
    }
}
