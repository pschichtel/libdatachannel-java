# libdatachannel-java
[![License: MPL 2.0](https://img.shields.io/badge/License-MPL_2.0-blue.svg)](https://www.mozilla.org/en-US/MPL/2.0/)

Java wrappers for [libdatachannel](https://github.com/paullouisageneau/libdatachannel), a WebRTC Data Channels standalone implementation in C++.

## Usage

#### Gradle (build.gradle.kts)
```kotlin
implementation("tel.schich:libdatachannel-java:0.22.6.1")
```

#### Maven (pom.xml)
```xml
<dependency>
    <groupId>tel.schich</groupId>
    <artifactId>libdatachannel-java</artifactId>
    <version>0.22.6.1</version>
</dependency>
```

Additionally, pull the architecture-specific native components. Alternatively, use the `libdatachannel-java-arch-detect`
module, which includes common architectures and has code to detect which one to apply.

### Offerer example

```java
var cfg = RTCConfiguration.of("stun.l.google.com:19302");
// try with resources to cleanup peer when done
try (var peer = RTCPeerConnection.createPeer(cfg)) {
        // when complete send sdp to remote peer
        peer.onGatheringStateChange((pc, state) -> {
        if (RTC_GATHERING_COMPLETE == state) {
var sdp = pc.localDescription();
            System.out.println(sdp);
        }
                });
// create data channel
var channel = peer.createDataChannel("test");
// wait for local sdp...
// then set answer from remote peer
    peer.setAnswer(readInput());
        // register message callback
        channel.onMessage((c, message, size) -> 
            System.out.println("Incoming message: " + new String(message)));
// block until channel is closed
CompletableFuture<Void> future = new CompletableFuture<>();
    channel.onClose(c -> future.completeAsync(() -> null));
        future.join();
}
```

## Examples

* Web Example: https://pschichtel.github.io/libdatachannel-java/ ([Source](example/web))

See [tests](#TODO) for more examples
