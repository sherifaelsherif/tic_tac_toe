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
    bool saveGame(int userId, const QString &moves, const QString &result);
    QString getGameHistory(int userId);
private:
    QSqlDatabase db;
    QString hashPassword(const QString &password);
};

#endif
