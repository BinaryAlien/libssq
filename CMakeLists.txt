cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if (UNIX)
    add_definitions(-D_POSIX_C_SOURCE=200112L)
endif (UNIX)

project(ssq VERSION 2.0.0)

include_directories(include)

add_library(ssq STATIC
    src/a2s/info.c
    src/a2s/player.c
    src/a2s/rules.c
    src/buf.c
    src/error.c
    src/packet.c
    src/query.c
    src/response.c
    src/server.c
)
