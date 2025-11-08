from conan import ConanFile
from conan.tools.cmake import cmake_layout

class NotificationServiceDeps(ConanFile):
    settings: tuple[str] = ("os", "compiler", "build_type", "arch")
    generators: tuple[str] = ("CMakeToolchain", "CMakeDeps")

    def requirements(self) -> None:
        self.requires("jemalloc/5.3.0")
        self.requires("spdlog/1.16.0")

    def build_requirements(self) -> None:
        self.tool_requires("cmake/3.30.0")

    def layout(self) -> None:
        cmake_layout(self)
