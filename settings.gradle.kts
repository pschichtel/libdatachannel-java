rootProject.name = "libdatachannel-java"

includeBuild("conventions")

plugins {
    id("org.gradle.toolchains.foojay-resolver-convention") version "1.0.0"
}

include("arch-detect")
project(":arch-detect").name = "libdatachannel-java-arch-detect"
include("android")
project(":android").name = "libdatachannel-java-android"

includeBuild("example/java")