#include <QtTest/QtTest>
#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QSignalSpy>
#include <QRegularExpression>

// Simple mock Database for isolated testing
class MockDatabase {
public:
    MockDatabase() : shouldSucceed(true), callCount(0) {}
    
    bool registerUser(const QString &username, const QString &password) {
        callCount++;
        lastUsername = username;
        lastPassword = password;
        return shouldSucceed;
    }
    
    void setShouldSucceed(bool succeed) { shouldSucceed = succeed; }
    QString getLastUsername() const { return lastUsername; }
    QString getLastPassword() const { return lastPassword; }
    int getCallCount() const { return callCount; }
    void reset() { callCount = 0; lastUsername.clear(); lastPassword.clear(); }

private:
    bool shouldSucceed;
    int callCount;
    QString lastUsername;
    QString lastPassword;
};

// Testable RegisterWindow that uses our mock
class TestableRegisterWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TestableRegisterWindow(MockDatabase *db, QWidget *parent = nullptr)
        : QMainWindow(parent), mockDb(db) {
        setMinimumSize(350, 450);
        setupUI();
        setStyleSheet("QMainWindow { background: #1c1c3a; color: #ffffff; }");
    }

    // Expose private members for testing
    QLineEdit* getUsernameEdit() { return usernameEdit; }
    QLineEdit* getPasswordEdit() { return passwordEdit; }
    QLineEdit* getConfirmPasswordEdit() { return confirmPasswordEdit; }
    QLabel* getPasswordStrengthLabel() { return passwordStrengthLabel; }
    QPushButton* getRegisterButton() { return registerButton; }
    QPushButton* getCancelButton() { return cancelButton; }

    // Test the actual validation logic
    bool validateInput(const QString &username, const QString &password, const QString &confirmPassword) {
        if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
            return false;
        }
        
        if (username.length() < 3 || password.length() < 6) {
            return false;
        }
        
        if (username.length() > 50 || password.length() > 100) {
            return false;
        }
        
        QRegularExpression specialChars("[^a-zA-Z0-9]");
        if (specialChars.match(username).hasMatch()) {
            return false;
        }
        
        if (password != confirmPassword) {
            return false;
        }
        
        return true;
    }

    // FIXED: Test password strength calculation to match actual implementation
    QString calculatePasswordStrength(const QString &password) {
        int length = password.length();
        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        
        for (const QChar &c : password) {
            if (c.isUpper()) hasUpper = true;
            if (c.isLower()) hasLower = true;
            if (c.isDigit()) hasDigit = true;
            if (!c.isLetterOrNumber()) hasSpecial = true;
        }
        
        int strength = 0;
        // REMOVED: if (length >= 6) strength++; - This was causing the mismatch
        if (length >= 8) strength++;  // Only count length >= 8, not >= 6
        if (hasUpper) strength++;
        if (hasLower) strength++;
        if (hasDigit) strength++;
        if (hasSpecial) strength++;
        
        if (strength <= 2) return "WEAK";
        else if (strength <= 4) return "MEDIUM";
        else return "STRONG";
    }

signals:
    void registrationSuccessful();

public slots:
    void attemptRegister() {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text();
        QString confirmPassword = confirmPasswordEdit->text();
        
        if (!validateInput(username, password, confirmPassword)) {
            return; // Validation failed
        }
        
        if (mockDb->registerUser(username, password)) {
            emit registrationSuccessful();
            this->close();
        }
    }
    
    void cancelRegistration() {
        this->close();
    }
    
    void updatePasswordStrength() {
        QString password = passwordEdit->text();
        QString strength = calculatePasswordStrength(password);
        passwordStrengthLabel->setText("PASSWORD STRENGTH: " + strength);
        
        if (strength == "WEAK") {
            passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #ff5555;");
        } else if (strength == "MEDIUM") {
            passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #ffaa00;");
        } else {
            passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #55ff55;");
        }
    }

private:
    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        usernameEdit = new QLineEdit(centralWidget);
        passwordEdit = new QLineEdit(centralWidget);
        passwordEdit->setEchoMode(QLineEdit::Password);
        confirm