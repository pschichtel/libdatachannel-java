import org.gradle.internal.declarativedsl.intrinsics.listOf
import org.gradle.kotlin.dsl.support.serviceOf
import tel.schich.dockcross.execute.DockerRunner
import tel.schich.dockcross.execute.NonContainerRunner
import tel.schich.dockcross.execute.RemoteSshRunner
import tel.schich.dockcross.execute.SubstitutingString
import tel.schich.dockcross.tasks.DockcrossRunTask
import java.net.URI
import java.nio.file.Files

plugins {
    id("tel.schich.libdatachannel.convention.common")
    alias(libs.plugins.dockcross)
    alias(libs.plugins.nexusPublish)
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
description = "${project.name} is a binding to the libdatachannel that feels native to Java developers."

val currentVersion by tasks.registering(DefaultTask::class) {
    doLast {
        println(version)
    }
}

val archDetectConfiguration by configurations.registering {
    isCanBeConsumed = true
}

val jniPath = project.layout.projectDirectory.dir("jni")
tasks.withType<JavaCompile>().configureEach {
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

    image = target.image
    dockcrossTag = "20250109-7bf589c"
    inputs.dir(jniPath)

    dependsOn(tasks.compileJava)

    output = outputTo.dir("native")
    val conanDir = "conan"
    extraEnv.putAll(target.env)
    extraEnv.put("CONAN_HOME", SubstitutingString("\${OUTPUT_DIR}/$conanDir/home"))
    // OpenSSL's makefile constructs broken compiler paths due to CROSS_COMPILE
    extraEnv.put("CROSS_COMPILE", "")

    val relativePathToProject = output.get().asFile.toPath().relativize(jniPath.asFile.toPath()).toString()
    val projectVersionOption = "-DPROJECT_VERSION=${project.version}"
    val releaseOption = "-DCMAKE_BUILD_TYPE=${if (buildReleaseBinaries) "Release" else "Debug"}"
    val conanProviderOption = SubstitutingString("-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=\${MOUNT_SOURCE}/jni/cmake-conan/conan_provider.cmake")
    val policyVersionOption = "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
    script = listOf(
        listOf("conan", "profile", "detect", "-f"),
        listOf("cmake", relativePathToProject, policyVersionOption, conanProviderOption, projectVersionOption, releaseOption) + target.args,
        listOf("make", "-j${project.gradle.startParameter.maxWorkerCount}"),
    )

    configureSshRemoteBuild(target)
}

fun Jar.baseConfigure(compileTask: TaskProvider<DockcrossRunTask>, buildOutputDir: Directory) {
    group = nativeGroup

    dependsOn(compileTask)

    from(buildOutputDir) {
        include("native/libdatachannel-java.so")
        include("native/libdatachannel-java.dll")
    }
}

val dockcrossOutputDir: Directory = project.layout.buildDirectory.get().dir("dockcross")
val nativeForHostOutputDir: Directory = dockcrossOutputDir.dir("host")
val compileNativeForHost by tasks.registering(DockcrossRunTask::class) {
    baseConfigure(nativeForHostOutputDir, BuildTarget(image = "host", family = "host", classifier = "host"))
    unsafeWritableMountSource = true
    runner(NonContainerRunner)
}

val packageNativeForHost by tasks.registering(Jar::class) {
    baseConfigure(compileNativeForHost, nativeForHostOutputDir)
    archiveClassifier = "host"
}

data class BuildTarget(
    val image: String,
    val family: String,
    val classifier: String,
    val env: Map<String, String> = emptyMap(),
    val args: List<String> = emptyList(),
)

val targets = listOf(
    BuildTarget(image = "linux-x64", family = "linux", classifier = "x86_64"),
    BuildTarget(image = "linux-x86", family = "linux", classifier = "x86_32"),
    BuildTarget(image = "linux-arm64", family = "linux", classifier = "aarch64"),
    BuildTarget(image = "windows-static-x64", family = "windows", classifier = "windows-x86_64"),
    BuildTarget(image = "windows-static-x86", family = "windows", classifier = "windows-x86_32"),
)

val packageNativeAll by tasks.registering(DefaultTask::class) {
    group = nativeGroup
}

var previousCompileNative: TaskProvider<DockcrossRunTask>? = null
for (target in targets) {
    val outputDir: Directory = dockcrossOutputDir.dir(target.classifier)
    val taskSuffix = target.classifier.split("[_-]".toRegex())
        .joinToString(separator = "") { it.lowercase().replaceFirstChar(Char::uppercaseChar) }
    val compileNative = tasks.register("compileNativeFor$taskSuffix", DockcrossRunTask::class) {
        baseConfigure(outputDir, target)
        unsafeWritableMountSource = true
        containerName = "dockcross-${project.name}-${target.classifier}"
    }

    if (ci) {
        val previous = previousCompileNative
        compileNative {
            runner(DockerRunner())
            if (previous != null) {
                mustRunAfter(previous)
            }

            val execOps = project.serviceOf<ExecOperations>()
            doLast {
                execOps.exec {
                    commandLine("docker", "image", "prune", "-af")
                }
            }
        }

        previousCompileNative = compileNative
    }

    val packageNative = tasks.register("packageNativeFor$taskSuffix", Jar::class) {
        baseConfigure(compileNative, outputDir)
        archiveClassifier = target.classifier
    }

    publishing.publications.withType<MavenPublication>().configureEach {
        artifact(packageNative)
    }

    packageNativeAll.configure {
        dependsOn(packageNative)
    }

    artifacts.add(archDetectConfiguration.name, packageNative)
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

nexusPublishing {
    this.repositories {
        sonatype()
    }
}

val mavenCentralDeploy by tasks.registering(DefaultTask::class) {
    group = "publishing"
    val isSnapshot = project.version.toString().endsWith("-SNAPSHOT")

    val publishTasks = allprojects
        .flatMap { it.tasks.withType<PublishToMavenRepository>() }
        .filter { it.repository.name == "sonatype" }
    dependsOn(publishTasks)
    if (!isSnapshot) {
        dependsOn(tasks.closeAndReleaseStagingRepositories)
    }
}