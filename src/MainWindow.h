#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "Game.h"
#include "Database.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(int userId, Database *db, QWidget *parent = nullptr);
    ~MainWindow() override;
private slots:
    void handleCellClick(int row, int col);
    void startGameVsAI();
    void startGameVsPlayer();
    void restartGame();
    void showHistory();
    void logout();
    void startReplay(int gameId);
    void playNextReplayMove();
    void exitReplayMode();
private:
    void setupUI();
    void updateBoard();
    void updatePlayerIndicator();
    void showModeSelectionDialog();
    void showSymbolSelectionDialog();
    QString formatBoard(const QString &board);
    QPushButton *cells[3][3];
    int userId;
    char currentPlayer;
    char playerSymbol;
    Database *db;
    Game *game;
    QLabel *playerIndicatorLabel;
    QLabel *modeIndicatorLabel;
    bool gameStarted;
    bool isReplayMode;
    int replayGameId;
    QStringList replayMoves;
    int replayMoveIndex;
    QTimer *replayTimer;
};

#endif
