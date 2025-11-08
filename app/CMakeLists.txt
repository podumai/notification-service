add_executable(App)
target_sources(
  App
  PRIVATE
  "main.cpp"
)
target_compile_features(
  App
  PRIVATE
  cxx_std_23
)
set_target_properties(
  App
  PROPERTIES
  OUTPUT_NAME "notification-service"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
target_link_libraries(
  App
  PRIVATE
  NotificationServiceModule::NotificationServiceModule
)
