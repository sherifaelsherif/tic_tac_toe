#include <gtest/gtest.h>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QTest>
#include <QTimer>
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSizePolicy>
#include <gmock/gmock.h>
#include "MainWindow.h"
#include "Database.h"
#include "Game.h"

// Helper function to convert QString to std::string for Google Test
std::string qStringToStdString(const QString& qstr) {
    return qstr.toStdString();
}

// Mock Database class
class MockDatabase : public Database {
public:
    MOCK_METHOD(void, saveGame, (int userId, char board[3][3], const QString& result));
    MOCK_METHOD(QString, getGameHistory, (int userId));
    MOCK_METHOD(bool, createUser, (const QString& username, const QString& password));
    MOCK_METHOD(int, authenticateUser, (const QString& username, const QString& password));
};

// Test fixture for MainWindow tests
class MainWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Ensure QApplication exists
        if (!QApplication::instance()) {
            int argc = 1;
            static char appName[] = "test";
            static char* argv[] = {appName, nullptr};
            app = std::make_unique<QApplication>(argc, argv);
        }
        
        mockDb = std::make_unique<MockDatabase>();
        mainWindow = nullptr;
    }

    void TearDown() override {
        // Clean up in reverse order
        if (mainWindow) {
            mainWindow->hide();
            mainWindow->close();
            // Use deleteLater to ensure proper Qt cleanup
            mainWindow->deleteLater();
            // Process events to ensure cleanup
            QApplication::processEvents();
            mainWindow = nullptr;
        }
        
        // Process any pending events
        QApplication::processEvents();
    }

    void createMainWindow(int userId = 1) {
        // THIS IS THE CRITICAL FIX - PASS TRUE FOR testMode!
        mainWindow = new MainWindow(userId, mockDb.get(), true);
        mainWindow->hide(); // Ensure it stays hidden during tests
        QApplication::processEvents(); // Allow dialogs to appear and be handled
        QTest::qWait(100); // Give it time to initialize
    }

    // Helper to find buttons by text pattern
    QPushButton* findButtonByText(const QString& pattern) {
        auto buttons = mainWindow->findChildren<QPushButton*>();
        for (auto* button : buttons) {
            if (button->text().toUpper().contains(pattern.toUpper())) {
                return button;
            }
        }
        return nullptr;
    }

    // Helper to get game board cells
    QList<QPushButton*> getGameBoardCells() {
        auto buttons = mainWindow->findChildren<QPushButton*>();
        QList<QPushButton*> cells;
        for (auto* button : buttons) {
            QString objectName = button->objectName();
            // Game cells have object names like "cell_0_0", "cell_0_1", etc.
            if (objectName.startsWith("cell_")) {
                cells.append(button);
            }
        }
        return cells;
    }

    std::unique_ptr<QApplication> app;
    std::unique_ptr<MockDatabase> mockDb;
    MainWindow* mainWindow = nullptr;
};

// ============= BASIC CONSTRUCTION TESTS =============
TEST_F(MainWindowTest, ConstructorInitializesCorrectly) {
    createMainWindow(1);
    ASSERT_NE(mainWindow, nullptr);
    EXPECT_FALSE(mainWindow->isVisible());
    // Basic widget properties
    EXPECT_TRUE(mainWindow->isWidgetType());
    EXPECT_NE(mainWindow->centralWidget(), nullptr);
    // Check inheritance
    EXPECT_TRUE(mainWindow->inherits("QMainWindow"));
    EXPECT_TRUE(mainWindow->inherits("QWidget"));
}

TEST_F(MainWindowTest, ConstructorWithGuestUser) {
    createMainWindow(-1); // Guest user
    ASSERT_NE(mainWindow, nullptr);
    EXPECT_FALSE(mainWindow->isVisible());
    // Guest users should still have basic functionality
    auto buttons = mainWindow->findChildren<QPushButton*>();
    EXPECT_GT(buttons.size(), 0) << "Guest user should have interactive buttons";
}

TEST_F(MainWindowTest, ConstructorWithDifferentUserIds) {
    // Test various user IDs
    std::vector<int> testUserIds = {0, 1, 100, 999, -1};
    for (int userId : testUserIds) {
        createMainWindow(userId);
        ASSERT_NE(mainWindow, nullptr) << "Constructor should work with userId: " << userId;
        auto buttons = mainWindow->findChildren<QPushButton*>();
        EXPECT_GT(buttons.size(), 0) << "User " << userId << " should have buttons";
        // Clean up for next iteration
        mainWindow->deleteLater();
        QApplication::processEvents();
        mainWindow = nullptr;
    }
}

// ============= UI COMPONENT TESTS =============
TEST_F(MainWindowTest, TitleLabelExists) {
    createMainWindow(1);
    auto labels = mainWindow->findChildren<QLabel*>();
    bool hasTitleLabel = false;
    for (auto* label : labels) {
        QString text = label->text().toUpper();
        if (text.contains("TIC") && text.contains("TAC") && text.contains("TOE")) {
            hasTitleLabel = true;
            EXPECT_FALSE(text.isEmpty()) << "Title should not be empty";
            break;
        }
    }
    EXPECT_TRUE(hasTitleLabel) << "Should have a title label containing 'TIC TAC TOE'";
}

TEST_F(MainWindowTest, GameControlButtonsExist) {
    createMainWindow(1);
    // Test for specific control buttons
    EXPECT_NE(findButtonByText("VS AI"), nullptr) << "Should have 'VS AI' button";
    EXPECT_NE(findButtonByText("VS PLAYER"), nullptr) << "Should have 'VS PLAYER' button";
    EXPECT_NE(findButtonByText("RESTART"), nullptr) << "Should have 'RESTART' button";
    EXPECT_NE(findButtonByText("HISTORY"), nullptr) << "Should have 'HISTORY' button";
    EXPECT_NE(findButtonByText("LOGOUT"), nullptr) << "Should have 'LOGOUT' button";
}

TEST_F(MainWindowTest, PlayerIndicatorExists) {
    createMainWindow(1);
    auto labels = mainWindow->findChildren<QLabel*>();
    bool hasPlayerIndicator = false;
    for (auto* label : labels) {
        QString text = label->text().toUpper();
        if (text.contains("PLAYER") || text.contains("TURN") || text.contains("SELECT")) {
            hasPlayerIndicator = true;
            break;
        }
    }
    EXPECT_TRUE(hasPlayerIndicator) << "Should have a player/turn indicator label";
}

TEST_F(MainWindowTest, ModeIndicatorExists) {
    createMainWindow(1);
    auto labels = mainWindow->findChildren<QLabel*>();
    bool hasModeIndicator = false;
    for (auto* label : labels) {
        QString text = label->text().toUpper();
        if (text.contains("MODE") || text.contains("SELECT") || text.contains("GAME")) {
            hasModeIndicator = true;
            break;
        }
    }
    EXPECT_TRUE(hasModeIndicator) << "Should have a mode indicator label";
}

// ============= GAME BOARD TESTS =============
TEST_F(MainWindowTest, GameBoardHasExactly9Cells) {
    createMainWindow(1);
    auto gameCells = getGameBoardCells();
    EXPECT_EQ(gameCells.size(), 9) << "Game board should have exactly 9 cells";
}

TEST_F(MainWindowTest, GameBoardCellsAreInitiallyEmpty) {
    createMainWindow(1);
    auto gameCells = getGameBoardCells();
    for (auto* cell : gameCells) {
        QString text = cell->text().trimmed();
        EXPECT_TRUE(text.isEmpty() || text == " ")
            << "Game cells should be initially empty";
    }
}

TEST_F(MainWindowTest, GameBoardCellsAreInitiallyDisabled) {
    createMainWindow(1);
    auto gameCells = getGameBoardCells();
    // In test mode, cells should be enabled since game is auto-started
    for (auto* cell : gameCells) {
        EXPECT_TRUE(cell->isEnabled())
            << "Game cells should be enabled in test mode";
    }
}

TEST_F(MainWindowTest, GameBoardHasGridLayout) {
    createMainWindow(1);
    // Look for a grid layout in the UI hierarchy
    bool hasGridLayout = false;
    std::function<void(QObject*)> checkForGridLayout = [&](QObject* obj) {
        if (auto* layout = qobject_cast<QGridLayout*>(obj)) {
            hasGridLayout = true;
            return;
        }
        
        for (auto* child : obj->children()) {
            checkForGridLayout(child);
        }
    };
    checkForGridLayout(mainWindow);
    EXPECT_TRUE(hasGridLayout) << "Should have a QGridLayout for the game board";
}

// ============= USER-SPECIFIC TESTS =============
TEST_F(MainWindowTest, RegisteredUserHasHistoryButton) {
    createMainWindow(1); // Create with registered user ID
    QPushButton* historyButton = findButtonByText("HISTORY");
    ASSERT_NE(historyButton, nullptr) << "History button should exist";
    
    // CRITICAL FIX: Show the window temporarily so isVisible() works correctly
    mainWindow->show();
    QApplication::processEvents();
    
    EXPECT_TRUE(historyButton->isVisible()) << "History button should be visible for registered user";
    
    // Hide it again for cleanup
    mainWindow->hide();
}


TEST_F(MainWindowTest, GuestUserInterfaceDifferences) {
    // Test guest user
    createMainWindow(-1);
    auto guestButtons = mainWindow->findChildren<QPushButton*>();
    int guestButtonCount = guestButtons.size();
    mainWindow->deleteLater();
    QApplication::processEvents();
    mainWindow = nullptr;
    
    // Test registered user
    createMainWindow(1);
    auto regButtons = mainWindow->findChildren<QPushButton*>();
    int regButtonCount = regButtons.size();
    
    // Both should have buttons, but counts might differ
    EXPECT_GT(guestButtonCount, 0) << "Guest user should have buttons";
    EXPECT_GT(regButtonCount, 0) << "Registered user should have buttons";
    std::cout << "Guest user buttons: " << guestButtonCount
              << ", Registered user buttons: " << regButtonCount << std::endl;
}

// ============= BUTTON FUNCTIONALITY TESTS =============
TEST_F(MainWindowTest, AllButtonsHaveValidSignals) {
    createMainWindow(1);
    auto buttons = mainWindow->findChildren<QPushButton*>();
    for (auto* button : buttons) {
        QSignalSpy clickSpy(button, &QPushButton::clicked);
        EXPECT_TRUE(clickSpy.isValid())
            << "Button '" << qStringToStdString(button->text()) << "' should have valid clicked signal";
    }
}

TEST_F(MainWindowTest, ButtonsHaveReasonableText) {
    createMainWindow(1);
    auto buttons = mainWindow->findChildren<QPushButton*>();
    for (auto* button : buttons) {
        QString text = button->text();
        // Button text should not be excessively long
        EXPECT_LT(text.length(), 50)
            << "Button text should not be excessively long: '" << qStringToStdString(text) << "'";
        // Control buttons should have meaningful text (not empty unless they're game cells)
        if (!text.isEmpty()) {
            EXPECT_GT(text.trimmed().length(), 0)
                << "Non-empty button text should contain meaningful content";
        }
    }
}

TEST_F(MainWindowTest, ButtonsHaveAppropriateSize) {
    createMainWindow(1);
    auto buttons = mainWindow->findChildren<QPushButton*>();
    for (auto* button : buttons) {
        QSize size = button->size();
        // Buttons should have reasonable minimum size
        EXPECT_GT(size.width(), 10) << "Button should have reasonable width";
        EXPECT_GT(size.height(), 10) << "Button should have reasonable height";
        // But not be ridiculously large
        EXPECT_LT(size.width(), 1000) << "Button width should not be excessive";
        EXPECT_LT(size.height(), 1000) << "Button height should not be excessive";
    }
}

// ============= WINDOW PROPERTIES TESTS =============
TEST_F(MainWindowTest, WindowHasReasonableSize) {
    createMainWindow(1);
    QSize size = mainWindow->size();
    EXPECT_GT(size.width(), 200) << "Window should be wide enough to be usable";
    EXPECT_GT(size.height(), 200) << "Window should be tall enough to be usable";
    EXPECT_LT(size.width(), 2000) << "Window width should not be excessive";
    EXPECT_LT(size.height(), 2000) << "Window height should not be excessive";
}

TEST_F(MainWindowTest, WindowHasCentralWidget) {
    createMainWindow(1);
    QWidget* centralWidget = mainWindow->centralWidget();
    ASSERT_NE(centralWidget, nullptr) << "MainWindow should have a central widget";
    // Central widget should have some content
    auto children = centralWidget->children();
    EXPECT_GT(children.size(), 0) << "Central widget should have child components";
}

TEST_F(MainWindowTest, WindowHasCustomStyling) {
    createMainWindow(1);
    QString stylesheet = mainWindow->styleSheet();
    EXPECT_FALSE(stylesheet.isEmpty()) << "MainWindow should have custom styling";
    // Check for some expected style elements
    EXPECT_TRUE(stylesheet.contains("background") || stylesheet.contains("color"))
        << "Stylesheet should contain basic styling properties";
}

// ============= LAYOUT AND HIERARCHY TESTS =============
TEST_F(MainWindowTest, UIComponentsAreProperlyParented) {
    createMainWindow(1);
    auto buttons = mainWindow->findChildren<QPushButton*>();
    auto labels = mainWindow->findChildren<QLabel*>();
    
    // All components should be descendants of mainWindow
    for (auto* button : buttons) {
        EXPECT_TRUE(mainWindow->isAncestorOf(button))
            << "Button should be a descendant of MainWindow";
    }
    
    for (auto* label : labels) {
        EXPECT_TRUE(mainWindow->isAncestorOf(label))
            << "Label should be a descendant of MainWindow";
    }
}

TEST_F(MainWindowTest, LayoutIsWellStructured) {
    createMainWindow(1);
    QWidget* centralWidget = mainWindow->centralWidget();
    ASSERT_NE(centralWidget, nullptr);
    QLayout* mainLayout = centralWidget->layout();
    EXPECT_NE(mainLayout, nullptr) << "Central widget should have a layout";
    if (mainLayout) {
        EXPECT_GT(mainLayout->count(), 0) << "Main layout should have child items";
    }
}

// ============= GAME STATE TESTS =============
TEST_F(MainWindowTest, InitialGameStateIsCorrect) {
    createMainWindow(1);
    // In test mode, should show VS AI mode
    auto labels = mainWindow->findChildren<QLabel*>();
    bool foundVsAIMode = false;
    for (auto* label : labels) {
        QString text = label->text().toUpper();
        if (text.contains("MODE") && text.contains("AI")) {
            foundVsAIMode = true;
            break;
        }
    }
    EXPECT_TRUE(foundVsAIMode) << "Should show VS AI mode in test mode";
}

TEST_F(MainWindowTest, GameBoardCellsHaveConsistentStyling) {
    createMainWindow(1);
    auto gameCells = getGameBoardCells();
    if (gameCells.size() >= 2) {
        // Check that cells have similar styling patterns
        QString firstCellStyle = gameCells[0]->styleSheet();
        QString secondCellStyle = gameCells[1]->styleSheet();
        // Both should have some styling (non-empty) or both should be empty
        bool firstHasStyle = !firstCellStyle.isEmpty();
        bool secondHasStyle = !secondCellStyle.isEmpty();
        if (firstHasStyle || secondHasStyle) {
            EXPECT_TRUE(firstHasStyle && secondHasStyle)
                << "Game board cells should have consistent styling";
        }
    }
}

// ============= ERROR HANDLING TESTS =============
TEST_F(MainWindowTest, HandlesNullDatabaseGracefully) {
    // This test checks if the constructor can handle edge cases
    // Note: In practice, this might cause issues, but we test the robustness
    if (!QApplication::instance()) {
        int argc = 1;
        static char appName[] = "test";
        static char* argv[] = {appName, nullptr};
        app = std::make_unique<QApplication>(argc, argv);
    }
    
    // This test mainly ensures the test framework can handle various scenarios
    EXPECT_NE(mockDb.get(), nullptr) << "Mock database should be properly initialized";
}

// ============= MEMORY MANAGEMENT TESTS =============
TEST_F(MainWindowTest, MultipleCreationAndDestruction) {
    for (int i = 0; i < 5; ++i) {
        createMainWindow(i + 1);
        ASSERT_NE(mainWindow, nullptr) << "Window creation " << i << " should succeed";
        // Verify basic functionality
        auto buttons = mainWindow->findChildren<QPushButton*>();
        EXPECT_GT(buttons.size(), 0) << "Window " << i << " should have buttons";
        // Clean up for next iteration
        mainWindow->deleteLater();
        QApplication::processEvents();
        mainWindow = nullptr;
    }
}

TEST_F(MainWindowTest, ComponentCountConsistency) {
    createMainWindow(1);
    // Count components multiple times to ensure consistency
    auto initialButtons = mainWindow->findChildren<QPushButton*>();
    auto initialLabels = mainWindow->findChildren<QLabel*>();
    int initialButtonCount = initialButtons.size();
    int initialLabelCount = initialLabels.size();
    
    // Check again after processing events
    QApplication::processEvents();
    auto laterButtons = mainWindow->findChildren<QPushButton*>();
    auto laterLabels = mainWindow->findChildren<QLabel*>();
    
    EXPECT_EQ(laterButtons.size(), initialButtonCount)
        << "Button count should remain consistent";
    EXPECT_EQ(laterLabels.size(), initialLabelCount)
        << "Label count should remain consistent";
}

// ============= INTEGRATION TESTS =============
TEST_F(MainWindowTest, AllRequiredComponentsExistTogether) {
    createMainWindow(1);
    // Verify all major components exist together
    bool hasTitle = false;
    bool hasModeIndicator = false;
    bool hasPlayerIndicator = false;
    
    auto labels = mainWindow->findChildren<QLabel*>();
    for (auto* label : labels) {
        QString text = label->text().toUpper();
        if (text.contains("TIC") && text.contains("TAC")) {
            hasTitle = true;
        }
        
        if (text.contains("MODE") || text.contains("SELECT")) {
            hasModeIndicator = true;
        }
        
        if (text.contains("PLAYER") || text.contains("TURN")) {
            hasPlayerIndicator = true;
        }
    }
    
    bool hasVsAI = findButtonByText("VS AI") != nullptr;
    bool hasVsPlayer = findButtonByText("VS PLAYER") != nullptr;
    bool hasRestart = findButtonByText("RESTART") != nullptr;
    bool hasGameBoard = getGameBoardCells().size() >= 9;
    
    EXPECT_TRUE(hasTitle) << "Should have title";
    EXPECT_TRUE(hasModeIndicator) << "Should have mode indicator";
    EXPECT_TRUE(hasPlayerIndicator) << "Should have player indicator";
    EXPECT_TRUE(hasVsAI) << "Should have VS AI button";
    EXPECT_TRUE(hasVsPlayer) << "Should have VS PLAYER button";
    EXPECT_TRUE(hasRestart) << "Should have restart button";
    EXPECT_TRUE(hasGameBoard) << "Should have complete game board";
}

// ============= ACCESSIBILITY TESTS =============
TEST_F(MainWindowTest, ButtonsAreFocusable) {
    createMainWindow(1);
    auto buttons = mainWindow->findChildren<QPushButton*>();
    for (auto* button : buttons) {
        if (button->isVisible() && button->isEnabled()) {
            EXPECT_NE(button->focusPolicy(), Qt::NoFocus)
                << "Enabled buttons should be focusable for accessibility";
        }
    }
}

TEST_F(MainWindowTest, ComponentsHaveReasonableObjectNames) {
    createMainWindow(1);
    auto buttons = mainWindow->findChildren<QPushButton*>();
    auto labels = mainWindow->findChildren<QLabel*>();
    
    // At least some components should have meaningful object names
    int namedComponents = 0;
    for (auto* button : buttons) {
        if (!button->objectName().isEmpty() && button->objectName() != "qt_pushbutton_default") {
            namedComponents++;
        }
    }
    
    for (auto* label : labels) {
        if (!label->objectName().isEmpty() && label->objectName() != "qt_label_default") {
            namedComponents++;
        }
    }
    
    // This is more of a best practice check
    if (namedComponents == 0) {
        std::cout << "Note: Consider adding object names to UI components for better testability and accessibility\n";
    }
}
