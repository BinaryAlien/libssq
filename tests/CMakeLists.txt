cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(TESTS_SRC
    src/a2s/test_info.c
    src/a2s/test_player.c
    src/a2s/test_rules.c
    src/helper.c
    src/test_buf.c
    src/test_error.c
    src/test_packet.c
    src/test_response.c
    src/test_ssq.c
)

set(LIB_SRC
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

include_directories(include)
include_directories(../include)

add_executable(tests ${TESTS_SRC} ${LIB_SRC})

target_link_libraries(tests criterion)
