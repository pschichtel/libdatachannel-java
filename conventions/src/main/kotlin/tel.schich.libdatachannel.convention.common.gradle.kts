plugins {
    `java-library`
    signing
    `maven-publish`
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

    testImplementation(platform("org.junit:junit-bom:5.13.1"))
    testImplementation("org.junit.jupiter:junit-jupiter")
    testImplementation("ch.qos.logback:logback-classic:1.5.18")
}

publishing {
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

if (!ci) {
    signing {
        useGpgCmd()
        sign(publishing.publications)
    }
}

