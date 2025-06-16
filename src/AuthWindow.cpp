#include "AuthWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

AuthWindow::AuthWindow(QWidget *parent) : QMainWindow(parent) {
    db = new Database();
    registerWindow = nullptr;
    setMinimumSize(350, 400);
    setupUI();

    setStyleSheet(
        "QMainWindow {"
        "    background: qradialgradient(cx:0.5, cy:0.5, radius:1, fx:0.5, fy:0.5, stop:0 #0a0a1a, stop:1 #1c1c3a);"
        "    background-image: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAABYSURBVChTY5i5f/8/Axjo6RlaWVkZGBgYGIiJiUlqampqa2vrt7e3t3d0dHT09PT09vb29vYODg72tra2Dg4ODg5OTk5OTi4uLi4uLi4uLi4uLi4uLi4uLi4uLi4uLhYAXtY5pAAAAABJRU5ErkJggg==');"
        "    color: #ffffff;"
        "}"
        "QLineEdit {"
        "    background-color: rgba(20, 20, 40, 0.9);"
        "    color: #e0e0ff;"
        "    border: 2px solid #bb00ff;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "    font-size: 14px;"
        "    font-family: 'Orbitron', 'Arial', sans-serif;"
        "    text-shadow: 0 0 5px #bb00ff;"
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #ff00ff;"
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
        );
}

void AuthWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QLabel *titleLabel = new QLabel("TIC TAC TOE", this);
    titleLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #00eaff;"
        "text-shadow: 0 0 10px #00eaff, 0 0 20px #00eaff;"
        );
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QLabel *welcomeLabel = new QLabel("WELCOME TO THE GAME", this);
    welcomeLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #bb00ff;"
        "text-shadow: 0 0 8px #bb00ff;"
        );
    mainLayout->addWidget(welcomeLabel, 0, Qt::AlignCenter);

    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->addWidget(new QLabel("USERNAME:"));
    usernameEdit = new QLineEdit(this);
    usernameEdit->setFixedWidth(200);
    formLayout->addWidget(usernameEdit);

    formLayout->addWidget(new QLabel("PASSWORD:"));
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);
    formLayout->addWidget(passwordEdit);

    QPushButton *loginButton = new QPushButton("LOGIN", this);
    QPushButton *registerButton = new QPushButton("REGISTER", this);
    QPushButton *guestButton = new QPushButton("PLAY AS GUEST", this);
    QPushButton *clearButton = new QPushButton("CLEAR", this);
    formLayout->addWidget(loginButton);
    formLayout->addWidget(registerButton);
    formLayout->addWidget(guestButton);
    formLayout->addWidget(clearButton);

    mainLayout->addLayout(formLayout);
    mainLayout->setAlignment(formLayout, Qt::AlignCenter);

    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    connect(loginButton, &QPushButton::clicked, this, &AuthWindow::attemptLogin);
    connect(registerButton, &QPushButton::clicked, this, &AuthWindow::openRegisterWindow);
    connect(guestButton, &QPushButton::clicked, this, &AuthWindow::playAsGuest);
    connect(clearButton, &QPushButton::clicked, this, &AuthWindow::clearForm);
}

void AuthWindow::attemptLogin() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "ERROR", "USERNAME AND PASSWORD CANNOT BE EMPTY!");
        return;
    }

    if (username.length() < 3 || password.length() < 6) {
        QMessageBox::warning(this, "ERROR", "USERNAME MUST BE AT LEAST 3 CHARACTERS AND PASSWORD AT LEAST 6 CHARACTERS!");
        return;
    }

    int userId = db->authenticate(username, password);
    if (userId >= 0) {
        QMessageBox::information(this, "SUCCESS", "LOGGED IN SUCCESSFULLY!");
        MainWindow *gameWindow = new MainWindow(userId, db);
        gameWindow->show();
        this->close();
    } else {
        QMessageBox::warning(this, "ERROR", "INVALID USERNAME OR PASSWORD!");
    }
}

void AuthWindow::openRegisterWindow() {
    if (!registerWindow) {
        registerWindow = new RegisterWindow(db, this);
        connect(registerWindow, &RegisterWindow::registrationSuccessful, this, &AuthWindow::clearForm);
    }
    registerWindow->show();
}

void AuthWindow::playAsGuest() {
    MainWindow *gameWindow = new MainWindow(-1, db);
    gameWindow->show();
    this->close();
}

void AuthWindow::clearForm() {
    usernameEdit->clear();
    passwordEdit->clear();
}
