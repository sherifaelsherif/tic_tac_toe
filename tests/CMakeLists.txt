# Enable testing
enable_testing()

# Add GoogleTest as a subdirectory
add_subdirectory(../googletest googletest-build)

# Add the test executable
add_executable(runTests test_math_utils.cpp ../math_utils.cpp)

# Link with GoogleTest libraries
target_link_libraries(runTests gtest gtest_main)

# Discover and register the tests
include(GoogleTest)
gtest_discover_tests(runTests)