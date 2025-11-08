cmake_minimum_required(VERSION 3.30)
project(
  NotificationService
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION "Microservice for sending notifications"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_MODULE_PATH}/cmake")
set(CMAKE_CXX_EXTENSIONS OFF)

if($ENV{DEV})
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
  find_program(CCACHE_EXE NAMES "ccache" REQUIRED)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXE}")
endif()

set(Boost_USE_DEBUG_LIBS ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.88.0 REQUIRED)
find_package(spdlog REQUIRED)

include(FindPkgConfig)
pkg_check_modules(liburing REQUIRED IMPORTED_TARGET GLOBAL liburing>=2.0)

add_subdirectory(app)
add_subdirectory(tests)
add_subdirectory(modules)
