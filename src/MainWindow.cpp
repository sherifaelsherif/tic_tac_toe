#include "MainWindow.h"
#include <QApplication>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QListWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include "AuthWindow.h"

MainWindow::MainWindow(int userId, Database *db, QWidget *parent)
    : QMainWindow(parent),
    userId(userId),
    currentPlayer('X'),
    playerSymbol('X'),
    db(db),
    gameStarted(false),
    isReplayMode(false),
    replayGameId(-1),
    replayMoveIndex(0) {
    game = new Game(db);
    replayTimer = new QTimer(this);
    connect(replayTimer, &QTimer::timeout, this, &MainWindow::playNextReplayMove);
    setupUI();
    showModeSelectionDialog();

    setStyleSheet(
        "QMainWindow {"
        "    background: qradialgradient(cx:0.5, cy:0.5, radius:1, fx:0.5, fy:0.5, stop:0 #0a0a1a, stop:1 #1c1c3a);"
        "    background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAABYSURBVChTY5i5f/8/Axjo6RlaWVkZGBgYGIiJiUlqampqa2vrt7e3t3d0dHT09PT09vb29vYODg72tra2Dg4ODg5OTk5OTi4uLi4uLi4uLi4uLi4uLi4uLi4uLi4uLhYAXtY5pAAAAABJRU5ErkJggg==');"
        "    color: #ffffff;"
        "}"
        "QPushButton {"
        "    background-color: rgba(30, 30, 60, 0.8);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 16px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(60, 60, 90, 0.8);"
        "    border: 2px solid #ff00ff;"
        "    text-shadow: 0 0 10px #ff00ff;"
        "}"
        "QLabel {"
        "    color: #e0e0ff;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "}"
        "QDialog {"
        "    background-color: rgba(20, 20, 40, 0.9);"
        "    border: 2px solid #bb00ff;"
        "}"
        "QListWidget {"
        "    background-color: rgba(20, 20, 40, 0.9);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    font-size: 14px;"
        "}"
        "QListWidget::item {"
        "    padding: 10px;"
        "    border-bottom: 1px solid #bb00ff;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: rgba(40, 40, 60, 0.9);"
        "}"
        );
}

MainWindow::~MainWindow() {
    delete replayTimer;
    delete game;
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Title
    QLabel *titleLabel = new QLabel("TIC TAC TOE", this);
    titleLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #00eaff;"
        "text-shadow: 0 0 10px #00eaff, 0 0 20px #00eaff;"
        );
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    // Game mode and player indicators
    modeIndicatorLabel = new QLabel("SELECT A GAME MODE", this);
    modeIndicatorLabel->setStyleSheet(
        "font-size: 16px;"
        "color: #bb00ff;"
        "text-shadow: 0 0 8px #bb00ff;"
        );
    mainLayout->addWidget(modeIndicatorLabel, 0, Qt::AlignCenter);

    playerIndicatorLabel = new QLabel("PLAYER X'S TURN", this);
    playerIndicatorLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #00eaff;"
        "text-shadow: 0 0 8px #00eaff;"
        );
    mainLayout->addWidget(playerIndicatorLabel, 0, Qt::AlignCenter);

    // Game mode buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *vsAIButton = new QPushButton("VS AI", this);
    QPushButton *vsPlayerButton = new QPushButton("VS PLAYER", this);
    QPushButton *restartButton = new QPushButton("RESTART", this);
    QPushButton *historyButton = new QPushButton("HISTORY", this);
    QPushButton *logoutButton = new QPushButton("LOGOUT", this);
    vsAIButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    vsPlayerButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    restartButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    historyButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    logoutButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonLayout->addWidget(vsAIButton);
    buttonLayout->addWidget(vsPlayerButton);
    buttonLayout->addWidget(restartButton);
    buttonLayout->addWidget(historyButton);
    buttonLayout->addWidget(logoutButton);
    mainLayout->addLayout(buttonLayout);

    // Game board
    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cells[i][j] = new QPushButton("", this);
            cells[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cells[i][j]->setEnabled(false); // Disable until game starts
            QString borderStyle;
            if (i == 0 && j == 0) {
                borderStyle = "border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 0 && j == 1) {
                borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 0 && j == 2) {
                borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            } else if (i == 1 && j == 0) {
                borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 1 && j == 1) {
                borderStyle = "border: 1px solid #bb00ff;";
            } else if (i == 1 && j == 2) {
                borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            } else if (i == 2 && j == 0) {
                borderStyle = "border-top: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 2 && j == 1) {
                borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 2 && j == 2) {
                borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            }
            cells[i][j]->setStyleSheet(
                QString(
                    "QPushButton {"
                    "    background-color: rgba(20, 20, 40, 0.7);"
                    "    %1"
                    "    border-radius: 0px;"
                    "    font-size: 40px;"
                    "    font-family: 'Orbitron', 'Arial', sans-serif;"
                    "    color: transparent;"
                    "    margin: 0px;"
                    "    padding: 0px;"
                    "    min-width: 100px;"
                    "    min-height: 100px;"
                    "}"
                    "QPushButton:pressed {"
                    "    background-color: rgba(40, 40, 60, 0.7);"
                    "}"
                    ).arg(borderStyle)
                );
            connect(cells[i][j], &QPushButton::clicked, this, [=]() { handleCellClick(i, j); });
            boardLayout->addWidget(cells[i][j], i, j);
        }
    }
    mainLayout->addLayout(boardLayout, 1);

    // Connect signals
    connect(vsAIButton, &QPushButton::clicked, this, &MainWindow::startGameVsAI);
    connect(vsPlayerButton, &QPushButton::clicked, this, &MainWindow::startGameVsPlayer);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::restartGame);
    connect(historyButton, &QPushButton::clicked, this, &MainWindow::showHistory);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
}

void MainWindow::handleCellClick(int row, int col) {
    if (isReplayMode) {
        QMessageBox::warning(this, "REPLAY MODE", "CANNOT MAKE MOVES IN REPLAY MODE!");
        return;
    }
    if (!gameStarted) {
        QMessageBox::warning(this, "GAME NOT STARTED", "PLEASE SELECT A GAME MODE!");
        return;
    }

    if (!game->makeMove(row, col, currentPlayer)) {
        QMessageBox::warning(this, "INVALID MOVE", "THIS CELL IS ALREADY TAKEN OR INVALID!");
        return;
    }

    updateBoard();
    if (game->checkWin(currentPlayer)) {
        char board[3][3];
        game->getBoard(board);
        QMessageBox::information(this, "RESULT", QString("PLAYER %1 WINS!").arg(currentPlayer));
        if (userId != -1) {
            db->saveGame(userId, board, QString(currentPlayer));
        }
        game->reset();
        updateBoard();
        currentPlayer = playerSymbol;
        updatePlayerIndicator();
    } else if (game->isBoardFull()) {
        char board[3][3];
        game->getBoard(board);
        QMessageBox::information(this, "RESULT", "IT'S A TIE!");
        if (userId != -1) {
            db->saveGame(userId, board, "Tie");
        }
        game->reset();
        updateBoard();
        currentPlayer = playerSymbol;
        updatePlayerIndicator();
    } else {
        if (game->isVsAI()) {
            char aiSymbol = (playerSymbol == 'X') ? 'O' : 'X';
            game->aiMove(aiSymbol);
            updateBoard();
            if (game->checkWin(aiSymbol)) {
                char board[3][3];
                game->getBoard(board);
                QMessageBox::information(this, "RESULT", "AI WINS!");
                if (userId != -1) {
                    db->saveGame(userId, board, QString(aiSymbol));
                }
                game->reset();
                updateBoard();
                currentPlayer = playerSymbol;
                updatePlayerIndicator();
            } else if (game->isBoardFull()) {
                char board[3][3];
                game->getBoard(board);
                QMessageBox::information(this, "RESULT", "IT'S A TIE!");
                if (userId != -1) {
                    db->saveGame(userId, board, "Tie");
                }
                game->reset();
                updateBoard();
                currentPlayer = playerSymbol;
                updatePlayerIndicator();
            }
        } else {
            currentPlayer = (currentPlayer == playerSymbol) ? (playerSymbol == 'X' ? 'O' : 'X') : playerSymbol;
            updatePlayerIndicator();
        }
    }
}

void MainWindow::updateBoard() {
    char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    game->getBoard(board);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString text = QString(board[i][j]);
            cells[i][j]->setText(text);
            QString borderStyle;
            if (i == 0 && j == 0) {
                borderStyle = "border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 0 && j == 1) {
                borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 0 && j == 2) {
                borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            } else if (i == 1 && j == 0) {
                borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 1 && j == 1) {
                borderStyle = "border: 1px solid #bb00ff;";
            } else if (i == 1 && j == 2) {
                borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            } else if (i == 2 && j == 0) {
                borderStyle = "border-top: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 2 && j == 1) {
                borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            } else if (i == 2 && j == 2) {
                borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            }
            if (text == "X") {
                cells[i][j]->setStyleSheet(
                    QString(
                        "QPushButton {"
                        "    background-color: rgba(20, 20, 40, 0.7);"
                        "    %1"
                        "    border-radius: 0px;"
                        "    font-size: 40px;"
                        "    font-family: 'Orbitron', 'Arial', sans-serif;"
                        "    color: #00eaff;"
                        "    text-shadow: 0 0 10px #00eaff, 0 0 20px #00eaff;"
                        "    margin: 0px;"
                        "    padding: 0px;"
                        "    min-width: 100px;"
                        "    min-height: 100px;"
                        "}"
                        ).arg(borderStyle)
                    );
            } else if (text == "O") {
                cells[i][j]->setStyleSheet(
                    QString(
                        "QPushButton {"
                        "    background-color: rgba(20, 20, 40, 0.7);"
                        "    %1"
                        "    border-radius: 0px;"
                        "    font-size: 40px;"
                        "    font-family: 'Orbitron', 'Arial', sans-serif;"
                        "    color: #ff00cc;"
                        "    text-shadow: 0 0 10px #ff00cc, 0 0 20px #ff00cc;"
                        "    margin: 0px;"
                        "    padding: 0px;"
                        "    min-width: 100px;"
                        "    min-height: 100px;"
                        "}"
                        ).arg(borderStyle)
                    );
            } else {
                cells[i][j]->setStyleSheet(
                    QString(
                        "QPushButton {"
                        "    background-color: rgba(20, 20, 40, 0.7);"
                        "    %1"
                        "    border-radius: 0px;"
                        "    font-size: 40px;"
                        "    font-family: 'Orbitron', 'Arial', sans-serif;"
                        "    color: transparent;"
                        "    margin: 0px;"
                        "    padding: 0px;"
                        "    min-width: 100px;"
                        "    min-height: 100px;"
                        "}"
                        "QPushButton:pressed {"
                        "    background-color: rgba(40, 40, 60, 0.7);"
                        "}"
                        ).arg(borderStyle)
                    );
            }
        }
    }
}

void MainWindow::updatePlayerIndicator() {
    if (isReplayMode) {
        playerIndicatorLabel->setText(QString("REPLAY MODE: MOVE %1/%2").arg(replayMoveIndex).arg(replayMoves.size() - 1));
        playerIndicatorLabel->setStyleSheet(
            "font-size: 14px; color: #bb00ff; text-shadow: 0 0 8px #bb00ff;"
            );
        return;
    }
    if (!gameStarted) {
        playerIndicatorLabel->setText("SELECT A GAME MODE");
        playerIndicatorLabel->setStyleSheet(
            "font-size: 14px; color: #bb00ff; text-shadow: 0 0 8px #bb00ff;"
            );
        return;
    }
    if (game->isVsAI()) {
        playerIndicatorLabel->setText(currentPlayer == playerSymbol ? QString("YOUR TURN (%1)").arg(currentPlayer) : QString("AI'S TURN (%1)").arg(playerSymbol == 'X' ? 'O' : 'X'));
    } else {
        playerIndicatorLabel->setText(QString("PLAYER %1'S TURN").arg(currentPlayer));
    }
    playerIndicatorLabel->setStyleSheet(
        currentPlayer == 'X' ?
            "font-size: 14px; color: #00eaff; text-shadow: 0 0 8px #00eaff;" :
            "font-size: 14px; color: #ff00cc; text-shadow: 0 0 8px #ff00cc;"
        );
}

void MainWindow::startGameVsAI() {
    if (isReplayMode) {
        exitReplayMode();
    }
    game->startGame(true);
    showSymbolSelectionDialog();
}

void MainWindow::startGameVsPlayer() {
    if (isReplayMode) {
        exitReplayMode();
    }
    game->startGame(false);
    showSymbolSelectionDialog();
}

void MainWindow::restartGame() {
    if (isReplayMode) {
        exitReplayMode();
    }
    if (!gameStarted) {
        QMessageBox::warning(this, "GAME NOT STARTED", "PLEASE SELECT A GAME MODE!");
        return;
    }
    game->reset();
    currentPlayer = playerSymbol;
    updateBoard();
    updatePlayerIndicator();
}

void MainWindow::startReplay(int gameId) {
    QString board = db->getGameBoard(gameId);
    if (board.isEmpty()) {
        QMessageBox::warning(this, "ERROR", "COULD NOT LOAD GAME FOR REPLAY!");
        return;
    }

    isReplayMode = true;
    replayGameId = gameId;
    replayMoves.clear();
    replayMoveIndex = 0;

    // Initialize with empty board
    QString emptyBoard = "         "; // 9 spaces
    replayMoves.append(emptyBoard);

    // Simulate moves by adding one symbol at a time
    for (int i = 0; i < board.length(); ++i) {
        if (board[i] == 'X' || board[i] == 'O') {
            QString newBoard = replayMoves.last();
            newBoard[i] = board[i];
            replayMoves.append(newBoard);
        }
    }

    // Disable board interaction
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cells[i][j]->setEnabled(false);
        }
    }

    // Start the replay with reduced delay
    replayTimer->start(500); // Reduced to 0.5 seconds
    playNextReplayMove();
}

void MainWindow::playNextReplayMove() {
    if (!isReplayMode) {
        replayTimer->stop();
        return;
    }

    // Update the board for the current move
    QString boardStr = replayMoves[replayMoveIndex];
    char board[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = boardStr[i * 3 + j].toLatin1();
        }
    }
    game->setBoard(board);
    updateBoard();
    updatePlayerIndicator();

    // Increment for the next move
    replayMoveIndex++;
    if (replayMoveIndex >= replayMoves.size()) {
        replayTimer->stop();
        QMessageBox::information(this, "REPLAY FINISHED", "GAME REPLAY COMPLETED!");
        exitReplayMode();
        return;
    }
}

void MainWindow::exitReplayMode() {
    replayTimer->stop();
    isReplayMode = false;
    replayGameId = -1;
    replayMoves.clear();
    replayMoveIndex = 0;
    game->reset();
    updateBoard();
    updatePlayerIndicator();
    if (gameStarted) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cells[i][j]->setEnabled(true);
            }
        }
    }
}

void MainWindow::showModeSelectionDialog() {
    QDialog *modeDialog = new QDialog(this);
    modeDialog->setWindowTitle("SELECT GAME MODE");
    QVBoxLayout *layout = new QVBoxLayout(modeDialog);

    QLabel *label = new QLabel("CHOOSE YOUR GAME MODE:", modeDialog);
    label->setStyleSheet(
        "font-size: 16px;"
        "color: #e0e0ff;"
        "font-family: 'Orbitron', 'Arial', sans-serif;"
        );
    layout->addWidget(label, 0, Qt::AlignCenter);

    QPushButton *vsAIButton = new QPushButton("VS AI", modeDialog);
    QPushButton *vsPlayerButton = new QPushButton("VS PLAYER", modeDialog);
    vsAIButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(30, 30, 60, 0.8);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 16px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(60, 60, 90, 0.8);"
        "    border: 2px solid #ff00ff;"
        "    text-shadow: 0 0 10px #ff00ff;"
        "}"
        );
    vsPlayerButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(30, 30, 60, 0.8);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 16px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(60, 60, 90, 0.8);"
        "    border: 2px solid #ff00ff;"
        "    text-shadow: 0 0 10px #ff00ff;"
        "}"
        );
    layout->addWidget(vsAIButton);
    layout->addWidget(vsPlayerButton);

    connect(vsAIButton, &QPushButton::clicked, this, &MainWindow::startGameVsAI);
    connect(vsAIButton, &QPushButton::clicked, modeDialog, &QDialog::accept);
    connect(vsPlayerButton, &QPushButton::clicked, this, &MainWindow::startGameVsPlayer);
    connect(vsPlayerButton, &QPushButton::clicked, modeDialog, &QDialog::accept);

    modeDialog->exec();
}

void MainWindow::showSymbolSelectionDialog() {
    QDialog *symbolDialog = new QDialog(this);
    symbolDialog->setWindowTitle("SELECT YOUR SYMBOL");
    QVBoxLayout *layout = new QVBoxLayout(symbolDialog);

    QLabel *label = new QLabel("CHOOSE YOUR SYMBOL:", symbolDialog);
    label->setStyleSheet(
        "font-size: 16px;"
        "color: #e0e0ff;"
        "font-family: 'Orbitron', 'Arial', sans-serif;"
        );
    layout->addWidget(label, 0, Qt::AlignCenter);

    QPushButton *xButton = new QPushButton("X", symbolDialog);
    QPushButton *oButton = new QPushButton("O", symbolDialog);
    xButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(30, 30, 60, 0.8);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 16px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(60, 60, 90, 0.8);"
        "    border: 2px solid #ff00ff;"
        "    text-shadow: 0 0 10px #ff00ff;"
        "}"
        );
    oButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(30, 30, 60, 0.8);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 16px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(60, 60, 90, 0.8);"
        "    border: 2px solid #ff00ff;"
        "    text-shadow: 0 0 10px #ff00ff;"
        "}"
        );
    layout->addWidget(xButton);
    layout->addWidget(oButton);

    connect(xButton, &QPushButton::clicked, [=]() {
        playerSymbol = 'X';
        currentPlayer = 'X';
        modeIndicatorLabel->setText(game->isVsAI() ? "MODE: VS AI" : "MODE: VS PLAYER");
        updateBoard();
        updatePlayerIndicator();
        gameStarted = true;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                cells[i][j]->setEnabled(true);
        symbolDialog->accept();
    });
    connect(oButton, &QPushButton::clicked, [=]() {
        playerSymbol = 'O';
        currentPlayer = 'O';
        modeIndicatorLabel->setText(game->isVsAI() ? "MODE: VS AI" : "MODE: VS PLAYER");
        updateBoard();
        updatePlayerIndicator();
        gameStarted = true;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                cells[i][j]->setEnabled(true);
        symbolDialog->accept();
    });

    symbolDialog->exec();
}

void MainWindow::showHistory() {
    if (userId == -1) {
        QMessageBox::information(this, "INFO", "GAME HISTORY IS NOT AVAILABLE IN GUEST MODE.");
        return;
    }

    QDialog *historyDialog = new QDialog(this);
    historyDialog->setWindowTitle("GAME HISTORY");
    QVBoxLayout *layout = new QVBoxLayout(historyDialog);

    QListWidget *historyList = new QListWidget(historyDialog);

    QString history = db->getGameHistory(userId);
    if (history.isEmpty() || history == "No games played.\n") {
        QListWidgetItem *item = new QListWidgetItem("NO GAMES PLAYED YET.");
        historyList->addItem(item);
    } else {
        QStringList games = history.split("\n", Qt::SkipEmptyParts);
        int gameId = 1;
        for (const QString &game : games) {
            if (game == "No games played.") continue;

            int separatorIndex = game.indexOf(": ");
            if (separatorIndex == -1) continue;

            QString timestamp = game.left(separatorIndex).replace("Game at ", "");
            QString details = game.mid(separatorIndex + 2);

            int boardLabelIndex = details.indexOf("Board: ");
            int resultLabelIndex = details.indexOf(", Result: ");
            if (boardLabelIndex == -1 || resultLabelIndex == -1) continue;

            QString board = details.mid(boardLabelIndex + 7, resultLabelIndex - (boardLabelIndex + 7));
            QString result = details.mid(resultLabelIndex + 10);

            // Get the actual database game ID
            int dbGameId = db->getGameId(userId, timestamp, board, result);
            if (dbGameId == -1) continue;

            // Create a custom widget for the game entry
            QWidget *entryWidget = new QWidget();
            QVBoxLayout *entryLayout = new QVBoxLayout(entryWidget);

            // Game ID and Timestamp
            QLabel *titleLabel = new QLabel(QString("GAME %1").arg(gameId++));
            titleLabel->setStyleSheet(
                "font-size: 16px;"
                "color: #00eaff;"
                "text-shadow: 0 0 8px #00eaff;"
                );
            entryLayout->addWidget(titleLabel);

            QLabel *timestampLabel = new QLabel(QString("TIMESTAMP: %1").arg(timestamp));
            timestampLabel->setStyleSheet(
                "font-size: 12px;"
                "color: #bb00ff;"
                "text-shadow: 0 0 5px #bb00ff;"
                );
            entryLayout->addWidget(timestampLabel);

            // Board as a 3x3 grid
            QGridLayout *boardLayout = new QGridLayout();
            boardLayout->setSpacing(0);
            boardLayout->setContentsMargins(0, 0, 0, 0);
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    int index = i * 3 + j;
                    if (index >= board.length()) continue;
                    QString cellText = board[index];
                    QLabel *cellLabel = new QLabel(cellText);
                    QString borderStyle;
                    if (i == 0 && j == 0) {
                        borderStyle = "border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    } else if (i == 0 && j == 1) {
                        borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    } else if (i == 0 && j == 2) {
                        borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
                    } else if (i == 1 && j == 0) {
                        borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    } else if (i == 1 && j == 1) {
                        borderStyle = "border: 1px solid #bb00ff;";
                    } else if (i == 1 && j == 2) {
                        borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
                    } else if (i == 2 && j == 0) {
                        borderStyle = "border-top: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    } else if (i == 2 && j == 1) {
                        borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    } else if (i == 2 && j == 2) {
                        borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
                    }
                    if (cellText == "X") {
                        cellLabel->setStyleSheet(
                            QString(
                                "QLabel {"
                                "    background-color: rgba(20, 20, 40, 0.7);"
                                "    %1"
                                "    font-size: 20px;"
                                "    font-family: 'Orbitron', 'Arial', sans-serif;"
                                "    color: #00eaff;"
                                "    text-shadow: 0 0 5px #00eaff;"
                                "    text-align: center;"
                                "    min-width: 30px;"
                                "    min-height: 30px;"
                                "}"
                                ).arg(borderStyle)
                            );
                    } else if (cellText == "O") {
                        cellLabel->setStyleSheet(
                            QString(
                                "QLabel {"
                                "    background-color: rgba(20, 20, 40, 0.7);"
                                "    %1"
                                "    font-size: 20px;"
                                "    font-family: 'Orbitron', 'Arial', sans-serif;"
                                "    color: #ff00cc;"
                                "    text-shadow: 0 0 5px #ff00cc;"
                                "    text-align: center;"
                                "    min-width: 30px;"
                                "    min-height: 30px;"
                                "}"
                                ).arg(borderStyle)
                            );
                    } else {
                        cellLabel->setStyleSheet(
                            QString(
                                "QLabel {"
                                "    background-color: rgba(20, 20, 40, 0.7);"
                                "    %1"
                                "    font-size: 20px;"
                                "    font-family: 'Orbitron', 'Arial', sans-serif;"
                                "    color: transparent;"
                                "    text-align: center;"
                                "    min-width: 30px;"
                                "    min-height: 30px;"
                                "}"
                                ).arg(borderStyle)
                            );
                    }
                    boardLayout->addWidget(cellLabel, i, j);
                }
            }
            entryLayout->addLayout(boardLayout);

            // Result
            QLabel *resultLabel = new QLabel(QString("RESULT: %1").arg(result));
            resultLabel->setStyleSheet(
                "font-size: 12px;"
                "color: #bb00ff;"
                "text-shadow: 0 0 5px #bb00ff;"
                );
            entryLayout->addWidget(resultLabel);

            // Replay Button
            QPushButton *replayButton = new QPushButton("REPLAY GAME", entryWidget);
            replayButton->setStyleSheet(
                "QPushButton {"
                "    background-color: rgba(30, 30, 60, 0.8);"
                "    color: #e0e0ff;"
                "    border: 2px solid #bb00ff;"
                "    border-radius: 8px;"
                "    padding: 8px;"
                "    font-size: 14px;"
                "    font-family: 'Orbitron', 'Arial', sans-serif;"
                "    text-shadow: 0 0 5px #bb00ff;"
                "}"
                "QPushButton:hover {"
                "    background-color: rgba(60, 60, 90, 0.8);"
                "    border: 2px solid #ff00ff;"
                "    text-shadow: 0 0 10px #ff00ff;"
                "}"
                );
            entryLayout->addWidget(replayButton);
            connect(replayButton, &QPushButton::clicked, [=]() {
                historyDialog->accept();
                startReplay(dbGameId);
            });

            // Add the custom widget to the list
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(entryWidget->sizeHint());
            historyList->addItem(item);
            historyList->setItemWidget(item, entryWidget);
        }
    }

    // Navigation buttons for replay (only exit button)
    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *exitButton = new QPushButton("EXIT REPLAY", historyDialog);
    exitButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(30, 30, 60, 0.8);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 14px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(60, 60, 90, 0.8);"
        "    border: 2px solid #ff00ff;"
        "    text-shadow: 0 0 10px #ff00ff;"
        "}"
        );
    navLayout->addWidget(exitButton);
    layout->addLayout(navLayout);

    connect(exitButton, &QPushButton::clicked, this, &MainWindow::exitReplayMode);

    layout->addWidget(historyList);
    historyDialog->resize(400, 500);
    historyDialog->exec();
}

QString MainWindow::formatBoard(const QString &board) {
    if (board.length() != 9) return board;

    QString formatted;
    for (int i = 0; i < 9; i += 3) {
        formatted += board.mid(i, 3) + "\n";
    }
    return formatted;
}

void MainWindow::logout() {
    int choice = QMessageBox::question(this, "CONFIRM LOGOUT", "ARE YOU SURE YOU WANT TO LOGOUT?", QMessageBox::Yes | QMessageBox::No);
    if (choice == QMessageBox::Yes) {
        AuthWindow *authWindow = new AuthWindow();
        authWindow->show();
        this->close();
    }
}
