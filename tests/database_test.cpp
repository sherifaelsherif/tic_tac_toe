#include <gtest/gtest.h>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include "Database.h"

class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Remove any existing test database
        if (QFile::exists("test_tictactoe.db")) {
            QFile::remove("test_tictactoe.db");
        }
        
        // Create a separate database connection for testing
        if (QSqlDatabase::contains("qt_sql_default_connection")) {
            QSqlDatabase::removeDatabase("qt_sql_default_connection");
        }
        
        db = new Database();
    }

    void TearDown() override {
        delete db;
        
        // Clean up test database
        if (QFile::exists("test_tictactoe.db")) {
            QFile::remove("test_tictactoe.db");
        }
        if (QFile::exists("tictactoe.db")) {
            QFile::remove("tictactoe.db");
        }
    }

    Database* db;
};

// Test user registration functionality
TEST_F(DatabaseTest, RegisterUser_ValidCredentials_ReturnsTrue) {
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
}

TEST_F(DatabaseTest, RegisterUser_DuplicateUsername_ReturnsFalse) {
    // Register first user
    EXPECT_TRUE(db->registerUser("testuser", "password1"));
    
    // Try to register same username again
    EXPECT_FALSE(db->registerUser("testuser", "password2"));
}

TEST_F(DatabaseTest, RegisterUser_EmptyUsername_ReturnsFalse) {
    EXPECT_FALSE(db->registerUser("", "password"));
}

// Test user authentication functionality
TEST_F(DatabaseTest, Authenticate_ValidCredentials_ReturnsUserId) {
    // Register a user first
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    
    // Authenticate with correct credentials
    int userId = db->authenticate("testuser", "testpassword");
    EXPECT_GT(userId, 0);
}

TEST_F(DatabaseTest, Authenticate_InvalidPassword_ReturnsMinusOne) {
    // Register a user first
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    
    // Try to authenticate with wrong password
    int userId = db->authenticate("testuser", "wrongpassword");
    EXPECT_EQ(userId, -1);
}

TEST_F(DatabaseTest, Authenticate_NonexistentUser_ReturnsMinusOne) {
    int userId = db->authenticate("nonexistent", "password");
    EXPECT_EQ(userId, -1);
}

TEST_F(DatabaseTest, Authenticate_EmptyCredentials_ReturnsMinusOne) {
    int userId = db->authenticate("", "");
    EXPECT_EQ(userId, -1);
}

// Test game saving functionality
TEST_F(DatabaseTest, SaveGame_ValidGame_ReturnsTrue) {
    // Register and authenticate user first
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    int userId = db->authenticate("testuser", "testpassword");
    EXPECT_GT(userId, 0);
    
    // Create a test board
    char board[3][3] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    
    EXPECT_TRUE(db->saveGame(userId, board, "Win"));
}

TEST_F(DatabaseTest, SaveGame_InvalidUserId_ReturnsFalse) {
    char board[3][3] = {
        {'X', 'O', ' '},
        {' ', 'X', ' '},
        {' ', ' ', ' '}
    };
    
    // Try to save game with invalid user ID
    EXPECT_FALSE(db->saveGame(-1, board, "Win"));
}

TEST_F(DatabaseTest, SaveGame_MultipleGames_AllReturnTrue) {
    // Register and authenticate user
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    int userId = db->authenticate("testuser", "testpassword");
    EXPECT_GT(userId, 0);
    
    // Save multiple games
    char board1[3][3] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    
    char board2[3][3] = {
        {'O', 'X', 'O'},
        {'X', 'O', 'X'},
        {'O', 'X', 'O'}
    };
    
    EXPECT_TRUE(db->saveGame(userId, board1, "Win"));
    EXPECT_TRUE(db->saveGame(userId, board2, "Loss"));
}

// Test game history retrieval
TEST_F(DatabaseTest, GetGameHistory_NoGames_ReturnsNoGamesMessage) {
    // Register and authenticate user
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    int userId = db->authenticate("testuser", "testpassword");
    EXPECT_GT(userId, 0);
    
    QString history = db->getGameHistory(userId);
    EXPECT_EQ(history, "No games played.");
}

TEST_F(DatabaseTest, GetGameHistory_WithGames_ReturnsFormattedHistory) {
    // Register and authenticate user
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    int userId = db->authenticate("testuser", "testpassword");
    EXPECT_GT(userId, 0);
    
    // Save a game
    char board[3][3] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    
    EXPECT_TRUE(db->saveGame(userId, board, "Win"));
    
    QString history = db->getGameHistory(userId);
    EXPECT_FALSE(history.isEmpty());
    EXPECT_TRUE(history.contains("XOX"));
    EXPECT_TRUE(history.contains("Win"));
    EXPECT_TRUE(history.contains("Game at"));
}

TEST_F(DatabaseTest, GetGameHistory_InvalidUserId_ReturnsNoGamesMessage) {
    QString history = db->getGameHistory(-1);
    EXPECT_EQ(history, "No games played.");
}

TEST_F(DatabaseTest, GetGameHistory_MultipleGames_ReturnsAllGames) {
    // Register and authenticate user
    EXPECT_TRUE(db->registerUser("testuser", "testpassword"));
    int userId = db->authenticate("testuser", "testpassword");
    EXPECT_GT(userId, 0);
    
    // Save multiple games
    char board1[3][3] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    
    char board2[3][3] = {
        {'O', 'X', 'O'},
        {'X', 'O', 'X'},
        {'O', 'X', 'O'}
    };
    
    EXPECT_TRUE(db->saveGame(userId, board1, "Win"));
    EXPECT_TRUE(db->saveGame(userId, board2, "Loss"));
    
    QString history = db->getGameHistory(userId);
    
    // Check that both games are in history
    EXPECT_TRUE(history.contains("Win"));
    EXPECT_TRUE(history.contains("Loss"));
    
    // Count number of game entries (should be 2)
    int gameCount = history.count("Game at");
    EXPECT_EQ(gameCount, 2);
}

// Test user isolation (one user's data doesn't interfere with another's)
TEST_F(DatabaseTest, UserIsolation_TwoUsers_IndependentData) {
    // Register two users
    EXPECT_TRUE(db->registerUser("user1", "pass1"));
    EXPECT_TRUE(db->registerUser("user2", "pass2"));
    
    int userId1 = db->authenticate("user1", "pass1");
    int userId2 = db->authenticate("user2", "pass2");
    
    EXPECT_GT(userId1, 0);
    EXPECT_GT(userId2, 0);
    EXPECT_NE(userId1, userId2);
    
    // Save games for user1
    char board1[3][3] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    EXPECT_TRUE(db->saveGame(userId1, board1, "Win"));
    
    // Check that user1 has games but user2 doesn't
    QString history1 = db->getGameHistory(userId1);
    QString history2 = db->getGameHistory(userId2);
    
    EXPECT_TRUE(history1.contains("Win"));
    EXPECT_EQ(history2, "No games played.");
}

// Integration test: Full workflow
TEST_F(DatabaseTest, FullWorkflow_RegisterAuthenticateSaveRetrieve_Success) {
    // Register user
    EXPECT_TRUE(db->registerUser("player1", "mypassword"));
    
    // Authenticate
    int userId = db->authenticate("player1", "mypassword");
    EXPECT_GT(userId, 0);
    
    // Save multiple games with different results
    char winBoard[3][3] = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    
    char lossBoard[3][3] = {
        {'O', 'X', 'O'},
        {'X', 'O', 'X'},
        {'O', 'X', 'O'}
    };
    
    char drawBoard[3][3] = {
        {'X', 'O', 'X'},
        {'X', 'O', 'O'},
        {'O', 'X', 'X'}
    };
    
    EXPECT_TRUE(db->saveGame(userId, winBoard, "Win"));
    EXPECT_TRUE(db->saveGame(userId, lossBoard, "Loss"));
    EXPECT_TRUE(db->saveGame(userId, drawBoard, "Draw"));
    
    // Retrieve and verify history
    QString history = db->getGameHistory(userId);
    EXPECT_TRUE(history.contains("Win"));
    EXPECT_TRUE(history.contains("Loss"));
    EXPECT_TRUE(history.contains("Draw"));
    
    int gameCount = history.count("Game at");
    EXPECT_EQ(gameCount, 3);
}

// Main function to run tests
int main(int argc, char **argv) {
    // Initialize Qt Application (required for Qt SQL operations)
    QApplication app(argc, argv);
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}