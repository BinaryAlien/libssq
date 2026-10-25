cmake_minimum_required(VERSION 3.23)

project(ssq VERSION 0.1.0 LANGUAGES C)

add_library(ssq)

set_target_properties(ssq
  PROPERTIES
    C_EXTENSIONS        OFF
    C_STANDARD          99
    C_STANDARD_REQUIRED ON
)

target_compile_definitions(ssq PUBLIC "$<$<PLATFORM_ID:Darwin,FreeBSD,Linux>:_POSIX_C_SOURCE=200809L>")
target_include_directories(ssq PRIVATE src)

add_subdirectory(include)
add_subdirectory(src)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
  TARGETS ssq
  EXPORT "${PROJECT_NAME}Targets"
  FILE_SET HEADERS
  LIBRARY
  RUNTIME
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CMAKE_PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)

install(
  EXPORT "${CMAKE_PROJECT_NAME}Targets"
  NAMESPACE "${CMAKE_PROJECT_NAME}::"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
)
