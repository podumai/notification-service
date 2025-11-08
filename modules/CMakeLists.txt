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
  PkgConfig::liburing
  spdlog::spdlog
  Boost::boost
)
