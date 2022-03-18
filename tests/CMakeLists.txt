cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(LIBRARY_SOURCES
    ../src/a2s/info.c
    ../src/a2s/player.c
    ../src/a2s/rules.c
    ../src/buf.c
    ../src/error.c
    ../src/packet.c
    ../src/packet.c
    ../src/query.c
    ../src/response.c
    ../src/ssq.c
)

project(tests)

include_directories(../include)

add_executable(tests
    ${LIBRARY_SOURCES}
    src/a2s/test_info.c
    src/a2s/test_player.c
    src/a2s/test_rules.c
    src/test_error.c
    src/test_packet.c
    src/test_response.c
    src/test_ssq.c
)

target_link_libraries(tests criterion)
