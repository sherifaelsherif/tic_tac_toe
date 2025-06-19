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

    // Test password strength calculation
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
        if (length >= 6) strength++;
        if (length >= 8) strength++;
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
        confirmPasswordEdit = new QLineEdit(centralWidget);
        confirmPasswordEdit->setEchoMode(QLineEdit::Password);
        
        passwordStrengthLabel = new QLabel("PASSWORD STRENGTH: WEAK", centralWidget);
        passwordStrengthLabel->setStyleSheet("font-size: 12px; color: #ff5555;");
        
        registerButton = new QPushButton("REGISTER", centralWidget);
        cancelButton = new QPushButton("CANCEL", centralWidget);
        
        mainLayout->addWidget(usernameEdit);
        mainLayout->addWidget(passwordEdit);
        mainLayout->addWidget(confirmPasswordEdit);
        mainLayout->addWidget(passwordStrengthLabel);
        mainLayout->addWidget(registerButton);
        mainLayout->addWidget(cancelButton);
        
        connect(registerButton, &QPushButton::clicked, this, &TestableRegisterWindow::attemptRegister);
        connect(cancelButton, &QPushButton::clicked, this, &TestableRegisterWindow::cancelRegistration);
        connect(passwordEdit, &QLineEdit::textChanged, this, &TestableRegisterWindow::updatePasswordStrength);
    }

    MockDatabase *mockDb;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *passwordStrengthLabel;
    QPushButton *registerButton;
    QPushButton *cancelButton;
};

class RegisterWindowTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanup();
    
    // UI Tests
    void testUIComponents();
    void testPasswordFieldsHidden();
    void testWindowSize();
    
    // Input Validation Tests
    void testEmptyFieldValidation();
    void testUsernameLengthValidation();
    void testPasswordLengthValidation();
    void testUsernameSpecialCharsValidation();
    void testPasswordMismatchValidation();
    void testValidInputValidation();
    
    // Password Strength Tests
    void testPasswordStrengthWeak();
    void testPasswordStrengthMedium();
    void testPasswordStrengthStrong();
    void testPasswordStrengthUpdate();
    
    // Button Tests
    void testRegisterButtonExists();
    void testCancelButtonExists();
    void testCancelButtonClosesWindow();
    
    // Registration Logic Tests
    void testSuccessfulRegistration();
    void testFailedRegistration();
    void testRegistrationSignalEmitted();
    void testUsernameTrimmingLogic();

private:
    TestableRegisterWindow *window;
    MockDatabase *mockDb;
};

void RegisterWindowTest::initTestCase() {
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        new QApplication(argc, argv);
    }
}

void RegisterWindowTest::init() {
    mockDb = new MockDatabase();
    window = new TestableRegisterWindow(mockDb);
}

void RegisterWindowTest::cleanup() {
    delete window;
    delete mockDb;
}

void RegisterWindowTest::testUIComponents() {
    QVERIFY(window->getUsernameEdit() != nullptr);
    QVERIFY(window->getPasswordEdit() != nullptr);
    QVERIFY(window->getConfirmPasswordEdit() != nullptr);
    QVERIFY(window->getPasswordStrengthLabel() != nullptr);
    QVERIFY(window->getRegisterButton() != nullptr);
    QVERIFY(window->getCancelButton() != nullptr);
}

void RegisterWindowTest::testPasswordFieldsHidden() {
    QCOMPARE(window->getPasswordEdit()->echoMode(), QLineEdit::Password);
    QCOMPARE(window->getConfirmPasswordEdit()->echoMode(), QLineEdit::Password);
}

void RegisterWindowTest::testWindowSize() {
    QVERIFY(window->minimumSize().width() >= 350);
    QVERIFY(window->minimumSize().height() >= 450);
}

void RegisterWindowTest::testEmptyFieldValidation() {
    QVERIFY(!window->validateInput("", "password", "password"));
    QVERIFY(!window->validateInput("username", "", "password"));
    QVERIFY(!window->validateInput("username", "password", ""));
    QVERIFY(!window->validateInput("", "", ""));
}

void RegisterWindowTest::testUsernameLengthValidation() {
    // Too short (less than 3 characters)
    QVERIFY(!window->validateInput("ab", "password123", "password123"));
    // Valid length
    QVERIFY(window->validateInput("abc", "password123", "password123"));
    // Too long (more than 50 characters)
    QString longUsername = QString("a").repeated(51);
    QVERIFY(!window->validateInput(longUsername, "password123", "password123"));
    // Maximum valid length
    QString maxUsername = QString("a").repeated(50);
    QVERIFY(window->validateInput(maxUsername, "password123", "password123"));
}

void RegisterWindowTest::testPasswordLengthValidation() {
    // Too short (less than 6 characters)
    QVERIFY(!window->validateInput("username", "12345", "12345"));
    // Valid length
    QVERIFY(window->validateInput("username", "123456", "123456"));
    // Too long (more than 100 characters)
    QString longPassword = QString("a").repeated(101);
    QVERIFY(!window->validateInput("username", longPassword, longPassword));
    // Maximum valid length
    QString maxPassword = QString("a").repeated(100);
    QVERIFY(window->validateInput("username", maxPassword, maxPassword));
}

void RegisterWindowTest::testUsernameSpecialCharsValidation() {
    // Invalid characters
    QVERIFY(!window->validateInput("user@name", "password123", "password123"));
    QVERIFY(!window->validateInput("user.name", "password123", "password123"));
    QVERIFY(!window->validateInput("user name", "password123", "password123"));
    QVERIFY(!window->validateInput("user#123", "password123", "password123"));
    // Valid characters only
    QVERIFY(window->validateInput("username123", "password123", "password123"));
    QVERIFY(window->validateInput("Username", "password123", "password123"));
    QVERIFY(window->validateInput("123456", "password123", "password123"));
}

void RegisterWindowTest::testPasswordMismatchValidation() {
    QVERIFY(!window->validateInput("username", "password123", "password456"));
    QVERIFY(window->validateInput("username", "password123", "password123"));
}

void RegisterWindowTest::testValidInputValidation() {
    QVERIFY(window->validateInput("validuser", "validpass123", "validpass123"));
    QVERIFY(window->validateInput("User123", "MyPassword456", "MyPassword456"));
}

void RegisterWindowTest::testPasswordStrengthWeak() {
    QCOMPARE(window->calculatePasswordStrength("123"), QString("WEAK"));
    QCOMPARE(window->calculatePasswordStrength("12345"), QString("WEAK"));
    QCOMPARE(window->calculatePasswordStrength("ab"), QString("WEAK"));
}

void RegisterWindowTest::testPasswordStrengthMedium() {
    QCOMPARE(window->calculatePasswordStrength("password"), QString("MEDIUM"));
    QCOMPARE(window->calculatePasswordStrength("PASSWORD"), QString("MEDIUM"));
    QCOMPARE(window->calculatePasswordStrength("Pass123"), QString("MEDIUM"));
}

void RegisterWindowTest::testPasswordStrengthStrong() {
    QCOMPARE(window->calculatePasswordStrength("Password1"), QString("STRONG"));
    QCOMPARE(window->calculatePasswordStrength("Password123!"), QString("STRONG"));
    QCOMPARE(window->calculatePasswordStrength("MyP@ssw0rd"), QString("STRONG"));
}

void RegisterWindowTest::testPasswordStrengthUpdate() {
    QLabel* strengthLabel = window->getPasswordStrengthLabel();
    QLineEdit* passwordEdit = window->getPasswordEdit();
    
    // Test weak password
    passwordEdit->setText("weak");
    window->updatePasswordStrength();
    QVERIFY(strengthLabel->text().contains("WEAK"));
    
    // Test medium password
    passwordEdit->setText("password");
    window->updatePasswordStrength();
    QVERIFY(strengthLabel->text().contains("MEDIUM"));
    
    // Test strong password
    passwordEdit->setText("Password1");
    window->updatePasswordStrength();
    QVERIFY(strengthLabel->text().contains("STRONG"));
}

void RegisterWindowTest::testRegisterButtonExists() {
    QPushButton* button = window->getRegisterButton();
    QVERIFY(button != nullptr);
    QCOMPARE(button->text(), QString("REGISTER"));
}

void RegisterWindowTest::testCancelButtonExists() {
    QPushButton* button = window->getCancelButton();
    QVERIFY(button != nullptr);
    QCOMPARE(button->text(), QString("CANCEL"));
}

void RegisterWindowTest::testCancelButtonClosesWindow() {
    window->show();
    QVERIFY(window->isVisible());
    window->cancelRegistration();
    QVERIFY(!window->isVisible());
}

void RegisterWindowTest::testSuccessfulRegistration() {
    mockDb->setShouldSucceed(true);
    mockDb->reset();
    
    window->getUsernameEdit()->setText("testuser");
    window->getPasswordEdit()->setText("testpass123");
    window->getConfirmPasswordEdit()->setText("testpass123");
    
    QSignalSpy spy(window, &TestableRegisterWindow::registrationSuccessful);
    window->attemptRegister();
    
    QCOMPARE(mockDb->getCallCount(), 1);
    QCOMPARE(mockDb->getLastUsername(), QString("testuser"));
    QCOMPARE(mockDb->getLastPassword(), QString("testpass123"));
    QCOMPARE(spy.count(), 1);
}

void RegisterWindowTest::testFailedRegistration() {
    mockDb->setShouldSucceed(false);
    mockDb->reset();
    
    window->getUsernameEdit()->setText("existinguser");
    window->getPasswordEdit()->setText("testpass123");
    window->getConfirmPasswordEdit()->setText("testpass123");
    
    QSignalSpy spy(window, &TestableRegisterWindow::registrationSuccessful);
    window->attemptRegister();
    
    QCOMPARE(mockDb->getCallCount(), 1);
    QCOMPARE(spy.count(), 0); // No success signal should be emitted
}

void RegisterWindowTest::testRegistrationSignalEmitted() {
    mockDb->setShouldSucceed(true);
    
    window->getUsernameEdit()->setText("newuser");
    window->getPasswordEdit()->setText("newpass123");
    window->getConfirmPasswordEdit()->setText("newpass123");
    
    QSignalSpy spy(window, &TestableRegisterWindow::registrationSuccessful);
    window->attemptRegister();
    
    QCOMPARE(spy.count(), 1);
}

void RegisterWindowTest::testUsernameTrimmingLogic() {
    mockDb->reset();
    
    window->getUsernameEdit()->setText(" trimmeduser ");
    window->getPasswordEdit()->setText("password123");
    window->getConfirmPasswordEdit()->setText("password123");
    
    window->attemptRegister();
    
    QCOMPARE(mockDb->getLastUsername(), QString("trimmeduser"));
}

QTEST_MAIN(RegisterWindowTest)
#include "registerwindow_test.moc"