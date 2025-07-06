plugins {
    id("tel.schich.libdatachannel.convention.common")
}

version = rootProject.version

val nativeLibs by configurations.registering

dependencies {
    api(rootProject)
    nativeLibs(project(mapOf(
        "path" to rootProject.path,
        "configuration" to Constants.ANDROID_CONFIG,
    )))
}

tasks.jar.configure {
    dependsOn(nativeLibs)
    this.archiveExtension = "aar"
    for (jar in nativeLibs.get().resolvedConfiguration.resolvedArtifacts) {
        val classifier = jar.classifier ?: continue
        val abi = classifier.removePrefix(Constants.ANDROID_CLASSIFIER_PREFIX)
        from(zipTree(jar.file)) {
            include("native/*.so")
            includeEmptyDirs = false
            eachFile {
                relativePath = RelativePath.parse(true, "jni/$abi/${relativeSourcePath.lastName}")
            }
        }
    }
}

publishing.publications.withType<MavenPublication>().configureEach {
    pom {
        description = "${rootProject.description} The ${project.name} module bundles all architectures and allows runtime architecture detection."
    }
}