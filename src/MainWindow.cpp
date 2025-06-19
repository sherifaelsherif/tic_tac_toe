#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSizePolicy>
#include <QApplication>
#include "AuthWindow.h"

MainWindow::MainWindow(int userId, Database *db, bool testMode, QWidget *parent)
    : QMainWindow(parent), currentUserId(userId), currentPlayer('X'), playerSymbol('X'), db(db), gameStarted(false), m_testMode(testMode) {
    game = new Game(db);
    setupUI();
    
    if (m_testMode) {
        setupTestDefaults();
    } else {
        showModeSelectionDialog();
    }
    
    setStyleSheet(
        "QMainWindow {"
        " background: qradialgradient(cx:0.5, cy:0.5, radius:1, fx:0.5, fy:0.5, stop:0 #0a0a1a, stop:1 #1c1c3a);"
        " background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAABYSURBVChTY5i5f/8/Axjo6RlaWVkZGBgYGIiJiUlqampqa2vrt7e3t3d0dHT09PT09vb29vYODg72tra2Dg4ODg5OTk5OTi4uLi4uLi4uLi4uLi4uLi4uLi4uLi4uLhYAXtY5pAAAAABJRU5ErkJggg==');"
        " color: #ffffff;"
        "}"
        "QPushButton {"
        " background-color: rgba(30, 30, 60, 0.8);"
        " color: #e0e0ff;"
        " border: 2px solid #bb00ff;"
        " border-radius: 8px;"
        " padding: 8px;"
        " font-size: 16px;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        " text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(60, 60, 90, 0.8);"
        " border: 2px solid #ff00ff;"
        " text-shadow: 0 0 10px #ff00ff;"
        "}"
        "QLabel {"
        " color: #e0e0ff;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        "}"
        "QDialog {"
        " background-color: rgba(20, 20, 40, 0.9);"
        " border: 2px solid #bb00ff;"
        "}"
        "QListWidget {"
        " background-color: rgba(20, 20, 40, 0.9);"
        " color: #e0e0ff;"
        " border: 2px solid #bb00ff;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        " font-size: 14px;"
        "}"
        "QListWidget::item {"
        " padding: 10px;"
        " border-bottom: 1px solid #bb00ff;"
        "}"
        "QListWidget::item:hover {"
        " background-color: rgba(40, 40, 60, 0.9);"
        "}"
    );
}

void MainWindow::setupTestDefaults() {
    // Set up the game in a ready-to-test state without showing dialogs
    game->startGame(true); // Default to VS AI
    playerSymbol = 'X';
    currentPlayer = 'X';
    gameStarted = true;
    modeIndicator->setText("MODE: VS AI");
    updatePlayerIndicator();
    
    // Enable all board cells for testing
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cells[i][j]->setEnabled(true);
        }
    }
    updateBoard();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Title
    QLabel *titleLabel = new QLabel("TIC TAC TOE", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #00eaff;"
        "text-shadow: 0 0 10px #00eaff, 0 0 20px #00eaff;"
    );
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    // Game mode and player indicators
    modeIndicator = new QLabel("SELECT A GAME MODE", this);
    modeIndicator->setObjectName("modeIndicator");
    modeIndicator->setStyleSheet(
        "font-size: 16px;"
        "color: #bb00ff;"
        "text-shadow: 0 0 8px #bb00ff;"
    );
    mainLayout->addWidget(modeIndicator, 0, Qt::AlignCenter);

    playerIndicator = new QLabel("PLAYER X'S TURN", this);
    playerIndicator->setObjectName("playerIndicator");
    playerIndicator->setStyleSheet(
        "font-size: 14px;"
        "color: #00eaff;"
        "text-shadow: 0 0 8px #00eaff;"
    );
    mainLayout->addWidget(playerIndicator, 0, Qt::AlignCenter);

    // Game mode buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *vsAIButton = new QPushButton("VS AI", this);
    vsAIButton->setObjectName("vsAIButton");
    QPushButton *vsPlayerButton = new QPushButton("VS PLAYER", this);
    vsPlayerButton->setObjectName("vsPlayerButton");
    QPushButton *restartButton = new QPushButton("RESTART", this);
    restartButton->setObjectName("restartButton");
    QPushButton *historyButton = new QPushButton("HISTORY", this);
    historyButton->setObjectName("historyButton");
    QPushButton *logoutButton = new QPushButton("LOGOUT", this);
    logoutButton->setObjectName("logoutButton");

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

    // History button visible only for registered users
    historyButton->setVisible(currentUserId != -1);

    // Game board
    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cells[i][j] = new QPushButton("", this);
            cells[i][j]->setObjectName(QString("cell_%1_%2").arg(i).arg(j));
            cells[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cells[i][j]->setEnabled(false); // Disable until game starts
            cells[i][j]->setText("");       // Ensure empty
            QString borderStyle;
            if (i == 0 && j == 0) borderStyle = "border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 0 && j == 1) borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 0 && j == 2) borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            else if (i == 1 && j == 0) borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 1 && j == 1) borderStyle = "border: 1px solid #bb00ff;";
            else if (i == 1 && j == 2) borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            else if (i == 2 && j == 0) borderStyle = "border-top: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 2 && j == 1) borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 2 && j == 2) borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            cells[i][j]->setStyleSheet(
                QString(
                    "QPushButton {"
                    " background-color: rgba(20, 20, 40, 0.7);"
                    " %1"
                    " border-radius: 0px;"
                    " font-size: 40px;"
                    " font-family: 'Orbitron', 'Arial', sans-serif;"
                    " color: transparent;"
                    " margin: 0px;"
                    " padding: 0px;"
                    " min-width: 100px;"
                    " min-height: 100px;"
                    "}"
                    "QPushButton:pressed {"
                    " background-color: rgba(40, 40, 60, 0.7);"
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
        if (currentUserId != -1) {
            db->saveGame(currentUserId, board, QString(currentPlayer));
        }
        game->reset();
        updateBoard();
        currentPlayer = playerSymbol;
        updatePlayerIndicator();
    } else if (game->isBoardFull()) {
        char board[3][3];
        game->getBoard(board);
        QMessageBox::information(this, "RESULT", "IT'S A TIE!");
        if (currentUserId != -1) {
            db->saveGame(currentUserId, board, "Tie");
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
                if (currentUserId != -1) {
                    db->saveGame(currentUserId, board, QString(aiSymbol));
                }
                game->reset();
                updateBoard();
                currentPlayer = playerSymbol;
                updatePlayerIndicator();
            } else if (game->isBoardFull()) {
                char board[3][3];
                game->getBoard(board);
                QMessageBox::information(this, "RESULT", "IT'S A TIE!");
                if (currentUserId != -1) {
                    db->saveGame(currentUserId, board, "Tie");
                }
                game->reset();
                updateBoard();
                currentPlayer = playerSymbol;
                updatePlayerIndicator();
            } else {
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
            cells[i][j]->setText(text == " " ? "" : text);
            QString borderStyle;
            if (i == 0 && j == 0) borderStyle = "border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 0 && j == 1) borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 0 && j == 2) borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            else if (i == 1 && j == 0) borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 1 && j == 1) borderStyle = "border: 1px solid #bb00ff;";
            else if (i == 1 && j == 2) borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            else if (i == 2 && j == 0) borderStyle = "border-top: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 2 && j == 1) borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
            else if (i == 2 && j == 2) borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
            if (text == "X") {
                cells[i][j]->setStyleSheet(
                    QString(
                        "QPushButton {"
                        " background-color: rgba(20, 20, 40, 0.7);"
                        " %1"
                        " border-radius: 0px;"
                        " font-size: 40px;"
                        " font-family: 'Orbitron', 'Arial', sans-serif;"
                        " color: #00eaff;"
                        " text-shadow: 0 0 10px #00eaff, 0 0 20px #00eaff;"
                        " margin: 0px;"
                        " padding: 0px;"
                        " min-width: 100px;"
                        " min-height: 100px;"
                        "}"
                    ).arg(borderStyle)
                );
            } else if (text == "O") {
                cells[i][j]->setStyleSheet(
                    QString(
                        "QPushButton {"
                        " background-color: rgba(20, 20, 40, 0.7);"
                        " %1"
                        " border-radius: 0px;"
                        " font-size: 40px;"
                        " font-family: 'Orbitron', 'Arial', sans-serif;"
                        " color: #ff00cc;"
                        " text-shadow: 0 0 10px #ff00cc, 0 0 20px #ff00cc;"
                        " margin: 0px;"
                        " padding: 0px;"
                        " min-width: 100px;"
                        " min-height: 100px;"
                        "}"
                    ).arg(borderStyle)
                );
            } else {
                cells[i][j]->setStyleSheet(
                    QString(
                        "QPushButton {"
                        " background-color: rgba(20, 20, 40, 0.7);"
                        " %1"
                        " border-radius: 0px;"
                        " font-size: 40px;"
                        " font-family: 'Orbitron', 'Arial', sans-serif;"
                        " color: transparent;"
                        " margin: 0px;"
                        " padding: 0px;"
                        " min-width: 100px;"
                        " min-height: 100px;"
                        "}"
                        "QPushButton:pressed {"
                        " background-color: rgba(40, 40, 60, 0.7);"
                        "}"
                    ).arg(borderStyle)
                );
            }
        }
    }
}

void MainWindow::updatePlayerIndicator() {
    if (!gameStarted) {
        playerIndicator->setText("SELECT A GAME MODE");
        playerIndicator->setStyleSheet(
            "font-size: 14px; color: #bb00ff; text-shadow: 0 0 8px #bb00ff;"
        );
        return;
    }
    if (game->isVsAI()) {
        playerIndicator->setText(currentPlayer == playerSymbol ? QString("YOUR TURN (%1)").arg(currentPlayer) : QString("AI'S TURN (%1)").arg(playerSymbol == 'X' ? 'O' : 'X'));
    } else {
        playerIndicator->setText(QString("PLAYER %1'S TURN").arg(currentPlayer));
    }
    playerIndicator->setStyleSheet(
        currentPlayer == 'X' ?
        "font-size: 14px; color: #00eaff; text-shadow: 0 0 8px #00eaff;" :
        "font-size: 14px; color: #ff00cc; text-shadow: 0 0 8px #ff00cc;"
    );
}

void MainWindow::startGameVsAI() {
    game->startGame(true);
    if (!m_testMode) {
        showSymbolSelectionDialog();
    }
}

void MainWindow::startGameVsPlayer() {
    game->startGame(false);
    if (!m_testMode) {
        showSymbolSelectionDialog();
    }
}

void MainWindow::restartGame() {
    if (!gameStarted) {
        QMessageBox::warning(this, "GAME NOT STARTED", "PLEASE SELECT A GAME MODE!");
        return;
    }
    game->reset();
    currentPlayer = playerSymbol;
    updateBoard();
    updatePlayerIndicator();
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
        " background-color: rgba(30, 30, 60, 0.8);"
        " color: #e0e0ff;"
        " border: 2px solid #bb00ff;"
        " border-radius: 8px;"
        " padding: 8px;"
        " font-size: 16px;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        " text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(60, 60, 90, 0.8);"
        " border: 2px solid #ff00ff;"
        " text-shadow: 0 0 10px #ff00ff;"
        "}"
    );
    vsPlayerButton->setStyleSheet(
        "QPushButton {"
        " background-color: rgba(30, 30, 60, 0.8);"
        " color: #e0e0ff;"
        " border: 2px solid #bb00ff;"
        " border-radius: 8px;"
        " padding: 8px;"
        " font-size: 16px;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        " text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(60, 60, 90, 0.8);"
        " border: 2px solid #ff00ff;"
        " text-shadow: 0 0 10px #ff00ff;"
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
        " background-color: rgba(30, 30, 60, 0.8);"
        " color: #e0e0ff;"
        " border: 2px solid #bb00ff;"
        " border-radius: 8px;"
        " padding: 8px;"
        " font-size: 16px;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        " text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(60, 60, 90, 0.8);"
        " border: 2px solid #ff00ff;"
        " text-shadow: 0 0 10px #ff00ff;"
        "}"
    );
    oButton->setStyleSheet(
        "QPushButton {"
        " background-color: rgba(30, 30, 60, 0.8);"
        " color: #e0e0ff;"
        " border: 2px solid #bb00ff;"
        " border-radius: 8px;"
        " padding: 8px;"
        " font-size: 16px;"
        " font-family: 'Orbitron', 'Arial', sans-serif;"
        " text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(60, 60, 90, 0.8);"
        " border: 2px solid #ff00ff;"
        " text-shadow: 0 0 10px #ff00ff;"
        "}"
    );
    layout->addWidget(xButton);
    layout->addWidget(oButton);

    connect(xButton, &QPushButton::clicked, [=]() {
        playerSymbol = 'X';
        currentPlayer = 'X';
        modeIndicator->setText(game->isVsAI() ? "MODE: VS AI" : "MODE: VS PLAYER");
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
        modeIndicator->setText(game->isVsAI() ? "MODE: VS AI" : "MODE: VS PLAYER");
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
    if (currentUserId == -1) {
        QMessageBox::information(this, "INFO", "GAME HISTORY IS NOT AVAILABLE IN GUEST MODE.");
        return;
    }
    QDialog *historyDialog = new QDialog(this);
    historyDialog->setWindowTitle("GAME HISTORY");
    QVBoxLayout *layout = new QVBoxLayout(historyDialog);
    QListWidget *historyList = new QListWidget(historyDialog);

    QString history = db->getGameHistory(currentUserId);
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

            QWidget *entryWidget = new QWidget();
            QVBoxLayout *entryLayout = new QVBoxLayout(entryWidget);

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
                    if (i == 0 && j == 0) borderStyle = "border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    else if (i == 0 && j == 1) borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    else if (i == 0 && j == 2) borderStyle = "border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
                    else if (i == 1 && j == 0) borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    else if (i == 1 && j == 1) borderStyle = "border: 1px solid #bb00ff;";
                    else if (i == 1 && j == 2) borderStyle = "border-top: 1px solid #bb00ff; border-bottom: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
                    else if (i == 2 && j == 0) borderStyle = "border-top: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    else if (i == 2 && j == 1) borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff; border-right: 1px solid #bb00ff;";
                    else if (i == 2 && j == 2) borderStyle = "border-top: 1px solid #bb00ff; border-left: 1px solid #bb00ff;";
                    if (cellText == "X") {
                        cellLabel->setStyleSheet(
                            QString(
                                "QLabel {"
                                " background-color: rgba(20, 20, 40, 0.7);"
                                " %1"
                                " font-size: 20px;"
                                " font-family: 'Orbitron', 'Arial', sans-serif;"
                                " color: #00eaff;"
                                " text-shadow: 0 0 5px #00eaff;"
                                " text-align: center;"
                                " min-width: 30px;"
                                " min-height: 30px;"
                                "}"
                            ).arg(borderStyle)
                        );
                    } else if (cellText == "O") {
                        cellLabel->setStyleSheet(
                            QString(
                                "QLabel {"
                                " background-color: rgba(20, 20, 40, 0.7);"
                                " %1"
                                " font-size: 20px;"
                                " font-family: 'Orbitron', 'Arial', sans-serif;"
                                " color: #ff00cc;"
                                " text-shadow: 0 0 5px #ff00cc;"
                                " text-align: center;"
                                " min-width: 30px;"
                                " min-height: 30px;"
                                "}"
                            ).arg(borderStyle)
                        );
                    } else {
                        cellLabel->setStyleSheet(
                            QString(
                                "QLabel {"
                                " background-color: rgba(20, 20, 40, 0.7);"
                                " %1"
                                " font-size: 20px;"
                                " font-family: 'Orbitron', 'Arial', sans-serif;"
                                " color: transparent;"
                                " text-align: center;"
                                " min-width: 30px;"
                                " min-height: 30px;"
                                "}"
                            ).arg(borderStyle)
                        );
                    }
                    boardLayout->addWidget(cellLabel, i, j);
                }
            }
            entryLayout->addLayout(boardLayout);

            QLabel *resultLabel = new QLabel(QString("RESULT: %1").arg(result));
            resultLabel->setStyleSheet(
                "font-size: 12px;"
                "color: #bb00ff;"
                "text-shadow: 0 0 5px #bb00ff;"
            );
            entryLayout->addWidget(resultLabel);

            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(entryWidget->sizeHint());
            historyList->addItem(item);
            historyList->setItemWidget(item, entryWidget);
        }
    }
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
