#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "Game.h"
#include "Database.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
// Add this parameter to your existing constructor
explicit MainWindow(int userId, Database *db, bool testMode = false, QWidget *parent = nullptr);

private slots:
    void handleCellClick(int row, int col);
    void startGameVsAI();
    void startGameVsPlayer();
    void restartGame();
    void showHistory();
    void logout();
private:
    void setupTestDefaults(); // NEW METHOD
    // ... all your existing methods stay the same ...

    // Add this new member variable
    bool m_testMode; // NEW MEMBER
    // ... all your existing members stay the same ...
    void setupUI();
    void updateBoard();
    void updatePlayerIndicator();
    void showModeSelectionDialog();
    void showSymbolSelectionDialog();
    QString formatBoard(const QString &board);
    QPushButton *cells[3][3];
    int currentUserId;
    char currentPlayer;
    char playerSymbol;
    Database *db;
    Game *game;
    QLabel *playerIndicator;
    QLabel *modeIndicator;
    bool gameStarted;
};

#endif

