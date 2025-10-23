set(CoreLib)
add_library(CoreLib STATIC)
target_sources(
  CoreLib
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/core.cpp"
)
target_compile_features(
  CoreLib
  PRIVATE
  cxx_std_23
)
