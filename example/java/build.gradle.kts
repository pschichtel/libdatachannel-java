plugins {
    application
    id("com.gradleup.shadow") version "9.3.1"
}

repositories {
    mavenCentral()
}

dependencies {
    implementation("tel.schich:libdatachannel-java-arch-detect")
    implementation("ch.qos.logback:logback-classic:1.5.26")
}

java {
    toolchain {
        languageVersion = JavaLanguageVersion.of(11)
    }

    testing {
        toolchain {
            languageVersion = JavaLanguageVersion.of(25)
        }
    }
}

application {
    mainClass = "tel.schich.libdatachannel.example.Main"
}
