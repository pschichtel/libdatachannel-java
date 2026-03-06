package tel.schich.libdatachannel;

import org.eclipse.jdt.annotation.NonNull;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tel.schich.jniaccess.JNIAccess;

import java.lang.ref.Cleaner;
import java.nio.ByteBuffer;
import java.util.Objects;
import java.util.function.IntFunction;

public abstract class LibDataChannel {
    static final Cleaner CLEANER = Cleaner.create();
    private static final Logger LOGGER = LoggerFactory.getLogger(LibDataChannel.class);
    private static volatile boolean initialized = false;
    private static volatile IntFunction<ByteBuffer> allocator = ByteBuffer::allocateDirect;

    public static final String LIB_NAME = "datachannel-java";

    private LibDataChannel() {}

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
     * @param level the log level
     * @param message the message to log
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


    public static void setAllocator(@NonNull IntFunction<ByteBuffer> allocator) {
        LibDataChannel.allocator = Objects.requireNonNull(allocator, "allocator");
    }


    @JNIAccess
    static ByteBuffer allocate(int size) {
        if (size < 0) {
            throw new IllegalArgumentException("size must be >= 0");
        }
        ByteBuffer buffer = Objects.requireNonNull(allocator.apply(size), "allocator returned null");
        if (!buffer.isDirect()) {
            throw new IllegalArgumentException("allocator must return a direct ByteBuffer");
        }
        if (buffer.capacity() < size) {
            throw new IllegalArgumentException("allocator returned buffer with insufficient capacity");
        }
        buffer.clear();
        buffer.limit(size);
        return buffer;
    }

     

    private static native void freeMemory(long pointer);
}
