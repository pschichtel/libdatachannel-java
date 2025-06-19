plugins {
    `kotlin-dsl`
}

repositories {
    gradlePluginPortal()
}

dependencies {
    implementation("io.github.zenhelix.maven-central-publish:io.github.zenhelix.maven-central-publish.gradle.plugin:0.8.0")
}