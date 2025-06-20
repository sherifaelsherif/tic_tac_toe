#include "Database.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Database::Database() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("tictactoe.db");
    if (!db.open()) {
        qDebug() << "Error: Could not open database:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password TEXT);");
    query.exec("CREATE TABLE IF NOT EXISTS games (id INTEGER PRIMARY KEY AUTOINCREMENT, user_id INTEGER, moves TEXT, result TEXT, timestamp TEXT);");
}

Database::~Database() {
    db.close();
}

QString Database::hashPassword(const QString &password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

int Database::authenticate(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = :username AND password = :password;");
    query.bindValue(":username", username);
    query.bindValue(":password", hashPassword(password));
    if (!query.exec()) {
        qDebug() << "Authenticate error:" << query.lastError().text();
        return -1;
    }
    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

bool Database::registerUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password);");
    query.bindValue(":username", username);
    query.bindValue(":password", hashPassword(password));
    if (!query.exec()) {
        qDebug() << "Register error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::saveGame(int userId, const QString &moves, const QString &result) {
    QSqlQuery query;
    query.prepare("INSERT INTO games (user_id, moves, result, timestamp) VALUES (:user_id, :moves, :result, :timestamp);");
    query.bindValue(":user_id", userId);
    query.bindValue(":moves", moves);
    query.bindValue(":result", result);
    query.bindValue(":timestamp", QDateTime::currentDateTime().toString());
    if (!query.exec()) {
        qDebug() << "Save game error:" << query.lastError().text();
        return false;
    }
    return true;
}

QString Database::getGameHistory(int userId) {
    QString history;
    QSqlQuery query;
    query.prepare("SELECT moves, result, timestamp FROM games WHERE user_id = :user_id;");
    query.bindValue(":user_id", userId);
    if (!query.exec()) {
        qDebug() << "Get history error:" << query.lastError().text();
        return "Error retrieving history.";
    }
    while (query.next()) {
        QString moves = query.value(0).toString();
        QString result = query.value(1).toString();
        QString timestamp = query.value(2).toString();
        history += QString("Game at %1: Moves: %2, Result: %3\n").arg(timestamp, moves, result);
    }
    return history.isEmpty() ? "No games played." : history;
}
