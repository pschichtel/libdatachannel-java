package tel.schich.libdatachannel;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;
import org.junit.jupiter.api.Test;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.*;

class PeerConnectionTest {
    @Test
    void createPeer() {
        try (PeerConnection peer = PeerConnection.createPeer(PeerConnectionConfiguration.DEFAULT)) {
            try (DataChannel ch = peer.createDataChannel("label")) {
                assertEquals("label", ch.label());
                assertFalse(peer.localDescription().isEmpty());
            }
        }
    }

    private void testLocalOffer(@Nullable String descriptionType) throws InterruptedException {
        final PeerConnectionConfiguration config = PeerConnectionConfiguration.DEFAULT
                .withDisableAutoNegotiation(true);
        try (PeerConnection peer = PeerConnection.createPeer(config)) {
            peer.createDataChannel("label");

            BlockingQueue<String> queue = new ArrayBlockingQueue<>(1);
            peer.onLocalDescription.register((peer1, sdp, type) -> {
                assertSame(peer, peer1);
                assertEquals(SessionDescriptionType.OFFER, type);
                assertNotNull(sdp);
                assertTrue(queue.offer(sdp));
            });

            peer.setLocalDescription(descriptionType);
            final String peerApiSdp = peer.localDescription();
            final String callbackSdp = queue.poll(1, TimeUnit.SECONDS);

            assertNotNull(callbackSdp);
            assertFalse(callbackSdp.isBlank());
            assertFalse(peerApiSdp.isBlank());
        }
    }

    @Test
    void explicitLocalOffer() throws InterruptedException {
        testLocalOffer("offer");
    }

    @Test
    void explicitLocalAutomaticDescription() throws InterruptedException {
        testLocalOffer(null);
    }
}