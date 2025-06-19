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