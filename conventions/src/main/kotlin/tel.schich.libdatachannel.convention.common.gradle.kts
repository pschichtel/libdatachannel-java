plugins {
    `java-library`
    signing
    `maven-publish`
}

group = "tel.schich"

java {
    withSourcesJar()
    withJavadocJar()

    toolchain {
        languageVersion = JavaLanguageVersion.of(11)
    }

    testing {
        toolchain {
            languageVersion = JavaLanguageVersion.of(25)
        }
    }
}

tasks.test {
    useJUnitPlatform()
}

tasks.withType<JavaCompile>().configureEach {
    options.compilerArgs.addAll(
        listOf(
            "-Xlint:deprecation",
            "-Xlint:unchecked",
        )
    )
}

repositories {
    mavenCentral()
    mavenLocal()
}

dependencies {
    compileOnly(libs.jdtAnnotations)
    implementation(libs.slf4j)

    testImplementation(libs.junitJupiter)
    testImplementation(libs.logbackClassic)
}

publishing {
    repositories {
        maven {
            name = Constants.SNAPSHOTS_REPO
            url = uri("https://central.sonatype.com/repository/maven-snapshots/")
            credentials(PasswordCredentials::class)
        }
        maven {
            name = Constants.RELEASES_REPO
            url = layout.buildDirectory.dir("repo").get().asFile.toURI()
        }
    }

    publications {
        register<MavenPublication>("maven") {
            artifactId = project.name
            from(components["java"])

            pom {
                name = artifactId
                description = project.description
                url = "https://github.com/pschichtel/libdatachannel-java"
                licenses {
                    license {
                        name = "MIT"
                        url = "https://opensource.org/licenses/MIT"
                    }
                }
                developers {
                    developer {
                        id.set("pschichtel")
                        name.set("Phillip Schichtel")
                        email.set("phillip@schich.tel")
                    }
                    developer {
                        id.set("faithcaio")
                    }
                }
                scm {
                    url.set("https://github.com/pschichtel/libdatachannel-java")
                    connection.set("scm:git:https://github.com/pschichtel/libdatachannel-java")
                    developerConnection.set("scm:git:git@github.com:pschichtel/libdatachannel-java")
                }
            }
        }
    }
}

private val signingKey = System.getenv("SIGNING_KEY")?.ifBlank { null }?.trim()
private val signingKeyPassword = System.getenv("SIGNING_KEY_PASSWORD")?.ifBlank { null }?.trim() ?: ""

when {
    signingKey != null -> {
        logger.lifecycle("Received a signing key, using in-memory pgp keys!")
        signing {
            useInMemoryPgpKeys(signingKey, signingKeyPassword)
            sign(publishing.publications)
        }
    }
    !Constants.CI -> {
        logger.lifecycle("Not running in CI, using the gpg command!")
        signing {
            useGpgCmd()
            sign(publishing.publications)
        }
    }
    else -> {
        logger.lifecycle("Not signing artifacts!")
    }
}
