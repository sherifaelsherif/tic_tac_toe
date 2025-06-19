# ---------------- Source Files for Each Test ----------------
set(GAME_TEST_SOURCES game_test.cpp)
set(MAINWINDOW_TEST_SOURCES mainwindow_test.cpp)
set(DATABASE_TEST_SOURCES database_test.cpp)
set(REGISTERWINDOW_TEST_SOURCES registerwindow_test.cpp)

# ---------------- Common Include Dirs ----------------
set(TEST_INCLUDE_DIRS
    ../src
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
)

# ---------------- Add pthread (required by GTest static libs) ----------------
find_package(Threads REQUIRED)

# ---------------- Common Libraries to Link ----------------
set(COMMON_TEST_LIBS
    TicTacToeLib
    gtest
    gtest_main
    gmock
    gmock_main
    Threads::Threads
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Test
)

# ---------------- Game Test ----------------
add_executable(testGame ${GAME_TEST_SOURCES})
set_target_properties(testGame PROPERTIES AUTOMOC ON)
target_include_directories(testGame PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(testGame PRIVATE ${COMMON_TEST_LIBS})
add_test(NAME GameTests COMMAND testGame)

# ---------------- Database Test ----------------
add_executable(testDatabase ${DATABASE_TEST_SOURCES})
set_target_properties(testDatabase PROPERTIES AUTOMOC ON)
target_include_directories(testDatabase PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(testDatabase PRIVATE ${COMMON_TEST_LIBS})
add_test(NAME DatabaseTests COMMAND testDatabase)

# ---------------- RegisterWindow Test ----------------
add_executable(testRegisterWindow ${REGISTERWINDOW_TEST_SOURCES})
set_target_properties(testRegisterWindow PROPERTIES AUTOMOC ON)
target_include_directories(testRegisterWindow PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(testRegisterWindow PRIVATE ${COMMON_TEST_LIBS})
add_test(NAME RegisterWindowTests COMMAND testRegisterWindow)

# ---------------- MainWindow Test (optional) ----------------
 add_executable(testMainWindow ${MAINWINDOW_TEST_SOURCES})
 set_target_properties(testMainWindow PROPERTIES AUTOMOC ON)
 target_include_directories(testMainWindow PRIVATE ${TEST_INCLUDE_DIRS})
 target_link_libraries(testMainWindow PRIVATE ${COMMON_TEST_LIBS})
 add_test(NAME MainWindowTests COMMAND testMainWindow)

