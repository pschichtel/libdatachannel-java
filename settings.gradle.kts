rootProject.name = "libdatachannel-java"

plugins {
    id("org.gradle.toolchains.foojay-resolver-convention") version "1.0.0"
}

includeBuild("conventions")

include("arch-detect")
project(":arch-detect").name = "libdatachannel-java-arch-detect"
include("android")
project(":android").name = "libdatachannel-java-android"

includeBuild("example/java")