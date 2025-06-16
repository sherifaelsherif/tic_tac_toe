#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "Database.h"
#include "MainWindow.h"
#include "RegisterWindow.h"

class AuthWindow : public QMainWindow {
    Q_OBJECT
public:
    AuthWindow(QWidget *parent = nullptr);
private slots:
    void attemptLogin();
    void openRegisterWindow();
    void playAsGuest();
    void clearForm();
private:
    void setupUI();
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    Database *db;
    RegisterWindow *registerWindow;
};

#endif
