# tests/CMakeLists.txt
set(GAME_TEST_SOURCES game_test.cpp)
set(MAINWINDOW_TEST_SOURCES mainwindow_test.cpp)
set(DATABASE_TEST_SOURCES database_test.cpp)
set(REGISTERWINDOW_TEST_SOURCES registerwindow_test.cpp)

# Test executable for game tests
add_executable(testGame ${GAME_TEST_SOURCES})
set_target_properties(testGame PROPERTIES AUTOMOC ON)
target_include_directories(testGame PRIVATE ../src ${GTEST_INCLUDE_DIRS})
target_link_libraries(testGame PRIVATE TicTacToeLib gtest gtest_main Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
set_target_properties(testGame PROPERTIES LINK_FLAGS "-mconsole")
add_test(NAME GameTests COMMAND testGame)

# Test executable for main window tests
# add_executable(testMainWindow ${MAINWINDOW_TEST_SOURCES})
# set_target_properties(testMainWindow PROPERTIES AUTOMOC ON)
# target_include_directories(testMainWindow PRIVATE ../src ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
# target_link_libraries(testMainWindow PRIVATE TicTacToeLib gmock gmock_main gtest Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Test)
# set_target_properties(testMainWindow PROPERTIES LINK_FLAGS "-mconsole")
# add_test(NAME MainWindowTests COMMAND testMainWindow)

# Test executable for database tests
add_executable(testDatabase ${DATABASE_TEST_SOURCES})
set_target_properties(testDatabase PROPERTIES AUTOMOC ON)
target_include_directories(testDatabase PRIVATE ../src ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
target_link_libraries(testDatabase PRIVATE TicTacToeLib gtest gtest_main gmock gmock_main Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Test)
set_target_properties(testDatabase PROPERTIES LINK_FLAGS "-mconsole")
add_test(NAME DatabaseTests COMMAND testDatabase)

# Test executable for register window tests
add_executable(testRegisterWindow ${REGISTERWINDOW_TEST_SOURCES})
set_target_properties(testRegisterWindow PROPERTIES AUTOMOC ON)
target_include_directories(testRegisterWindow PRIVATE ../src)
target_link_libraries(testRegisterWindow PRIVATE TicTacToeLib Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Test)
set_target_properties(testRegisterWindow PROPERTIES LINK_FLAGS "-mconsole")
add_test(NAME RegisterWindowTests COMMAND testRegisterWindow)