plugins {
    application
    id("com.github.johnrengelman.shadow") version "8.1.1"
    id("org.graalvm.buildtools.native") version "0.10.2"
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("tel.schich:libdatachannel-java-arch-detect")
    implementation("ch.qos.logback:logback-classic:1.5.6")
}

java {
    toolchain {
        languageVersion = JavaLanguageVersion.of(11)
    }
}

application {
    mainClass = "tel.schich.libdatachannel.example.Main"
}

val graalToolchain = javaToolchains.launcherFor {
    languageVersion = JavaLanguageVersion.of(21)
    vendor = JvmVendorSpec.GRAAL_VM
}

graalvmNative {
    binaries {
        named("main") {
            resources.autodetect()
            imageName = project.name
            mainClass = application.mainClass.get()
            buildArgs.add("-O4")
            useFatJar = true
        }
    }
    binaries.all {
        buildArgs.add("--verbose")
        javaLauncher = graalToolchain
    }
}
