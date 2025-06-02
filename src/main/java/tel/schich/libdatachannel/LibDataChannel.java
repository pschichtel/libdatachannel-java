package tel.schich.libdatachannel;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tel.schich.jniaccess.JNIAccess;
import static tel.schich.libdatachannel.LibDataChannelNative.rtcFree;

import java.lang.ref.Cleaner;
import java.nio.ByteBuffer;

public class LibDataChannel {
    static final Cleaner CLEANER = Cleaner.create();
    private static final Logger LOGGER = LoggerFactory.getLogger(LibDataChannel.class);
    private static volatile boolean initialized = false;

    public static final String LIB_NAME = "datachannel-java";

    /**
     * Initializes the library by loading the native library.
     */
    public synchronized static void initialize() {
        if (initialized) {
            return;
        }

        Platform.loadNativeLibrary(LIB_NAME, LibDataChannel.class);

        initialized = true;
    }

    /**
     * @param level
     * @param message
     * @see <a href="https://github.com/paullouisageneau/libdatachannel/blob/master/DOC.md#rtcinitlogger">Documentation</a>
     */
    @JNIAccess
    static void log(int level, String message) {
        switch (level) {
            case 1:
            case 2:
                LOGGER.error(message);
                return;
            case 3:
                LOGGER.warn(message);
                return;
            case 4:
                LOGGER.info(message);
                return;
            case 5:
                LOGGER.debug(message);
                return;
            case 6:
                LOGGER.trace(message);
        }
    }

    @JNIAccess
    static void registerForCleanup(Object buffer, long nativeAddress) {
        if (buffer == null) return;
        CLEANER.register(buffer, ()->{
            rtcFree(nativeAddress);
        });
    }
}
