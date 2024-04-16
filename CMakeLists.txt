cmake_minimum_required(VERSION 3.23)

project(ssq VERSION 3.0.1)
add_library(ssq)

set_target_properties(ssq PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

if (UNIX)
    target_compile_definitions(ssq PRIVATE _POSIX_C_SOURCE=200112L)
endif (UNIX)

target_include_directories(ssq PUBLIC include)
target_sources(ssq PUBLIC FILE_SET HEADERS BASE_DIRS include)

add_subdirectory(src)
add_subdirectory(include)

install(TARGETS ssq LIBRARY FILE_SET HEADERS)
