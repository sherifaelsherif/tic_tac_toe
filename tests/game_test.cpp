#include <gtest/gtest.h>
#include <QCoreApplication>
#include "Game.h"
#include "Database.h"

// Mock Database class for testing
class MockDatabase : public Database {
public:
    // Add any necessary mock methods here
    // For now, assuming Database doesn't need specific implementation for Game tests
};

class GameTest : public ::testing::Test {
protected:
    static int argc;
    static char** argv;
    static QCoreApplication* app;
    
    static void SetUpTestSuite() {
        // Initialize QCoreApplication before ANY QSqlDatabase usage
        app = new QCoreApplication(argc, argv);
    }
    
    static void TearDownTestSuite() {
        delete app;
        app = nullptr;
    }
    
    void SetUp() override {
        mockDb = new MockDatabase();
        game = new Game(mockDb);
    }

    void TearDown() override {
        delete game;
        delete mockDb;
    }

    Game* game;
    MockDatabase* mockDb;
    
      
    
     
    
     
    
     
};

// Initialize static members
int GameTest::argc = 1;
char* dummy = (char*)"test";
char** GameTest::argv = &dummy;
QCoreApplication* GameTest::app = nullptr;

// Test Game Constructor
TEST_F(GameTest, ConstructorInitializesCorrectly) {
    EXPECT_FALSE(game->isVsAI());
    
    char board[3][3];
    game->getBoard(board);
    
    // Verify board is empty
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(board[i][j], ' ');
        }
    }
}

// Test startGame method
TEST_F(GameTest, StartGameSetsVsAIFlag) {
    game->startGame(true);
    EXPECT_TRUE(game->isVsAI());
    
    game->startGame(false);
    EXPECT_FALSE(game->isVsAI());
}

TEST_F(GameTest, StartGameResetsBoard) {
    // Make some moves first
    game->makeMove(0, 0, 'X');
    game->makeMove(1, 1, 'O');
    
    // Start new game
    game->startGame(false);
    
    // Verify board is reset
    char board[3][3];
    game->getBoard(board);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(board[i][j], ' ');
        }
    }
}

// Test makeMove method
TEST_F(GameTest, MakeMoveValidPosition) {
    EXPECT_TRUE(game->makeMove(0, 0, 'X'));
    EXPECT_TRUE(game->makeMove(1, 1, 'O'));
    EXPECT_TRUE(game->makeMove(2, 2, 'X'));
    
    char board[3][3];
    game->getBoard(board);
    
    EXPECT_EQ(board[0][0], 'X');
    EXPECT_EQ(board[1][1], 'O');
    EXPECT_EQ(board[2][2], 'X');
}

TEST_F(GameTest, MakeMoveInvalidPositions) {
    // Test out of bounds positions
    EXPECT_FALSE(game->makeMove(-1, 0, 'X'));
    EXPECT_FALSE(game->makeMove(0, -1, 'X'));
    EXPECT_FALSE(game->makeMove(3, 0, 'X'));
    EXPECT_FALSE(game->makeMove(0, 3, 'X'));
    EXPECT_FALSE(game->makeMove(3, 3, 'X'));
}

TEST_F(GameTest, MakeMoveOccupiedPosition) {
    // Make initial move
    EXPECT_TRUE(game->makeMove(1, 1, 'X'));
    
    // Try to make move on occupied position
    EXPECT_FALSE(game->makeMove(1, 1, 'O'));
    
    // Verify original move is still there
    char board[3][3];
    game->getBoard(board);
    EXPECT_EQ(board[1][1], 'X');
}

// Test checkWin method
TEST_F(GameTest, CheckWinHorizontal) {
    // Test all horizontal wins
    for (int row = 0; row < 3; row++) {
        game->reset();
        game->makeMove(row, 0, 'X');
        game->makeMove(row, 1, 'X');
        game->makeMove(row, 2, 'X');
        
        EXPECT_TRUE(game->checkWin('X')) << "Failed to detect horizontal win in row " << row;
        EXPECT_FALSE(game->checkWin('O'));
    }
}

TEST_F(GameTest, CheckWinVertical) {
    // Test all vertical wins
    for (int col = 0; col < 3; col++) {
        game->reset();
        game->makeMove(0, col, 'O');
        game->makeMove(1, col, 'O');
        game->makeMove(2, col, 'O');
        
        EXPECT_TRUE(game->checkWin('O')) << "Failed to detect vertical win in column " << col;
        EXPECT_FALSE(game->checkWin('X'));
    }
}

TEST_F(GameTest, CheckWinDiagonal) {
    // Test main diagonal (top-left to bottom-right)
    game->reset();
    game->makeMove(0, 0, 'X');
    game->makeMove(1, 1, 'X');
    game->makeMove(2, 2, 'X');
    
    EXPECT_TRUE(game->checkWin('X'));
    EXPECT_FALSE(game->checkWin('O'));
    
    // Test anti-diagonal (top-right to bottom-left)
    game->reset();
    game->makeMove(0, 2, 'O');
    game->makeMove(1, 1, 'O');
    game->makeMove(2, 0, 'O');
    
    EXPECT_TRUE(game->checkWin('O'));
    EXPECT_FALSE(game->checkWin('X'));
}

TEST_F(GameTest, CheckWinNoWin) {
    // Set up a board with no winner
    game->makeMove(0, 0, 'X');
    game->makeMove(0, 1, 'O');
    game->makeMove(0, 2, 'X');
    game->makeMove(1, 0, 'O');
    game->makeMove(1, 1, 'X');
    
    EXPECT_FALSE(game->checkWin('X'));
    EXPECT_FALSE(game->checkWin('O'));
}

// Test isBoardFull method
TEST_F(GameTest, IsBoardFullEmpty) {
    EXPECT_FALSE(game->isBoardFull());
}

TEST_F(GameTest, IsBoardFullPartial) {
    game->makeMove(0, 0, 'X');
    game->makeMove(1, 1, 'O');
    
    EXPECT_FALSE(game->isBoardFull());
}

TEST_F(GameTest, IsBoardFullComplete) {
    // Fill the entire board
    game->makeMove(0, 0, 'X');
    game->makeMove(0, 1, 'O');
    game->makeMove(0, 2, 'X');
    game->makeMove(1, 0, 'O');
    game->makeMove(1, 1, 'X');
    game->makeMove(1, 2, 'O');
    game->makeMove(2, 0, 'X');
    game->makeMove(2, 1, 'O');
    game->makeMove(2, 2, 'X');
    
    EXPECT_TRUE(game->isBoardFull());
}

// Test reset method
TEST_F(GameTest, ResetClearsBoard) {
    // Fill some positions
    game->makeMove(0, 0, 'X');
    game->makeMove(1, 1, 'O');
    game->makeMove(2, 2, 'X');
    
    // Reset and verify
    game->reset();
    
    char board[3][3];
    game->getBoard(board);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(board[i][j], ' ');
        }
    }
}

// Test AI move functionality
TEST_F(GameTest, AIMoveBlocksWinningMove) {
    // Set up scenario where player is about to win
    game->makeMove(0, 0, 'X');  // Human player
    game->makeMove(0, 1, 'X');  // Human player
    // Position (0,2) should be blocked by AI
    
    game->aiMove('O');
    
    char board[3][3];
    game->getBoard(board);
    
    // AI should have blocked at (0,2)
    EXPECT_EQ(board[0][2], 'O');
}

TEST_F(GameTest, AIMoveWinsWhenPossible) {
    // Set up scenario where AI can win
    game->makeMove(0, 0, 'O');  // AI move (simulated)
    game->makeMove(0, 1, 'O');  // AI move (simulated)
    game->makeMove(1, 0, 'X');  // Human move
    
    game->aiMove('O');
    
    char board[3][3];
    game->getBoard(board);
    
    // AI should win at (0,2)
    EXPECT_EQ(board[0][2], 'O');
    EXPECT_TRUE(game->checkWin('O'));
}

TEST_F(GameTest, AIMakesMoveOnEmptyBoard) {
    game->aiMove('X');
    
    char board[3][3];
    game->getBoard(board);
    
    // Count number of X's on board
    int xCount = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 'X') {
                xCount++;
            }
        }
    }
    
    EXPECT_EQ(xCount, 1);
}

// Test getBoard method
TEST_F(GameTest, GetBoardCopiesCorrectly) {
    // Set up a specific board state
    game->makeMove(0, 0, 'X');
    game->makeMove(1, 1, 'O');
    game->makeMove(2, 2, 'X');
    
    char board[3][3];
    game->getBoard(board);
    
    EXPECT_EQ(board[0][0], 'X');
    EXPECT_EQ(board[1][1], 'O');
    EXPECT_EQ(board[2][2], 'X');
    
    // Verify other positions are empty
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if ((i == 0 && j == 0) || (i == 1 && j == 1) || (i == 2 && j == 2)) {
                continue;  // Skip the positions we set
            }
            EXPECT_EQ(board[i][j], ' ');
        }
    }
}

// Integration tests
TEST_F(GameTest, CompleteGameScenario) {
    game->startGame(false);  // Human vs Human
    
    // Play a complete game
    EXPECT_TRUE(game->makeMove(0, 0, 'X'));
    EXPECT_FALSE(game->checkWin('X'));
    EXPECT_FALSE(game->isBoardFull());
    
    EXPECT_TRUE(game->makeMove(1, 0, 'O'));
    EXPECT_FALSE(game->checkWin('O'));
    
    EXPECT_TRUE(game->makeMove(0, 1, 'X'));
    EXPECT_FALSE(game->checkWin('X'));
    
    EXPECT_TRUE(game->makeMove(1, 1, 'O'));
    EXPECT_FALSE(game->checkWin('O'));
    
    EXPECT_TRUE(game->makeMove(0, 2, 'X'));
    EXPECT_TRUE(game->checkWin('X'));  // X wins with top row
}

TEST_F(GameTest, DrawGameScenario) {
    // Set up a draw scenario
    game->makeMove(0, 0, 'X');
    game->makeMove(0, 1, 'O');
    game->makeMove(0, 2, 'X');
    game->makeMove(1, 0, 'O');
    game->makeMove(1, 1, 'X');
    game->makeMove(1, 2, 'O');
    game->makeMove(2, 0, 'O');
    game->makeMove(2, 1, 'X');
    game->makeMove(2, 2, 'O');
    
    EXPECT_FALSE(game->checkWin('X'));
    EXPECT_FALSE(game->checkWin('O'));
    EXPECT_TRUE(game->isBoardFull());
}

// Main function to run tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}