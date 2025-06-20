#ifndef GAME_H
#define GAME_H

#include "Database.h"
#include <random>
#include <vector>

class Game {
public:
    enum class Difficulty { Easy, Medium, Hard };
    Game(Database *db);
    void startGame(bool vsAI, Difficulty difficulty = Difficulty::Medium);
    bool makeMove(int row, int col, char player);
    void aiMove(char aiSymbol);
    bool checkWin(char player);
    bool isBoardFull();
    void reset();
    bool isVsAI() const { return vsAI; }
    void getBoard(char board[3][3]) const;
    QString getMoveHistory() const;
private:
    int minimax(char board[3][3], int depth, bool isMax, int alpha, int beta, char aiSymbol, char playerSymbol, int maxDepth);
    char board[3][3];
    Database *db;
    bool vsAI;
    Difficulty difficulty;
    std::random_device rd;
    std::mt19937 rng;
    std::vector<std::pair<int, int>> moveHistory; // Store move sequence
};

#endif
