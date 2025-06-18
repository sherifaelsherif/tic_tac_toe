cmake_minimum_required(VERSION 3.16)
project(TicTacToe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---------------- Qt6 Setup ----------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Test)
if (NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found. Please set CMAKE_PREFIX_PATH to the Qt6 installation directory.")
endif()

# ---------------- Google Test Setup ----------------
# Add GoogleTest as a subdirectory (e.g., via submodule)
add_subdirectory(googletest EXCLUDE_FROM_ALL)
enable_testing()

# Optional include dirs (mainly for clarity)
set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include)
set(GMOCK_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock/include)

# ---------------- Shared Static Library ----------------
add_library(TicTacToeLib STATIC
    src/Game.cpp
    src/Database.cpp
    src/MainWindow.cpp
    src/AuthWindow.cpp
    src/RegisterWindow.cpp
)
target_include_directories(TicTacToeLib PUBLIC src)
target_link_libraries(TicTacToeLib PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

# ---------------- Main Executable ----------------
add_executable(TicTacToe src/main.cpp)
target_link_libraries(TicTacToe PRIVATE TicTacToeLib)

# ---------------- Add Test Subdirectory ----------------
add_subdirectory(tests)

