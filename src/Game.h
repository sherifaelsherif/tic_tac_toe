#ifndef GAME_H
#define GAME_H

#include "Database.h"

class Game {
public:
    Game(Database *db);
    void startGame(bool vsAI);
    bool makeMove(int row, int col, char player);
    void aiMove(char aiSymbol);
    bool checkWin(char player);
    bool isBoardFull();
    void reset();
    bool isVsAI() const { return vsAI; }
    void getBoard(char board[3][3]) const;
private:
    int minimax(char board[3][3], int depth, bool isMax, int alpha, int beta, char aiSymbol, char playerSymbol);
    char board[3][3];
    Database *db;
    bool vsAI;
};

#endif
