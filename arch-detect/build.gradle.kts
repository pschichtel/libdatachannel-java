plugins {
    id("tel.schich.libdatachannel.convention.common")
}

version = rootProject.version

val nativeLibs by configurations.registering

dependencies {
    api(rootProject)
    nativeLibs(project(mapOf("path" to rootProject.path, "configuration" to "archDetectConfiguration")))
}
tasks.jar.configure {
    dependsOn(nativeLibs)
    val allArtifacts = mutableListOf<Pair<File, String>>()
    
    // Add resolved artifacts
    nativeLibs.get().resolvedConfiguration.resolvedArtifacts.forEach { artifact ->
        val classifier = artifact.classifier
        if (classifier != null) {
            allArtifacts.add(Pair(artifact.file, classifier))
        }
    }
    
    // Add prebuilt libraries
    val prebuiltArtifacts = rootProject.extra["prebuiltArtifacts"] as? List<Pair<File, String>> ?: emptyList()
    prebuiltArtifacts.forEach { (file, classifier) ->
        if (!classifier.startsWith("android-")) {
            allArtifacts.add(file to classifier)
            logger.lifecycle("Found prebuilt library: ${file.name} with classifier: $classifier")
        }
    }
    
    // Process all artifacts
    allArtifacts.forEach { (file, classifier) ->
        from(zipTree(file)) {
            include("native/*.so")
            include("native/*.dll")
            include("native/*.dylib")
            into(classifier)
        }
        logger.lifecycle("Added library to arch-detect JAR: ${file.name} with classifier: $classifier")
    }
}

publishing.publications.withType<MavenPublication>().configureEach {
    pom {
        description = "${rootProject.description} The ${project.name} module bundles all architectures and allows runtime architecture detection."
    }
}