from conan import ConanFile


class LibDataChannel(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    requires = "openssl/3.5.0"
    generators = "CMakeDeps"

    def configure(self):
        self.options["openssl"].shared = self.settings.os != "Windows" and self.settings.os != "Macos"
