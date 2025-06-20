// mainwindow_test.cpp
// -----------------------------------------------
// Google-Test / Qt6 unit-tests for MainWindow
// -----------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSignalSpy>
#include <QSize>
#include <QTest>
#include <QString>

#include <memory>
#include <vector>

#include "MainWindow.h"
#include "Database.h"
#include "Game.h"

using ::testing::_;
using ::testing::Return;

// ========================================================
// Helper: guarantee a QApplication exists for all tests
// ========================================================
static int   argcDummy = 1;
static char  arg0[]    = "tst_mainwindow";
static char* argvDummy[] = { arg0, nullptr };

// A RAII wrapper so that the first test that needs a QApplication
// creates it, later tests simply reuse the global instance.
class QtAppGuard
{
public:
    QtAppGuard()
    {
        if (!QApplication::instance())
        {
            app_.reset(new QApplication(argcDummy, argvDummy));
        }
    }
private:
    std::unique_ptr<QApplication> app_;
};

// ========================================================
// Mock-Database implementing the interface used by MainWindow
// ========================================================
class MockDatabase : public Database
{
public:
    MOCK_METHOD(void,   saveGame,       (int userId, char board[3][3], const QString& result),           (override));
    MOCK_METHOD(QString,getGameHistory, (int userId),                                                   (override));
    MOCK_METHOD(bool,   createUser,     (const QString& username, const QString& password),             (override));
    MOCK_METHOD(int,    authenticateUser,(const QString& username, const QString& password),            (override));
};

// ========================================================
// Test-Fixture
// ========================================================
class MainWindowTest : public ::testing::Test
{
protected:
    QtAppGuard                    appGuard;   // guarantees QApplication
    std::unique_ptr<MockDatabase> mockDb;
    MainWindow*                   mainWindow = nullptr;

    void SetUp() override
    {
        mockDb = std::make_unique<MockDatabase>();
    }

    void TearDown() override
    {
        if (mainWindow)
        {
            mainWindow->hide();
            mainWindow->close();
            mainWindow->deleteLater();
            mainWindow = nullptr;
        }
        // Let Qt finish deleting objects that have deleteLater()
        QApplication::processEvents();
    }

    // Helper that actually instantiates the window in “test mode”
    void createMainWindow(int userId = 1)
    {
        mainWindow = new MainWindow(userId, mockDb.get(), /*testMode =*/ true);
        mainWindow->hide();                // keep it off-screen
        QApplication::processEvents();     // ensure widgets are built
    }

    // Convenience helpers ---------------------------------------------------
    QPushButton* findButtonByText(const QString& pattern)
    {
        const auto buttons = mainWindow->findChildren<QPushButton*>();
        for (auto* b : buttons)
        {
            if (b->text().toUpper().contains(pattern.toUpper()))
                return b;
        }
        return nullptr;
    }

    QList<QPushButton*> gameBoardCells()   // cells are named "cell_<r>_<c>"
    {
        QList<QPushButton*> cells;
        const auto buttons = mainWindow->findChildren<QPushButton*>();
        for (auto* b : buttons)
            if (b->objectName().startsWith("cell_"))
                cells.append(b);
        return cells;
    }
};

// ========================================================
//                BASIC CONSTRUCTION TESTS
// ========================================================
TEST_F(MainWindowTest, ConstructorInitialises)
{
    createMainWindow(1);
    ASSERT_NE(mainWindow, nullptr);
    EXPECT_FALSE(mainWindow->isVisible());
    EXPECT_TRUE(mainWindow->inherits("QMainWindow"));
    EXPECT_NE(mainWindow->centralWidget(), nullptr);
}

TEST_F(MainWindowTest, ConstructorWorksWithGuestId)
{
    createMainWindow(-1);
    ASSERT_NE(mainWindow, nullptr);
    EXPECT_FALSE(mainWindow->isVisible());
    EXPECT_GT(mainWindow->findChildren<QWidget*>().size(), 0);
}

// ========================================================
//                UI COMPONENT PRESENCE
// ========================================================
TEST_F(MainWindowTest, TitleLabelExists)
{
    createMainWindow();
    bool hasTitle = false;
    const auto labels = mainWindow->findChildren<QLabel*>();
    for (auto* l : labels)
    {
        const QString text = l->text().toUpper();
        if (text.contains("TIC") && text.contains("TAC") && text.contains("TOE"))
        {
            hasTitle = true;
            EXPECT_FALSE(text.trimmed().isEmpty());
            break;
        }
    }
    EXPECT_TRUE(hasTitle);
}

TEST_F(MainWindowTest, EssentialControlButtonsExist)
{
    createMainWindow();
    EXPECT_NE(findButtonByText("VS AI"),      nullptr);
    EXPECT_NE(findButtonByText("VS PLAYER"),  nullptr);
    EXPECT_NE(findButtonByText("RESTART"),    nullptr);
    EXPECT_NE(findButtonByText("HISTORY"),    nullptr);
    EXPECT_NE(findButtonByText("LOGOUT"),     nullptr);
}

// ========================================================
//                GAME BOARD PROPERTIES
// ========================================================
TEST_F(MainWindowTest, BoardHasNineCellsInitiallyEmpty)
{
    createMainWindow();
    const auto cells = gameBoardCells();
    ASSERT_EQ(cells.size(), 9);

    for (auto* c : cells)
    {
        EXPECT_TRUE(c->text().trimmed().isEmpty());
        EXPECT_TRUE(c->isEnabled());               // enabled in test-mode
    }
}

// ========================================================
//                WINDOW SIZE & LAYOUT
// ========================================================
TEST_F(MainWindowTest, WindowHasReasonableSize)
{
    createMainWindow();
    const QSize sz = mainWindow->size();
    EXPECT_GT(sz.width(),  200);
    EXPECT_GT(sz.height(), 200);
    EXPECT_LT(sz.width(),  2000);
    EXPECT_LT(sz.height(), 2000);
}

TEST_F(MainWindowTest, CentralWidgetAndLayoutExist)
{
    createMainWindow();
    QWidget* cw = mainWindow->centralWidget();
    ASSERT_NE(cw, nullptr);
    EXPECT_NE(cw->layout(), nullptr);
    EXPECT_GT(cw->layout()->count(), 0);
}

// ========================================================
//                BUTTON SANITY
// ========================================================
TEST_F(MainWindowTest, AllButtonsEmitClicked)
{
    createMainWindow();
    const auto buttons = mainWindow->findChildren<QPushButton*>();
    for (auto* b : buttons)
    {
        QSignalSpy spy(b,&QPushButton::clicked);
        EXPECT_TRUE(spy.isValid()) << "clicked() missing for button: "
                                   << b->text().toStdString();
    }
}

TEST_F(MainWindowTest, ButtonsHaveReasonableSizeAndText)
{
    createMainWindow();
    const auto buttons = mainWindow->findChildren<QPushButton*>();
    for (auto* b : buttons)
    {
        const QSize sz = b->size();
        EXPECT_GT(sz.width(),  10);
        EXPECT_GT(sz.height(), 10);
        EXPECT_LT(sz.width(),  1000);
        EXPECT_LT(sz.height(), 1000);

        const QString txt = b->text();
        if (!txt.isEmpty())
            EXPECT_LT(txt.length(), 50);
    }
}
