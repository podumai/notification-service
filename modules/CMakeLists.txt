add_library(ExceptionModule)
add_library(ExceptionModule::ExceptionModule ALIAS ExceptionModule)
target_sources(
  ExceptionModule
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/custom_throw_exception.cpp"
)
target_compile_features(
  ExceptionModule
  PRIVATE
  cxx_std_23
)
target_link_libraries(
  ExceptionModule
  PRIVATE
  Boost::boost
)

add_library(NotificationServiceModule)
add_library(NotificationServiceModule::NotificationServiceModule ALIAS NotificationServiceModule)
target_sources(
  NotificationServiceModule
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
  FILES "${CMAKE_SOURCE_DIR}/modules/notification_service.cppm"
)
target_compile_features(
  NotificationServiceModule
  PRIVATE
  cxx_std_23
)
target_link_libraries(
  NotificationServiceModule
  PRIVATE
  jemalloc::jemalloc
  ExceptionModule::ExceptionModule
  PkgConfig::liburing
  Boost::boost
  spdlog::spdlog
)
