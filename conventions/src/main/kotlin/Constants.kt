object Constants {
    const val ARCH_DETECT_CONFIG = "archDetectConfiguration"
    const val ANDROID_CONFIG = "androidConfiguration"
    const val ANDROID_CLASSIFIER_PREFIX = "android-"
    const val WINDOWS_CLASSIFIER_PREFIX = "windows-"
    const val MACOS_CLASSIFIER_PREFIX = "macos-"

    const val SNAPSHOTS_REPO = "mavenCentralSnapshots"
    const val RELEASES_REPO = "mavenLocal"

    val CI = System.getenv("CI") != null
}