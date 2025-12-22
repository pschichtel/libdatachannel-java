import io.github.danielliu1123.deployer.PublishingType
import org.gradle.kotlin.dsl.support.serviceOf
import tel.schich.dockcross.execute.DockerRunner
import tel.schich.dockcross.execute.NonContainerRunner
import tel.schich.dockcross.execute.RemoteSshRunner
import tel.schich.dockcross.tasks.DockcrossRunTask
import java.net.URI
import java.nio.file.Files
import java.nio.file.Paths

plugins {
    id("tel.schich.libdatachannel.convention.common")
    alias(libs.plugins.dockcross)
    alias(libs.plugins.mavenDeployer)
}

tasks.wrapper {
    distributionType = Wrapper.DistributionType.ALL
}

fun extractLibDataChannelVersion(): String {
    val regex = """#define\s+RTC_VERSION\s+"([^"]+)"""".toRegex()
    val headerPath = project.layout.projectDirectory
        .file("jni/libdatachannel/include/rtc/version.h")
        .asFile.toPath()
    val headerContent = Files.readString(headerPath)
    val match = regex.find(headerContent) ?: return "unknown"

    return match.groupValues[1]
}

fun produceVersion(): String {
    val libDataChannelVersion = extractLibDataChannelVersion()
    val hasTags = project.providers.exec {
        commandLine("git", "tag")
    }.standardOutput.asText.get().trim().isNotEmpty()
    val defaultVersion = "$libDataChannelVersion.0-SNAPSHOT"
    if (!hasTags) {
        return defaultVersion
    }
    val describeOutput = project.providers.exec {
        commandLine("git", "describe", "--tags")
    }.standardOutput.asText.get().trim().removePrefix("v")

    val parts = describeOutput.split("-", limit = 2)
    val tagVersion = parts[0]
    return if (parts.size > 1) {
        if (tagVersion.startsWith(libDataChannelVersion)) {
            val versionParts = tagVersion.split('.').toMutableList()
            versionParts[versionParts.size - 1] = versionParts[versionParts.size - 1].toInt().inc().toString()
            versionParts.joinToString(".") + "-SNAPSHOT"
        } else {
            defaultVersion
        }
    } else {
        if (tagVersion.startsWith(libDataChannelVersion)) {
            tagVersion
        } else {
            throw GradleException("The version derived from the latest git tag is conflicting with libdatachannel!")
        }
    }
}

version = produceVersion()
val isSnapshot = version.toString().endsWith("-SNAPSHOT")
description = "${project.name} is a binding to the libdatachannel that feels native to Java developers."

val currentVersion by tasks.registering(DefaultTask::class) {
    doLast {
        println(version)
    }
}

val archDetectConfiguration = configurations.register(Constants.ARCH_DETECT_CONFIG) {
    isCanBeConsumed = true
}

val androidConfiguration = configurations.register(Constants.ANDROID_CONFIG) {
    isCanBeConsumed = true
}

val jniPath = project.layout.projectDirectory.dir("jni")
tasks.compileJava.configure {
    val annotationProcessorArgs = listOf(
        "generate.jni.headers" to "true",
        "generate.cache.mode.default" to "EAGER_PERSISTENT",
    ).map { "-A${it.first}=${it.second}" }
    options.compilerArgs.addAll(annotationProcessorArgs)
    options.headerOutputDirectory = jniPath.dir("generated")
}

val nativeGroup = "native"
val ci = System.getenv("CI") != null
val buildReleaseBinaries = project.findProperty("libdatachannel.build-release-binaries")
    ?.toString()
    ?.ifEmpty { null }
    ?.toBooleanStrictOrNull()
    ?: !project.version.toString().endsWith("-SNAPSHOT")

fun DockcrossRunTask.configureSshRemoteBuild(target: BuildTarget) {
    if (!ci) {
        return
    }

    fun findTarget(scope: String?): URI? {
        return project.findProperty("libdatachannel${scope?.let { ".$it" }.orEmpty()}.ssh-target")
            ?.toString()
            ?.let(::URI)
    }

    val sshTarget = findTarget(scope = target.classifier)
        ?: findTarget(scope = target.family)
        ?: findTarget(scope = null)
        ?: return

    val sshRunner = RemoteSshRunner(sshTarget) {
        add("--exclude", "**/.git")
        // this intentionally only includes the root-.gitignore, so that the generated jni files are included
        add("--filter=.- .gitignore")
    }
    runner(sshRunner)
    image = ""
}

fun DockcrossRunTask.baseConfigure(outputTo: Directory, target: BuildTarget) {
    group = nativeGroup
    dockcrossTag = "20250109-7bf589c"

    inputs.file(project.layout.projectDirectory.file("jni/build.sh"))

    when {
        target.image == null -> {
            image = "dummy"
        }
        '/' in target.image || ':' in target.image -> {
            val parts = target.image.split(':', limit = 2)
            dockcrossRepository = parts[0]
            dockcrossTag = if (parts.size == 2) {
                parts[1]
            } else {
                "latest"
            }
            image = "dummy"
        }
        else -> {
            image = target.image
        }
    }

    inputs.dir(jniPath)

    dependsOn(tasks.compileJava)

    output = outputTo.dir("native")
    extraEnv.putAll(target.env)
    extraEnv.put("JOBS", project.gradle.startParameter.maxWorkerCount.toString())
    extraEnv.put("RELATIVE_PROJECT_PATH", output.get().asFile.toPath().relativize(jniPath.asFile.toPath()).toString())
    extraEnv.put("PROJECT_VERSION", project.version.toString())
    extraEnv.put("PROJECT_BUILD_TYPE", if (buildReleaseBinaries) "Release" else "Debug")
    extraEnv.put("TARGET_FAMILY", target.family)
    extraEnv.put("TARGET_CLASSIFIER", target.classifier)
    target.image?.let {
        extraEnv.put("TARGET_IMAGE", it)
    }

    script = listOf(listOf("bash", "../../../../jni/build.sh"))

    configureSshRemoteBuild(target)
}

fun Jar.baseConfigure(compileTask: TaskProvider<DockcrossRunTask>, buildOutputDir: Directory) {
    group = nativeGroup

    dependsOn(compileTask)

    from(buildOutputDir) {
        include("native/libdatachannel-java.so")
        include("native/libdatachannel-java.dll")
        include("native/libdatachannel-java.dylib")
    }
}

val dockcrossOutputDir: Directory = project.layout.buildDirectory.get().dir("dockcross")
val nativeForHostOutputDir: Directory = dockcrossOutputDir.dir("host")
val compileNativeForHost by tasks.registering(DockcrossRunTask::class) {
    baseConfigure(nativeForHostOutputDir, BuildTarget(image = null, family = "host", classifier = "host"))
    unsafeWritableMountSource = true
    runner(NonContainerRunner)
}

val packageNativeForHost by tasks.registering(Jar::class) {
    baseConfigure(compileNativeForHost, nativeForHostOutputDir)
    archiveClassifier = "host"
}

data class BuildTarget(
    val image: String?,
    val family: String,
    val classifier: String,
    val env: Map<String, String> = emptyMap(),
    val args: List<String> = emptyList(),
    val outputTo: NamedDomainObjectProvider<Configuration> = archDetectConfiguration,
)

fun androidTarget(abi: String) = BuildTarget(
    image = "ghcr.io/pschichtel/cross-build/android:20250620-f1c8ddd",
    family = "android",
    classifier = "${Constants.ANDROID_CLASSIFIER_PREFIX}$abi",
    env = mapOf("ANDROID_ABI" to abi),
    outputTo = androidConfiguration,
)

fun macosTarget(classifier: String, arch: String) = BuildTarget(
    image = "ghcr.io/pschichtel/cross-build/osx:20250620-f1c8ddd",
    family = "macos",
    classifier = "${Constants.MACOS_CLASSIFIER_PREFIX}$classifier",
    env = mapOf("OSXCROSS_HOST" to "$arch-apple-darwin23.6"),
)

val targets = listOf(
    BuildTarget(
        image = "linux-x64",
        family = "linux",
        classifier = "x86_64",
    ),
//    BuildTarget(
//        image = "linux-x86",
//        family = "linux",
//        classifier = "x86_32",
//    ),
    BuildTarget(
        image = "linux-arm64",
        family = "linux",
        classifier = "aarch64",
    ),
    BuildTarget(
        image = "windows-static-x64",
        family = "windows",
        classifier = "${Constants.WINDOWS_CLASSIFIER_PREFIX}x86_64",
    ),
//    BuildTarget(
//        image = "windows-static-x86",
//        family = "windows",
//        classifier = "${Constants.WINDOWS_CLASSIFIER_PREFIX}x86_32",
//    ),
    androidTarget(abi = "armeabi-v7a"),
    androidTarget(abi = "arm64-v8a"),
    androidTarget(abi = "x86"),
    androidTarget(abi = "x86_64"),
    macosTarget(classifier = "arm64", arch = "aarch64"),
    macosTarget(classifier = "x86_64", arch = "x86_64"),
)

val packageNativeAll by tasks.registering(DefaultTask::class) {
    group = nativeGroup
}

var previousCompileNative: TaskProvider<DockcrossRunTask>? = null
for (target in targets) {
    val outputDir: Directory = dockcrossOutputDir.dir(target.classifier)
    val taskSuffix = target.classifier.split("[_-]".toRegex())
        .joinToString(separator = "") { it.lowercase().replaceFirstChar(Char::uppercaseChar) }
    val prebuiltPath = project.findProperty("libdatachannel.${target.classifier}.prebuilt-path")
        ?.toString()
        ?.ifBlank { null }
        ?.let { Paths.get(it).toFile() }

    val packageTaskName = "packageNativeFor$taskSuffix"
    val packageNative = if (prebuiltPath == null) {
        val compileNative = tasks.register("compileNativeFor$taskSuffix", DockcrossRunTask::class) {
            baseConfigure(outputDir, target)
            unsafeWritableMountSource = true
            containerName = "dockcross-${project.name}-${target.classifier}"
        }


        if (ci) {
            val previous = previousCompileNative
            compileNative {
                if (target.image == null) {
                    runner(NonContainerRunner)
                } else {
                    runner(DockerRunner())
                }
                if (previous != null) {
                    mustRunAfter(previous)
                }

                if (target.image != null) {
                    val execOps = project.serviceOf<ExecOperations>()
                    doLast {
                        execOps.exec {
                            commandLine("docker", "image", "prune", "-af")
                        }
                    }
                }
            }

            previousCompileNative = compileNative
        }

        tasks.register(packageTaskName, Jar::class) {
            baseConfigure(compileNative, outputDir)
            archiveClassifier = target.classifier
        }
    } else {
        tasks.register(packageTaskName, Jar::class) {
            group = nativeGroup
            from(prebuiltPath.parentFile) {
                include(prebuiltPath.name)
                includeEmptyDirs = false
                eachFile {
                    val fileName = when (target.family) {
                        "macos" -> "libdatachannel-java.dylib"
                        "windows" -> "libdatachannel-java.dll"
                        else -> "libdatachannel-java.so"
                    }
                    relativePath = RelativePath.parse(true, "native/$fileName")
                }
            }
            archiveClassifier = target.classifier

            doFirst {
                if (!prebuiltPath.exists()) {
                    throw GradleException("Prebuilt binary for $target does not exist: $prebuiltPath")
                }
            }
        }
    }

    publishing.publications.withType<MavenPublication>().configureEach {
        artifact(packageNative)
    }

    packageNativeAll.configure {
        dependsOn(packageNative)
    }

    artifacts.add(target.outputTo.name, packageNative)
}

dependencies {
    annotationProcessor(libs.jniAccessGenerator)
    compileOnly(libs.jniAccessGenerator)

    testImplementation(files(packageNativeForHost))
}

publishing.publications.withType<MavenPublication>().configureEach {
    pom {
        description = "${project.description}"
    }
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

deploy {
    // dirs to upload, they will all be packaged into one bundle
    dirs = provider {
        allprojects
            .map { it.layout.buildDirectory.dir("repo").get().asFile }
            .filter { it.exists() }
            .toList()
    }
    username = project.getSecret("MAVEN_CENTRAL_PORTAL_USERNAME")
    password = project.getSecret("MAVEN_CENTRAL_PORTAL_PASSWORD")
    publishingType = if (Constants.CI) {
        PublishingType.WAIT_FOR_PUBLISHED
    } else {
        PublishingType.USER_MANAGED
    }
}

tasks.deploy {
    for (project in allprojects) {
        val publishTasks = project.tasks
            .withType<PublishToMavenRepository>()
        mustRunAfter(publishTasks)
    }
}

val mavenCentralDeploy by tasks.registering(DefaultTask::class) {
    group = "publishing"

    val repo = if (isSnapshot) {
        Constants.SNAPSHOTS_REPO
    } else {
        dependsOn(tasks.deploy)
        Constants.RELEASES_REPO
    }
    for (project in allprojects) {
        val publishTasks = project.tasks
            .withType<PublishToMavenRepository>()
            .matching { it.repository.name == repo }
        dependsOn(publishTasks)
    }

    doFirst {
        if (isSnapshot) {
            logger.lifecycle("Snapshot deployment!")
        } else {
            logger.lifecycle("Release deployment!")
        }
    }
}

val githubActions by tasks.registering(DefaultTask::class) {
    group = "publishing"
    val deployRefPattern = """^refs/(?:tags/v\d+.\d+.\d+.\d+|heads/main)$""".toRegex()
    val ref = System.getenv("GITHUB_REF")?.ifBlank { null }?.trim()

    if (ref != null && deployRefPattern.matches(ref)) {
        logger.lifecycle("Job in $ref will deploy!")
        dependsOn(mavenCentralDeploy)
    } else {
        logger.lifecycle("Job will only build!")
        dependsOn(tasks.assemble)
    }
}
