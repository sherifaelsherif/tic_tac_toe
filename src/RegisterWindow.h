#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel> // Ensure QLabel is included here
#include "Database.h"

class RegisterWindow : public QMainWindow {
    Q_OBJECT
public:
    RegisterWindow(Database *db, QWidget *parent = nullptr);
signals:
    void registrationSuccessful();
private slots:
    void attemptRegister();
    void cancelRegistration();
    void updatePasswordStrength();
private:
    void setupUI();
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *passwordStrengthLabel; // Add this member variable
    Database *db;
};

#endif
