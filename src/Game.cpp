#include "Game.h"
#include <algorithm>

Game::Game(Database *db) : db(db), vsAI(false) {
    reset();
}

void Game::startGame(bool vsAI) {
    this->vsAI = vsAI;
    reset();
}

bool Game::makeMove(int row, int col, char player) {
    if (row < 0 || row >= 3 || col < 0 || col >= 3 || board[row][col] != ' ')
        return false;
    board[row][col] = player;
    return true;
}

void Game::aiMove(char aiSymbol) {
    char playerSymbol = (aiSymbol == 'X') ? 'O' : 'X';
    int bestScore = -1000;
    int bestRow = -1, bestCol = -1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == ' ') {
                board[i][j] = aiSymbol;
                int score = minimax(board, 0, false, -1000, 1000, aiSymbol, playerSymbol);
                board[i][j] = ' ';
                if (score > bestScore) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }
    if (bestRow != -1 && bestCol != -1)
        board[bestRow][bestCol] = aiSymbol;
}

int Game::minimax(char board[3][3], int depth, bool isMax, int alpha, int beta, char aiSymbol, char playerSymbol) {
    if (checkWin(aiSymbol)) return 10 - depth;
    if (checkWin(playerSymbol)) return depth - 10;
    if (isBoardFull()) return 0;

    if (isMax) {
        int best = -1000;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') {
                    board[i][j] = aiSymbol;
                    best = std::max(best, minimax(board, depth + 1, false, alpha, beta, aiSymbol, playerSymbol));
                    board[i][j] = ' ';
                    alpha = std::max(alpha, best);
                    if (beta <= alpha) break;
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') {
                    board[i][j] = playerSymbol;
                    best = std::min(best, minimax(board, depth + 1, true, alpha, beta, aiSymbol, playerSymbol));
                    board[i][j] = ' ';
                    beta = std::min(beta, best);
                    if (beta <= alpha) break;
                }
            }
        }
        return best;
    }
}

bool Game::checkWin(char player) {
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
    }
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;
    return false;
}

bool Game::isBoardFull() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == ' ')
                return false;
    return true;
}

void Game::reset() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board[i][j] = ' ';
}

void Game::getBoard(char board[3][3]) const {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board[i][j] = this->board[i][j];
}
