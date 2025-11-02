package tel.schich.libdatachannel;

public abstract class LibDataChannelArchDetect {

    private LibDataChannelArchDetect() {}

    public static void initialize() {
        System.setProperty(Platform.classPathPropertyNameForLibrary(LibDataChannel.LIB_NAME), "/" + Platform.detectArch() + "/native/" + Platform.libraryFilename(LibDataChannel.LIB_NAME));
        LibDataChannel.initialize();
    }
}
