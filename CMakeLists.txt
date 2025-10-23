cmake_minimum_required(VERSION 3.27)
project(
  NotificationService
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION "Microservice for sending notifications"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_MODULE_PATH}/cmake")
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tests)
