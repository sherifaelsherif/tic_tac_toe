#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

class Database {
public:
    Database();
    ~Database();
    int authenticate(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password);
    bool saveGame(int userId, char board[3][3], const QString &result);
    QString getGameHistory(int userId);
    QString getGameBoard(int gameId); // New method to get specific game board
    int getGameId(int userId, const QString &timestamp, const QString &board, const QString &result); // New method to get game ID
private:
    QSqlDatabase db;
    QString hashPassword(const QString &password);
};

#endif
