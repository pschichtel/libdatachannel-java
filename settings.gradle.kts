rootProject.name = "libdatachannel-java"

pluginManagement {
    includeBuild("conventions")
}

plugins {
    id("org.gradle.toolchains.foojay-resolver-convention") version "0.10.0"
}

include("arch-detect")
project(":arch-detect").name = "libdatachannel-java-arch-detect"

includeBuild("example/java")