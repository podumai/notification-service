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
  CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
)
target_link_libraries(
  App
  PRIVATE
  NotificationServiceModule::NotificationServiceModule
)
