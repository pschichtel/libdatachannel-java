package tel.schich.libdatachannel;

import tel.schich.libdatachannel.exception.FailureException;
import tel.schich.libdatachannel.exception.InvalidException;
import tel.schich.libdatachannel.exception.LibDataChannelException;
import tel.schich.libdatachannel.exception.NotAvailableException;
import tel.schich.libdatachannel.exception.TooSmallException;
import tel.schich.libdatachannel.exception.UnknownFailureException;

import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Map;
import java.util.function.Function;

import static java.util.stream.Collectors.toMap;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_FAILURE;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_INVALID;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_NOT_AVAIL;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_SUCCESS;
import static tel.schich.libdatachannel.exception.LibDataChannelException.ERR_TOO_SMALL;

class Util {

    static void ensureDirect(ByteBuffer buffer) {
        if (!buffer.isDirect()) {
            throw new IllegalArgumentException("data must be a direct ByteBuffer!");
        }
    }

    static InetSocketAddress parseAddress(String rawAddress) {
        int colonIndex = rawAddress.lastIndexOf(':');
        String ip = rawAddress.substring(0, colonIndex);
        int port = Integer.parseInt(rawAddress.substring(colonIndex + 1));
        return InetSocketAddress.createUnresolved(ip, port);
    }

    static <T extends Enum<T>, K> Map<K, T> mappedEnum(final T[] values, Function<T, K> mapper) {
        return Arrays.stream(values).collect(toMap(mapper, s -> s));
    }

    static int wrapError(String operation, int result) throws LibDataChannelException {
        if (result > 0) {
            return result;
        }

        switch (result) {
            case ERR_SUCCESS:
                return 0;
            case ERR_INVALID:
                throw new InvalidException(operation);
            case ERR_FAILURE:
                throw new FailureException(operation);
            case ERR_NOT_AVAIL:
                throw new NotAvailableException(operation);
            case ERR_TOO_SMALL:
                throw new TooSmallException(operation);
            default:
                throw new UnknownFailureException(result, operation);
        }
    }
}
