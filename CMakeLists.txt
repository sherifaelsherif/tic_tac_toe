 cmake_minimum_required(VERSION 3.16)
project(TicTacToe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Test)

# Add Google Test submodule
add_subdirectory(googletest EXCLUDE_FROM_ALL)
set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include)
set(GMOCK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock/include)

# Enable testing
enable_testing()

# Define a static library for shared code
add_library(TicTacToeLib STATIC
    src/Game.cpp
    src/Database.cpp
    src/MainWindow.cpp
    src/AuthWindow.cpp
    src/RegisterWindow.cpp
)
set_target_properties(TicTacToeLib PROPERTIES AUTOMOC ON)
target_include_directories(TicTacToeLib PUBLIC src)
target_link_libraries(TicTacToeLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

# Define the main application executable
add_executable(TicTacToe src/main.cpp)
set_target_properties(TicTacToe PROPERTIES AUTOMOC ON)
target_link_libraries(TicTacToe PRIVATE TicTacToeLib)

# Add tests subdirectory
add_subdirectory(tests)

 

