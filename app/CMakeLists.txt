set(App)
add_executable(App)
target_sources(
  App
  PRIVATE
  FILE_SET app_headers
  TYPE HEADERS
  BASE_DIRS
  "${CMAKE_SOURCE_DIR}/include"
  FILES
  "${CMAKE_SOURCE_DIR}/include/notification_service/core.hpp"
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
  CoreLib
)
