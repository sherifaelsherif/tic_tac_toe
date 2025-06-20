#include "ReplayWindow.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

ReplayWindow::ReplayWindow(const QString &moves, const QString &result, char playerSymbol, QWidget *parent)
    : QMainWindow(parent), replayIndex(0), playerSymbol(playerSymbol) {
    // Initialize replay board
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            replayBoard[i][j] = ' ';

    replayMoves = moves.split(";", Qt::SkipEmptyParts);
    replayTimer = new QTimer(this);
    connect(replayTimer, &QTimer::timeout, this, &ReplayWindow::processReplayMove);
    setupUI();
    resultLabel->setText(QString("RESULT: %1").arg(result));
    replayTimer->start(1000); // Show each move every 1 second
}

void ReplayWindow::setupUI() {
    setWindowTitle("GAME REPLAY");
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    resultLabel = new QLabel("", this);
    resultLabel->setStyleSheet(
        "font-size: 16px;"
        "color: #bb00ff;"
        "text-shadow: 0 0 8px #bb00ff;"
        "font-family: 'Orbitron', 'Arial', sans-serif;"
        );
    mainLayout->addWidget(resultLabel, 0, Qt::AlignCenter);

    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cells[i][j] = new QPushButton("", this);
            cells[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cells[i][j]->setEnabled(false); // Disable interaction
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
                    "    text-align: center;"
                    "    line-height: 100px;" // Ensure vertical centering
                    "    margin: 0px;"
                    "    padding: 0px;"
                    "    min-width: 100px;"
                    "    min-height: 100px;"
                    "}"
                    ).arg(borderStyle)
                );
            boardLayout->addWidget(cells[i][j], i, j);
        }
    }
    mainLayout->addLayout(boardLayout);

    setStyleSheet(
        "QMainWindow {"
        "    background: qradialgradient(cx:0.5, cy:0.5, radius:1, fx:0.5, fy:0.5, stop:0 #0a0a1a, stop:1 #1c1c3a);"
        "    background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAABYSURBVChTY5i5f/8/Axjo6RlaWVkZGBgYGIiJiUlqampqa2vrt7e3t3d0dHT09PT09vb29vYODg72tra2Dg4ODg5OTk5OTi4uLi4uLi4uLi4uLi4uLi4uLi4uLi4uLhYAXtY5pAAAAABJRU5ErkJggg==');"
        "    color: #ffffff;"
        "}"
        "QLabel {"
        "    color: #e0e0ff;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "}"
        );
}

void ReplayWindow::processReplayMove() {
    if (replayIndex >= replayMoves.size()) {
        replayTimer->stop();
        return;
    }

    QStringList move = replayMoves[replayIndex].split(",");
    if (move.size() != 2) {
        replayTimer->stop();
        return;
    }

    int row = move[0].toInt();
    int col = move[1].toInt();
    char symbol = (replayIndex % 2 == 0) ? playerSymbol : (playerSymbol == 'X' ? 'O' : 'X');
    replayBoard[row][col] = symbol;

    // Update UI
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString text = QString(replayBoard[i][j]);
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
                        "    text-align: center;"
                        "    line-height: 100px;" // Ensure vertical centering
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
                        "    text-align: center;"
                        "    line-height: 100px;" // Ensure vertical centering
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
                        "    text-align: center;"
                        "    line-height: 100px;" // Ensure vertical centering
                        "    margin: 0px;"
                        "    padding: 0px;"
                        "    min-width: 100px;"
                        "    min-height: 100px;"
                        "}"
                        ).arg(borderStyle)
                    );
            }
        }
    }

    replayIndex++;
}
