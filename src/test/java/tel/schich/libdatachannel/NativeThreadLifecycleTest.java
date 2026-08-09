package tel.schich.libdatachannel;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;

class NativeThreadLifecycleTest {
    static {
        LibDataChannel.initialize();
    }

    private static native Thread attachAndTerminateNativeThread();

    @Test
    void detachesTerminatedNativeThread() {
        Thread thread = attachAndTerminateNativeThread();

        assertNotNull(thread);
        assertFalse(thread.isAlive());
        assertEquals(Thread.State.TERMINATED, thread.getState());
    }
}
