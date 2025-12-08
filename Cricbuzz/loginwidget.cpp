#include "LoginWidget.h"
#include "FirebaseAuth.h"
#include <QApplication>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QPainter>
#include <QResizeEvent>
#include <QStackedLayout>
#include <QTimer>

// InputField Implementation
InputField::InputField(const QString &iconPath,
                       const QString &placeholder,
                       bool isPassword,
                       QWidget *parent)
    : QWidget(parent)
{
    setObjectName("inputField");
    setMinimumHeight(48);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName("inputIcon");
    QIcon icon(iconPath);
    if (!icon.isNull()) {
        int iconDisplaySize = 20;
        m_iconLabel->setPixmap(icon.pixmap(QSize(iconDisplaySize, iconDisplaySize)));
        m_iconLabel->setFixedSize(iconDisplaySize, iconDisplaySize);
    } else {
        m_iconLabel->setText("?");
        m_iconLabel->setFixedSize(18, 18);
        qWarning() << "InputField: Icon not found or invalid at path:" << iconPath;
    }
    m_iconLabel->setAlignment(Qt::AlignCenter);

    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setPlaceholderText(placeholder);
    m_lineEdit->setFrame(false);
    m_lineEdit->setObjectName("inputText");
    if (isPassword) {
        m_lineEdit->setEchoMode(QLineEdit::Password);
    }

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 5, 0);
    layout->setSpacing(8);
    layout->addWidget(m_iconLabel);
    layout->addWidget(m_lineEdit);
    setLayout(layout);

    connect(m_lineEdit, &QLineEdit::textChanged, this, &InputField::textChanged);
}

QString InputField::text() const
{
    return m_lineEdit->text();
}

void InputField::setText(const QString &text)
{
    m_lineEdit->setText(text);
}

void InputField::clear()
{
    m_lineEdit->clear();
}

void InputField::setFocus()
{
    m_lineEdit->setFocus();
}

// RegistrationForm Implementation
RegistrationForm::RegistrationForm(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("formWidget");
    setupUI();
}

void RegistrationForm::setupUI()
{
    QVBoxLayout *formLayout = new QVBoxLayout(this);
    formLayout->setContentsMargins(40, 20, 40, 20);
    formLayout->setSpacing(15);
    formLayout->addStretch(1);

    QLabel *titleLabel = new QLabel("Create Account", this);
    titleLabel->setObjectName("formTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(titleLabel);

    QLabel *separatorLabel = new QLabel("Please provide your details:", this);
    separatorLabel->setObjectName("formSeparator");
    separatorLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(separatorLabel);

    nameInput = new InputField(":/assets/icons/user_icon.svg", "Full Name", false, this);
    formLayout->addWidget(nameInput);

    emailInput = new InputField(":/assets/icons/email_icon.svg", "Email Address", false, this);
    formLayout->addWidget(emailInput);

    passwordInput = new InputField(":/assets/icons/lock_icon.svg",
                                   "Password (min 6 chars)",
                                   true,
                                   this);
    formLayout->addWidget(passwordInput);

    errorLabel = new QLabel("", this);
    errorLabel->setObjectName("errorLabel");
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);
    errorLabel->setVisible(false);
    formLayout->addWidget(errorLabel);

    registerButton = new QPushButton("SIGN UP", this);
    registerButton->setObjectName("formActionButton");
    registerButton->setMinimumHeight(45);
    registerButton->setCursor(Qt::PointingHandCursor);
    registerButton->setEnabled(false);
    formLayout->addWidget(registerButton);

    formLayout->addStretch(1);

    connect(registerButton, &QPushButton::clicked, this, &RegistrationForm::onRegisterClicked);
    connect(nameInput, &InputField::textChanged, this, &RegistrationForm::onInputChanged);
    connect(emailInput, &InputField::textChanged, this, &RegistrationForm::onInputChanged);
    connect(passwordInput, &InputField::textChanged, this, &RegistrationForm::onInputChanged);
}

void RegistrationForm::onRegisterClicked()
{
    QString name = nameInput->text().trimmed();
    QString email = emailInput->text().trimmed();
    QString password = passwordInput->text();

    errorLabel->setVisible(false);

    if (name.isEmpty() || email.isEmpty() || password.isEmpty()) {
        errorLabel->setText("Please fill in all fields.");
        errorLabel->setVisible(true);
        return;
    }

    if (!email.contains('@') || !email.contains('.')) {
        errorLabel->setText("Please enter a valid email.");
        errorLabel->setVisible(true);
        return;
    }

    if (password.length() < 6) {
        errorLabel->setText("Password must be at least 6 characters.");
        errorLabel->setVisible(true);
        return;
    }

    emit registrationAttempted(name, email, password);
}

void RegistrationForm::onInputChanged()
{
    emit validateFormSignal();
}

void RegistrationForm::clearForm()
{
    nameInput->clear();
    emailInput->clear();
    passwordInput->clear();
    errorLabel->setVisible(false);
    onInputChanged();
}

QString RegistrationForm::getFullName() const
{
    return nameInput->text().trimmed();
}

// LoginForm Implementation
LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("formWidget");
    setupUI();
}

void LoginForm::setupUI()
{
    QVBoxLayout *formLayout = new QVBoxLayout(this);
    formLayout->setContentsMargins(40, 20, 40, 20);
    formLayout->setSpacing(15);
    formLayout->addStretch(1);

<<<<<<< HEAD
    QLabel *titleLabel = new QLabel("Sign in to Cricphy", this);
=======
    QLabel *titleLabel = new QLabel("Sign in to Cinephile", this);
>>>>>>> a4ce4ea0f5a1005722be20c3afda11ecdfaa26df
    titleLabel->setObjectName("formTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(titleLabel);

    QLabel *separatorLabel = new QLabel("Enter your credentials:", this);
    separatorLabel->setObjectName("formSeparator");
    separatorLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(separatorLabel);

    emailInput = new InputField(":/assets/icons/email_icon.svg", "Email Address", false, this);
    formLayout->addWidget(emailInput);

    passwordInput = new InputField(":/assets/icons/lock_icon.svg", "Password", true, this);
    formLayout->addWidget(passwordInput);

    errorLabel = new QLabel("", this);
    errorLabel->setObjectName("errorLabel");
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);
    errorLabel->setVisible(false);
    formLayout->addWidget(errorLabel);

    loginButton = new QPushButton("SIGN IN", this);
    loginButton->setObjectName("formActionButton");
    loginButton->setMinimumHeight(45);
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setEnabled(false);
    formLayout->addWidget(loginButton);

    formLayout->addStretch(1);

    connect(loginButton, &QPushButton::clicked, this, &LoginForm::onLoginClicked);
    connect(emailInput, &InputField::textChanged, this, &LoginForm::onInputChanged);
    connect(passwordInput, &InputField::textChanged, this, &LoginForm::onInputChanged);
}

void LoginForm::onLoginClicked()
{
    QString email = emailInput->text().trimmed();
    QString password = passwordInput->text();

    errorLabel->setVisible(false);

    if (email.isEmpty() || password.isEmpty()) {
        errorLabel->setText("Please enter email and password.");
        errorLabel->setVisible(true);
        return;
    }

    if (!email.contains('@') || !email.contains('.')) {
        errorLabel->setText("Please enter a valid email.");
        errorLabel->setVisible(true);
        return;
    }

    emit loginAttempted(email, password);
}

void LoginForm::onInputChanged()
{
    emit validateFormSignal();
}

void LoginForm::clearForm()
{
    emailInput->clear();
    passwordInput->clear();
    errorLabel->setVisible(false);
    onInputChanged();
}

// LoginWidget Implementation
LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , m_firebaseApiKey("AIzaSyAkWEZyrIT_xpw5dA1fmAaNBnaXO-Dnpog") // Replace with your actual API key
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("loginWidgetBase");
    setupUI();
    applyStyles();
    setupAnimations();
    isLoginVisible = true;
    m_initialPositionsSet = false;

    // Initialize Firebase Auth
    m_firebaseAuth = new FirebaseAuth(m_firebaseApiKey, this);

    // Connect Firebase Auth signals
    connect(m_firebaseAuth, &FirebaseAuth::signInSuccess,
            this, &LoginWidget::onSignInSuccess);
    connect(m_firebaseAuth, &FirebaseAuth::signInError,
            this, &LoginWidget::onSignInError);
    connect(m_firebaseAuth, &FirebaseAuth::signUpSuccess,
            this, &LoginWidget::onSignUpSuccess);
    connect(m_firebaseAuth, &FirebaseAuth::signUpError,
            this, &LoginWidget::onSignUpError);

    // Connect form signals
    connect(loginForm, &LoginForm::loginAttempted, this, &LoginWidget::handleLoginAttempt);
    connect(registrationForm, &RegistrationForm::registrationAttempted,
            this, &LoginWidget::handleRegistrationAttempt);
    connect(loginForm, &LoginForm::validateFormSignal, this, &LoginWidget::onLoginValidate);
    connect(registrationForm, &RegistrationForm::validateFormSignal,
            this, &LoginWidget::onRegisterValidate);
}

void LoginWidget::handleLoginAttempt(const QString &email, const QString &password)
{
    loginForm->loginButton->setEnabled(false);
    loginForm->errorLabel->setText("Signing in...");
    loginForm->errorLabel->setVisible(true);

    m_firebaseAuth->signInWithEmailAndPassword(email, password);
}

void LoginWidget::handleRegistrationAttempt(const QString &name, const QString &email, const QString &password)
{
    registrationForm->registerButton->setEnabled(false);
    registrationForm->errorLabel->setText("Creating account...");
    registrationForm->errorLabel->setVisible(true);

    // Store the display name for later use
    m_pendingDisplayName = name;

    m_firebaseAuth->createUserWithEmailAndPassword(email, password);
}

void LoginWidget::onSignInSuccess(const QString &idToken, const QString &refreshToken,
                                  const QString &localId, const QString &email,
                                  const QString &displayName)
{
    loginForm->loginButton->setEnabled(true);
    loginForm->errorLabel->setVisible(false);

    // Store authentication data and emit success
    emit userAuthenticated(localId, idToken, refreshToken, email, displayName);
}

void LoginWidget::onSignInError(const QString &errorMessage)
{
    loginForm->loginButton->setEnabled(true);
    showLoginError(errorMessage);
}

void LoginWidget::onSignUpSuccess(const QString &idToken, const QString &refreshToken,
                                  const QString &localId, const QString &email)
{
    registrationForm->registerButton->setEnabled(true);
    registrationForm->errorLabel->setVisible(false);

    // Update profile with display name if provided
    if (!m_pendingDisplayName.isEmpty()) {
        m_firebaseAuth->updateUserProfile(idToken, m_pendingDisplayName);
    }

    // Emit success with the display name
    emit userAuthenticated(localId, idToken, refreshToken, email, m_pendingDisplayName);

    // Clear pending display name
    m_pendingDisplayName.clear();
}

void LoginWidget::onSignUpError(const QString &errorMessage)
{
    registrationForm->registerButton->setEnabled(true);
    showRegistrationError(errorMessage);
}

// Remove the onFirestoreUserProfileChecked method entirely since Firestore is not used

// Profile update methods
void LoginWidget::updateUserProfile(const QString &idToken, const QString &displayName, const QString &photoUrl)
{
    m_firebaseAuth->updateUserProfile(idToken, displayName, photoUrl);
}

void LoginWidget::updateUserEmail(const QString &idToken, const QString &newEmail)
{
    m_firebaseAuth->updateUserEmail(idToken, newEmail);
}

void LoginWidget::updateUserPassword(const QString &idToken, const QString &newPassword)
{
    m_firebaseAuth->updateUserPassword(idToken, newPassword);
}

// UI Setup Methods
QWidget *LoginWidget::createCustomWindowControls()
{
    QWidget *controlWidget = new QWidget(this);
    controlWidget->setObjectName("customControlWidget");
    QHBoxLayout *layout = new QHBoxLayout(controlWidget);
    layout->setContentsMargins(0, 0, 10, 5);
    layout->setSpacing(5);
    layout->addStretch();

    internalMinimizeButton = new QPushButton("—", controlWidget);
    internalMinimizeButton->setObjectName("internalMinimizeButton");
    internalMinimizeButton->setFixedSize(28, 28);
    internalMinimizeButton->setFlat(true);
    internalMinimizeButton->setToolTip("Minimize");
    connect(internalMinimizeButton, &QPushButton::clicked, this, &LoginWidget::onInternalMinimizeClicked);
    layout->addWidget(internalMinimizeButton);

    internalCloseButton = new QPushButton("✕", controlWidget);
    internalCloseButton->setObjectName("internalCloseButton");
    internalCloseButton->setFixedSize(28, 28);
    internalCloseButton->setFlat(true);
    internalCloseButton->setToolTip("Close");
    connect(internalCloseButton, &QPushButton::clicked, this, &LoginWidget::onInternalCloseClicked);
    layout->addWidget(internalCloseButton);

    controlWidget->setFixedHeight(35);
    return controlWidget;
}

void LoginWidget::setupUI()
{
    QStackedLayout *outerMostLayout = new QStackedLayout(this);
    outerMostLayout->setStackingMode(QStackedLayout::StackAll);

    panelsFrame = new QFrame(this);
    panelsFrame->setObjectName("panelsFrame");
    panelsFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    outerMostLayout->addWidget(panelsFrame);
    formPanelContainer = new QWidget(panelsFrame);
    formPanelContainer->setObjectName("formPanelContainer");

    QStackedLayout *formStack = new QStackedLayout();
    loginForm = new LoginForm();
    registrationForm = new RegistrationForm();

    formStack->addWidget(loginForm);
    formStack->addWidget(registrationForm);
    formPanelContainer->setLayout(formStack);

    overlayPanel = createOverlayPanel(false);
    overlayPanel->setParent(panelsFrame);

    QWidget *controls = createCustomWindowControls();
    outerMostLayout->addWidget(controls);
    controls->raise();
    setLayout(outerMostLayout);
}

QWidget *LoginWidget::createOverlayPanel(bool isLoginInvitePanel)
{
    QWidget *panel = new QWidget();
    panel->setObjectName("overlayPanel");

    QVBoxLayout *overlayLayout = new QVBoxLayout(panel);
    overlayLayout->setContentsMargins(50, 80, 50, 80);
    overlayLayout->setSpacing(25);
    overlayLayout->setAlignment(Qt::AlignCenter);

    overlayTitle = new QLabel(panel);
    overlayTitle->setObjectName("overlayTitle");
    overlayTitle->setAlignment(Qt::AlignCenter);
    overlayTitle->setWordWrap(true);

    overlayText = new QLabel(panel);
    overlayText->setObjectName("overlayText");
    overlayText->setAlignment(Qt::AlignCenter);
    overlayText->setWordWrap(true);

    overlayButton = new QPushButton(panel);
    overlayButton->setObjectName("overlayButton");
    overlayButton->setMinimumSize(160, 45);
    overlayButton->setCursor(Qt::PointingHandCursor);

    overlayLayout->addStretch(1);
    overlayLayout->addWidget(overlayTitle);
    overlayLayout->addWidget(overlayText);
    overlayLayout->addSpacing(15);
    overlayLayout->addWidget(overlayButton, 0, Qt::AlignCenter);
    overlayLayout->addStretch(1);

    updateOverlayContent(isLoginInvitePanel);
    return panel;
}

void LoginWidget::updateOverlayContent(bool showLoginInvite)
{
    overlayButton->disconnect();

    if (showLoginInvite) {
        overlayTitle->setText("One of Us?");
        overlayText->setText("If you already have an account, just sign in. We've missed you!");
        overlayButton->setText("SIGN IN");
        connect(overlayButton, &QPushButton::clicked, this, &LoginWidget::slideToLogin);
    } else {
        overlayTitle->setText("New Here?");
        overlayText->setText("Sign up and discover a great amount of new opportunities!");
        overlayButton->setText("SIGN UP");
        connect(overlayButton, &QPushButton::clicked, this, &LoginWidget::slideToRegister);
    }
}

void LoginWidget::applyStyles()
{
    QString styleSheet = QString(
        "QWidget#loginWidgetBase { background-color: transparent; }"
        "QFrame#panelsFrame { background-color: transparent; border: none; }"
        "QWidget#formPanelContainer { background-color: #FFFFFF; border-radius: 10px; }"
        "QWidget#overlayPanel { background-color: #2C3E50; border-radius: 10px; }"
        "QWidget#formWidget { background-color: transparent; }"
        "QLabel#formTitle { color: #E74C3C; font-size: 28px; font-weight: bold; margin-bottom: 20px; }"
        "QLabel#formSeparator { color: #2C3E50; font-size: 12px; margin-top: 15px; margin-bottom: 15px; }"
        "QPushButton#formActionButton {"
        "   background-color: #E74C3C;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 25px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   margin-top: 15px;"
        "   min-width: 150px;"
        "}"
        "QPushButton#formActionButton:hover { background-color: #C0392B; }"
        "QPushButton#formActionButton:pressed { background-color: #A93226; }"
        "QPushButton#formActionButton:disabled { background-color: #CCCCCC; color: #666666; }"
        "QLabel#errorLabel { color: #E74C3C; font-size: 12px; font-weight: 500; min-height: 16px; }"
        "QWidget#inputField { background-color: #FFFFFF; border-radius: 6px; border: 1px solid #CCCCCC; }"
        "QWidget#inputField:focus-within { border: 1px solid #E74C3C; }"
        "QLabel#inputIcon { background-color: transparent; }"
        "QLineEdit#inputText { background-color: transparent; border: none; color: #2C3E50; font-size: 14px; padding: 8px 5px; selection-background-color: #E74C3C; selection-color: #FFFFFF; }"
        "QLineEdit#inputText::placeholder { color: #999999; }"
        "QLabel#overlayTitle { color: #EEEBDD; font-size: 28px; font-weight: bold; }"
        "QLabel#overlayText { color: #EEEBDD; font-size: 14px; }"
        "QPushButton#overlayButton {"
        "   background-color: transparent;"
        "   color: #EEEBDD;"
        "   border: 2px solid #EEEBDD;"
        "   border-radius: 8px;"
        "   padding: 10px 25px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton#overlayButton:hover {"
        "   background-color: #FFFFFF;"
        "   border-color: #FFFFFF;"
        "   color: #2C3E50;"
        "}"
        "QPushButton#overlayButton:pressed {"
        "   background-color: rgba(255, 255, 255, 0.9);"
        "   color: #2C3E50;"
        "   border-color: rgba(255, 255, 255, 0.9);"
        "}"
        "QWidget#customControlWidget { background-color: transparent; }"
        "QPushButton#internalMinimizeButton, QPushButton#internalCloseButton { "
        "background-color: transparent; border: none; border-radius: 4px; padding: 4px; "
        "color: #EEEBDD; font-size: 14px; font-weight: bold; }"
        "QPushButton#internalMinimizeButton:hover { background-color: #666666; }"
        "QPushButton#internalCloseButton:hover { background-color: #CE1212; }"
        "QPushButton#internalMinimizeButton:pressed { background-color: #555555; }"
        "QPushButton#internalCloseButton:pressed { background-color: #AA0000; }");

    this->setStyleSheet(styleSheet);

    if (panelsFrame) {
        if (panelsFrame->graphicsEffect()) {
            delete panelsFrame->graphicsEffect();
        }

        auto shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(25);
        shadow->setXOffset(0);
        shadow->setYOffset(4);
        shadow->setColor(QColor(0, 0, 0, 50)); // Shadow color

        panelsFrame->setGraphicsEffect(shadow);
    }
}

void LoginWidget::setupAnimations()
{
    int duration = 450;
    QEasingCurve curve = QEasingCurve::InOutCubic;

    if (!overlayPanel)
        return;
    if (!overlayAnimation) {
        overlayAnimation = new QPropertyAnimation(overlayPanel, "pos", this);
    }

    overlayAnimation->setDuration(duration);
    overlayAnimation->setEasingCurve(curve);
}

void LoginWidget::setupInitialLayout()
{
    if (m_initialPositionsSet)
        return;
    if (!panelsFrame || !overlayPanel || !formPanelContainer || !loginForm || !registrationForm
        || !this->layout()) {
        qWarning() << "setupInitialLayout: Required widgets or layout missing.";
        QTimer::singleShot(10, this, &LoginWidget::setupInitialLayout);
        return;
    }

    const int initialWidth = 850;
    const int initialHeight = 600;
    if (qAbs(this->width() - initialWidth) > 50 || qAbs(this->height() - initialHeight) > 50) {
        qWarning() << "setupInitialLayout: Widget size" << this->size()
        << "differs significantly from target" << initialWidth << "x" << initialHeight
        << ". Retrying...";
        QTimer::singleShot(20, this, &LoginWidget::setupInitialLayout);
        return;
    }

    int panelWidth = initialWidth / 2;
    int panelHeight = initialHeight;
    qDebug() << "setupInitialLayout: Setting initial panel size based on target:" << panelWidth
             << "x" << panelHeight;

    formPanelContainer->setFixedSize(panelWidth, panelHeight);
    overlayPanel->setFixedSize(panelWidth, panelHeight);

    QStackedLayout *formStack = static_cast<QStackedLayout *>(formPanelContainer->layout());
    if (!formStack)
        return;

    if (isLoginVisible) {
        formPanelContainer->move(0, 0);
        overlayPanel->move(panelWidth, 0);
        formStack->setCurrentWidget(loginForm);
        updateOverlayContent(false);
    } else {
        formPanelContainer->move(panelWidth, 0);
        overlayPanel->move(0, 0);
        formStack->setCurrentWidget(registrationForm);
        updateOverlayContent(true);
    }

    m_initialPositionsSet = true;

    QWidget *controls = findChild<QWidget *>("customControlWidget");
    if (controls) {
        int controlsWidth = controls->sizeHint().width();
        int controlsHeight = controls->sizeHint().height();
        if (controlsWidth <= 0)
            controlsWidth = 80;
        if (controlsHeight <= 0)
            controlsHeight = 35;
        controls->setGeometry(initialWidth - controlsWidth - 10, 5, controlsWidth, controlsHeight);
        controls->raise();
        qDebug() << "Controls positioned based on initial width.";
    }
}

// Event handlers
void LoginWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if (!m_initialPositionsSet) {
        QTimer::singleShot(0, this, &LoginWidget::setupInitialLayout);
    }
}

void LoginWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_initialPositionsSet) {
        int newWidth = event->size().width();
        int newHeight = event->size().height();
        int panelWidth = newWidth / 2;
        int panelHeight = newHeight;

        if (formPanelContainer)
            formPanelContainer->setFixedSize(panelWidth, panelHeight);
        if (overlayPanel)
            overlayPanel->setFixedSize(panelWidth, panelHeight);

        if (isLoginVisible) {
            if (formPanelContainer)
                formPanelContainer->move(0, 0);
            if (overlayPanel)
                overlayPanel->move(panelWidth, 0);
        } else {
            if (overlayPanel)
                overlayPanel->move(0, 0);
            if (formPanelContainer)
                formPanelContainer->move(panelWidth, 0);
        }

        QWidget *controls = findChild<QWidget *>("customControlWidget");
        if (controls)
            controls->move(newWidth - controls->width() - 10, controls->y());
    } else {
        QTimer::singleShot(0, this, &LoginWidget::setupInitialLayout);
    }
}

// Animation methods
void LoginWidget::slideToRegister()
{
    if (!m_initialPositionsSet || !overlayAnimation || !isLoginVisible || !panelsFrame
        || !formPanelContainer || !registrationForm)
        return;

    int panelWidth = panelsFrame->width() / 2;
    if (panelWidth <= 0)
        return;

    isLoginVisible = false;
    formPanelContainer->move(panelWidth, 0);

    overlayAnimation->stop();
    overlayAnimation->setStartValue(overlayPanel->pos());
    overlayAnimation->setEndValue(QPoint(0, 0));
    overlayAnimation->start();

    updateOverlayContent(true);

    int delayMs = 150;
    QTimer::singleShot(delayMs, this, [this]() {
        if (registrationForm) {
            static_cast<QStackedLayout *>(formPanelContainer->layout())
            ->setCurrentWidget(registrationForm);
            registrationForm->clearForm();
        }
    });
}

void LoginWidget::slideToLogin()
{
    if (!m_initialPositionsSet || !overlayAnimation || isLoginVisible || !panelsFrame
        || !formPanelContainer || !loginForm)
        return;

    int panelWidth = panelsFrame->width() / 2;
    if (panelWidth <= 0)
        return;

    isLoginVisible = true;
    formPanelContainer->move(0, 0);

    overlayAnimation->stop();
    overlayAnimation->setStartValue(overlayPanel->pos());
    overlayAnimation->setEndValue(QPoint(panelWidth, 0));
    overlayAnimation->start();

    updateOverlayContent(false);

    int delayMs = 150;
    QTimer::singleShot(delayMs, this, [this]() {
        if (loginForm) {
            static_cast<QStackedLayout *>(formPanelContainer->layout())->setCurrentWidget(loginForm);
            loginForm->clearForm();
        }
    });
}

// Form validation
void LoginWidget::onLoginValidate()
{
    if (!loginForm || !loginForm->loginButton)
        return;

    bool emailOk = !loginForm->emailInput->text().trimmed().isEmpty();
    bool passOk = !loginForm->passwordInput->text().isEmpty();
    loginForm->loginButton->setEnabled(emailOk && passOk);

    if (loginForm->errorLabel->isVisible() && (emailOk || passOk)) {
        loginForm->errorLabel->setVisible(false);
    }
}

void LoginWidget::onRegisterValidate()
{
    if (!registrationForm || !registrationForm->registerButton)
        return;

    bool nameOk = !registrationForm->nameInput->text().trimmed().isEmpty();
    bool emailOk = !registrationForm->emailInput->text().trimmed().isEmpty();
    bool passOk = !registrationForm->passwordInput->text().isEmpty();
    registrationForm->registerButton->setEnabled(nameOk && emailOk && passOk);

    if (registrationForm->errorLabel->isVisible() && (nameOk || emailOk || passOk)) {
        registrationForm->errorLabel->setVisible(false);
    }
}

// Error display methods
void LoginWidget::showLoginError(const QString &message)
{
    if (loginForm && loginForm->errorLabel) {
        loginForm->errorLabel->setText(message);
        loginForm->errorLabel->setVisible(true);
    }
}

void LoginWidget::showRegistrationError(const QString &message)
{
    if (registrationForm && registrationForm->errorLabel) {
        registrationForm->errorLabel->setText(message);
        registrationForm->errorLabel->setVisible(true);
    }
}

// Password clearing methods
void LoginWidget::clearLoginPassword()
{
    if (loginForm && loginForm->passwordInput) {
        loginForm->passwordInput->clear();
    }
}

void LoginWidget::clearRegistrationPasswords()
{
    if (registrationForm && registrationForm->passwordInput) {
        registrationForm->passwordInput->clear();
    }
}

// Mouse event handling for window dragging
void LoginWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget *child = childAt(event->position().toPoint());
    bool isInteractive = false;
    while (child != nullptr && child != this) {
        if (qobject_cast<QPushButton *>(child) || qobject_cast<QLineEdit *>(child)) {
            isInteractive = true;
            break;
        }
        child = qobject_cast<QWidget *>(child->parent());
    }

    QWidget *controls = findChild<QWidget *>("customControlWidget");
    if (controls && controls->geometry().contains(event->position().toPoint())) {
        isInteractive = true;
    }

    if (!isInteractive && event->button() == Qt::LeftButton && window()) {
        m_dragPosition = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        m_dragging = true;
        event->accept();
        return;
    }

    m_dragging = false;
    QWidget::mousePressEvent(event);
}

void LoginWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton) && window()) {
        window()->move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void LoginWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

// Window control handlers
void LoginWidget::onInternalMinimizeClicked()
{
    emit minimizeApp();
}

void LoginWidget::onInternalCloseClicked()
{
    emit closeApp();
}
