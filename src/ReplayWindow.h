#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "Game.h"

class ReplayWindow : public QMainWindow {
    Q_OBJECT
public:
    ReplayWindow(const QString &moves, const QString &result, char playerSymbol, QWidget *parent = nullptr);
private slots:
    void processReplayMove();
private:
    void setupUI();
    QPushButton *cells[3][3];
    char replayBoard[3][3];
    QStringList replayMoves;
    int replayIndex;
    QLabel *resultLabel;
    QTimer *replayTimer;
    char playerSymbol;
};

#endif