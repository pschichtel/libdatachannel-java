import io.github.zenhelix.gradle.plugin.extension.PublishingType

plugins {
    `java-library`
    signing
    `maven-publish`
    id("io.github.zenhelix.maven-central-publish")
}

group = "tel.schich"

val ci = System.getenv("CI") != null

java {
    withSourcesJar()
    withJavadocJar()

    toolchain {
        languageVersion = JavaLanguageVersion.of(11)
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
    compileOnly("org.eclipse.jdt:org.eclipse.jdt.annotation:2.3.100")
    implementation("org.slf4j:slf4j-api:2.0.17")

    testImplementation(platform("org.junit:junit-bom:5.13.4"))
    testImplementation("org.junit.jupiter:junit-jupiter")
    testImplementation("ch.qos.logback:logback-classic:1.5.18")
}

private fun Project.getSecret(name: String): Provider<String> = provider {
    val env = System.getenv(name)
        ?.ifBlank { null }
    if (env != null) {
        return@provider env
    }

    val propName = name.split("_")
        .map { it.lowercase() }
        .joinToString(separator = "") { word ->
            word.replaceFirstChar { it.uppercase() }
        }
        .replaceFirstChar { it.lowercase() }

    property(propName) as String
}

mavenCentralPortal {
    credentials {
        username = project.getSecret("MAVEN_CENTRAL_PORTAL_USERNAME")
        password = project.getSecret("MAVEN_CENTRAL_PORTAL_PASSWORD")
    }
    publishingType = PublishingType.AUTOMATIC
}

publishing {
    repositories {
        maven {
            name = "mavenCentralSnapshots"
            url = uri("https://central.sonatype.com/repository/maven-snapshots/")
            credentials(PasswordCredentials::class)
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
when {
    signingKey != null -> {
        logger.lifecycle("Received a signing key, using in-memory pgp keys!")
        signing {
            useInMemoryPgpKeys(signingKey, null)
            sign(publishing.publications)
        }
    }
    !ci -> {
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

