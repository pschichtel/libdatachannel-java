package tel.schich.libdatachannel;

import org.eclipse.jdt.annotation.Nullable;

import static tel.schich.libdatachannel.Util.mappedEnum;

import java.util.Map;
import java.util.concurrent.Executor;

public class Track extends Channel {
    public Track(final PeerConnection peer, final long trackHandle, final Executor executor) {
        super(peer, trackHandle, executor, Track::delete);
    }

    // After this function has been called, tr must not be used in a function call anymore. This function will block until all scheduled callbacks
    // of tr return (except the one this function might be called in) and no other callback will be called for tr after it returns.
    public String description() {
        return getTrackDescription(handle);
    }

    // Retrieves the mid (media identifier) of a Track.
    public String mediaId() {
        return getTrackMid(handle);
    }

    // Retrieves the direction of a Track.
    public Direction direction() {
        final int directionInt = getTrackDirection(handle);
        final Direction direction = Direction.of(directionInt);
        if (direction == null) {
            throw new IllegalStateException("Unknown track direction: " + directionInt);
        }
        return direction;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Track)) return false;
        Track track = (Track) o;
        return handle == track.handle;
    }

    public enum Direction {
        RTC_DIRECTION_UNKNOWN(0),
        RTC_DIRECTION_SENDONLY(1),
        RTC_DIRECTION_RECVONLY(2),
        RTC_DIRECTION_SENDRECV(3),
        RTC_DIRECTION_INACTIVE(4),
        ;

        public static final Direction DEFAULT = RTC_DIRECTION_UNKNOWN;

        private static final Map<Integer, Direction> MAP = mappedEnum(Direction.values(), s -> s.direction);
        final int direction;

        Direction(int direction) {
            this.direction = direction;
        }

        @Nullable
        public static Direction of(final int direction) {
            return MAP.get(direction);
        }
    }

    public enum Codec {
        RTC_CODEC_H264(0),
        RTC_CODEC_VP8(1),
        RTC_CODEC_VP9(2),
        RTC_CODEC_H265(3),
        RTC_CODEC_AV1(4),
        RTC_CODEC_OPUS(128),
        RTC_CODEC_PCMU(129),
        RTC_CODEC_PCMA(130),
        RTC_CODEC_AAC(131),
        ;

        public static final Codec DEFAULT = RTC_CODEC_H264;

        private static final Map<Integer, Codec> MAP = mappedEnum(Codec.values(), s -> s.codec);
        final int codec;

        Codec(int codec) {
            this.codec = codec;
        }

        @Nullable
        static Codec of(final int codec) {
            return MAP.get(codec);
        }
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

    private static native String getTrackDescription(long trackHandle);
    private static native int getTrackDirection(long trackHandle);
    private static native String getTrackMid(long trackHandle);

    private static native void delete(long handle);
}
