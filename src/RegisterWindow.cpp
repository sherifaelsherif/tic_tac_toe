#include "RegisterWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>

RegisterWindow::RegisterWindow(Database *db, QWidget *parent) : QMainWindow(parent), db(db) {
    setMinimumSize(350, 450);
    setupUI();

    setStyleSheet(QStringLiteral(
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
        ));
}

void RegisterWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QLabel *titleLabel = new QLabel("REGISTER", this);
    titleLabel->setStyleSheet(QStringLiteral(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #00eaff;"
        "text-shadow: 0 0 10px #00eaff, 0 0 20px #00eaff;"
        ));
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    QLabel *welcomeLabel = new QLabel("CREATE A NEW ACCOUNT", this);
    welcomeLabel->setStyleSheet(QStringLiteral(
        "font-size: 14px;"
        "color: #bb00ff;"
        "text-shadow: 0 0 8px #bb00ff;"
        ));
    mainLayout->addWidget(welcomeLabel, 0, Qt::AlignCenter);

    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->addWidget(new QLabel("USERNAME:"));
    usernameEdit = new QLineEdit(this);
    usernameEdit->setFixedWidth(200);
    usernameEdit->setPlaceholderText("Letters and numbers only");
    usernameEdit->setToolTip("Username can only contain letters (a-z, A-Z) and numbers (0-9)");
    formLayout->addWidget(usernameEdit);

    formLayout->addWidget(new QLabel("PASSWORD:"));
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedWidth(200);
    formLayout->addWidget(passwordEdit);

    passwordStrengthLabel = new QLabel("PASSWORD STRENGTH: WEAK", this);
    passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #ff5555;");
    formLayout->addWidget(passwordStrengthLabel);

    formLayout->addWidget(new QLabel("CONFIRM PASSWORD:"));
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setFixedWidth(200);
    formLayout->addWidget(confirmPasswordEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *registerButton = new QPushButton("REGISTER", this);
    QPushButton *cancelButton = new QPushButton("CANCEL", this);
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(cancelButton);
    formLayout->addLayout(buttonLayout);

    mainLayout->addLayout(formLayout);
    mainLayout->setAlignment(formLayout, Qt::AlignCenter);

    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    connect(registerButton, &QPushButton::clicked, this, &RegisterWindow::attemptRegister);
    connect(cancelButton, &QPushButton::clicked, this, &RegisterWindow::cancelRegistration);
    connect(passwordEdit, &QLineEdit::textChanged, this, &RegisterWindow::updatePasswordStrength);
}

void RegisterWindow::attemptRegister() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();

    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "ERROR", "ALL FIELDS MUST BE FILLED!");
        return;
    }

    if (username.length() < 3 || password.length() < 6) {
        QMessageBox::warning(this, "ERROR", "USERNAME MUST BE AT LEAST 3 CHARACTERS AND PASSWORD AT LEAST 6 CHARACTERS!");
        return;
    }

    if (username.length() > 50) {
        QMessageBox::warning(this, "ERROR", "USERNAME CANNOT EXCEED 50 CHARACTERS!");
        return;
    }

    if (password.length() > 100) {
        QMessageBox::warning(this, "ERROR", "PASSWORD CANNOT EXCEED 100 CHARACTERS!");
        return;
    }

    QRegularExpression specialChars("[^a-zA-Z0-9]");
    if (specialChars.match(username).hasMatch()) {
        QMessageBox::warning(this, "ERROR", "USERNAME CAN ONLY CONTAIN LETTERS AND NUMBERS!");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "ERROR", "PASSWORDS DO NOT MATCH!");
        return;
    }

    if (db->registerUser(username, password)) {
        QMessageBox::information(this, "SUCCESS", "REGISTERED SUCCESSFULLY! PLEASE LOGIN.");
        emit registrationSuccessful();
        this->close();
    } else {
        QMessageBox::warning(this, "ERROR", "USERNAME ALREADY EXISTS!");
    }
}

void RegisterWindow::cancelRegistration() {
    this->close();
}

void RegisterWindow::updatePasswordStrength() {
    QString password = passwordEdit->text();
    int length = password.length();
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;

    for (const QChar &c : password) {
        if (c.isUpper()) hasUpper = true;
        if (c.isLower()) hasLower = true;
        if (c.isDigit()) hasDigit = true;
        if (!c.isLetterOrNumber()) hasSpecial = true;
    }

    int strength = 0;
    if (length >= 6) strength++;
    if (length >= 8) strength++;
    if (hasUpper) strength++;
    if (hasLower) strength++;
    if (hasDigit) strength++;
    if (hasSpecial) strength++;

    if (strength <= 2) {
        passwordStrengthLabel->setText("PASSWORD STRENGTH: WEAK");
        passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #ff5555;");
    } else if (strength <= 4) {
        passwordStrengthLabel->setText("PASSWORD STRENGTH: MEDIUM");
        passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #ffaa00;");
    } else {
        passwordStrengthLabel->setText("PASSWORD STRENGTH: STRONG");
        passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #55ff55;");
    }
}
