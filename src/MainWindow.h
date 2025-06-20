#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "Game.h"
#include "Database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(int userId, Database *db, bool testMode = false, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleCellClick(int row, int col);
    void startGameVsAI();
    void startGameVsPlayer();
    void restartGame();
    void showHistory();
    void logout();

private:
    void setupTestDefaults();
    void setupUI();
    void updateBoard();
    void updatePlayerIndicator();
    void showModeSelectionDialog();
    void showSymbolSelectionDialog();
    QString formatBoard(const QString &board);

    // UI file
    Ui::MainWindow *ui;

    // Custom UI members
    QPushButton *cells[3][3];
    QLabel *playerIndicator;
    QLabel *modeIndicator;

    // State
    int currentUserId;
    char currentPlayer;
    char playerSymbol;
    Database *db;
    Game *game;
    bool gameStarted;
    bool m_testMode;
};

#endif // MAINWINDOW_H
