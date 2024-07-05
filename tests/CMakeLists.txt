include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "main"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(ssq_test src/test_packet.cc)
set_target_properties(ssq_test PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(ssq_test PRIVATE ../include)
target_link_libraries(ssq_test GTest::gtest_main)
target_link_libraries(ssq_test ssq)

file(COPY bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(GoogleTest)
gtest_discover_tests(ssq_test)
