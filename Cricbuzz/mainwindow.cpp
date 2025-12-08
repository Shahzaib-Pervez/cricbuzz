#include "mainwindow.h"
#include "loginwidget.h"
#include "firebaseauth.h"  // Make sure this include matches your actual file name (case-sensitive)
#include "dashboard.h"     // Add this include for Dashboard
#include <QApplication>
#include <QStackedLayout>
#include <QTimer>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDebug>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_loginWidget(nullptr)
    , m_loginLayout(nullptr)
    , m_loginFormLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_emailLineEdit(nullptr)
    , m_passwordLineEdit(nullptr)
    , m_loginButton(nullptr)
    , m_loadingBar(nullptr)
    , m_statusLabel(nullptr)
    , m_mainWidget(nullptr)
    , m_mainInterfaceLayout(nullptr)
    , m_welcomeLabel(nullptr)
    , m_userInfoLabel(nullptr)
    , m_logoutButton(nullptr)
    , m_firebaseAuth(nullptr)
    , m_dashboard(nullptr)  // Initialize dashboard pointer
{
    setupUI();
    setupFirebaseAuth();
    applyStyles();

    // Start with login widget
    showLoginForm();
}

MainWindow::~MainWindow()
{
    // Qt will handle cleanup of child widgets automatically
    if (m_dashboard) {
        m_dashboard->deleteLater();
    }
}

void MainWindow::setupFirebaseAuth()
{
    // Initialize Firebase Auth with your API key
    m_firebaseAuth = new FirebaseAuth("AIzaSyAkWEZyrIT_xpw5dA1fmAaNBnaXO-Dnpog", this);

    // Connect Firebase signals - Use the correct syntax
    connect(m_firebaseAuth, &FirebaseAuth::signInSuccess,
            this, [this](const QString &idToken, const QString &refreshToken,
                   const QString &localId, const QString &email, const QString &displayName) {
                Q_UNUSED(refreshToken)
                Q_UNUSED(displayName)
                onAuthenticationSuccessful(localId, email, idToken);
            });

    connect(m_firebaseAuth, &FirebaseAuth::signInError,
            this, [this](const QString &error) {
                onAuthenticationFailed(error);
            });

    connect(m_firebaseAuth, &FirebaseAuth::signUpSuccess,
            this, [this](const QString &idToken, const QString &refreshToken,
                   const QString &localId, const QString &email) {
                Q_UNUSED(refreshToken)
                onAuthenticationSuccessful(localId, email, idToken);
            });

    connect(m_firebaseAuth, &FirebaseAuth::signUpError,
            this, [this](const QString &error) {
                onAuthenticationFailed(error);
            });
}

void MainWindow::setupUI()
{
    // Set window properties
    setWindowTitle("Cricket App");
    setFixedSize(850, 600);
    setWindowFlags(Qt::FramelessWindowHint);

    // Create central widget with stacked layout
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    QStackedLayout *stackedLayout = new QStackedLayout(m_centralWidget);

    // Setup login widget
    setupLoginForm();

    // Setup main interface
    setupMainInterface();

    // Add both widgets to stacked layout
    stackedLayout->addWidget(m_loginWidget);
    stackedLayout->addWidget(m_mainWidget);

    // Start with login widget visible
    stackedLayout->setCurrentWidget(m_loginWidget);
}

void MainWindow::setupLoginForm()
{
    m_loginWidget = new LoginWidget();

    // Connect LoginWidget signals to MainWindow slots
    connect(m_loginWidget, &LoginWidget::userAuthenticated,
            this, [this](const QString &uid, const QString &idToken, const QString &refreshToken,
                   const QString &email, const QString &displayName) {
                Q_UNUSED(refreshToken)
                Q_UNUSED(displayName)
                onAuthenticationSuccessful(uid, email, idToken);
            });

    // Connect window control signals
    connect(m_loginWidget, &LoginWidget::minimizeApp, this, [this]() {
        this->showMinimized();
    });

    connect(m_loginWidget, &LoginWidget::closeApp, this, [this]() {
        this->close();
    });
}

void MainWindow::setupMainInterface()
{
    // Create main interface widget
    m_mainWidget = new QWidget();
    m_mainWidget->setStyleSheet("background-color: #F8F9FA;");

    m_mainInterfaceLayout = new QVBoxLayout(m_mainWidget);
    m_mainInterfaceLayout->setContentsMargins(50, 50, 50, 50);
    m_mainInterfaceLayout->setSpacing(30);

    // Header section
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    // Welcome label
    m_welcomeLabel = new QLabel("Welcome to Cricket App!");
    m_welcomeLabel->setStyleSheet(R"(
        font-size: 32px;
        font-weight: bold;
        color: #E74C3C;
        margin: 20px 0;
    )");

    // Window controls
    QWidget *controlsWidget = createWindowControls();

    headerLayout->addWidget(m_welcomeLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(controlsWidget);

    // User info section
    QFrame *userInfoFrame = new QFrame();
    userInfoFrame->setFrameStyle(QFrame::StyledPanel);
    userInfoFrame->setStyleSheet(R"(
        QFrame {
            background-color: white;
            border-radius: 15px;
            padding: 30px;
            border: 1px solid #E0E0E0;
        }
    )");

    QVBoxLayout *userInfoLayout = new QVBoxLayout(userInfoFrame);

    QLabel *userSectionTitle = new QLabel("User Information");
    userSectionTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #2C3E50; margin-bottom: 15px;");

    m_userInfoLabel = new QLabel("");
    m_userInfoLabel->setStyleSheet("font-size: 14px; color: #2C3E50; line-height: 1.5;");

    userInfoLayout->addWidget(userSectionTitle);
    userInfoLayout->addWidget(m_userInfoLabel);

    // Action buttons section
    QWidget *actionsWidget = new QWidget();
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setSpacing(20);

    // Logout button
    m_logoutButton = new QPushButton("Logout");
    m_logoutButton->setStyleSheet(R"(
        QPushButton {
            background-color: #E74C3C;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #C0392B;
        }
        QPushButton:pressed {
            background-color: #A93226;
        }
    )");
    m_logoutButton->setCursor(Qt::PointingHandCursor);

    // Dashboard button - Now opens full Dashboard
    QPushButton *dashboardButton = new QPushButton("Open Dashboard");
    dashboardButton->setStyleSheet(R"(
        QPushButton {
            background-color: #27AE60;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 30px;
            font-size: 14px;
            font-weight: bold;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #229954;
        }
        QPushButton:pressed {
            background-color: #1E8449;
        }
    )");
    dashboardButton->setCursor(Qt::PointingHandCursor);
    dashboardButton->setToolTip("Open Cricket Dashboard");

    actionsLayout->addWidget(dashboardButton);
    actionsLayout->addStretch();
    actionsLayout->addWidget(m_logoutButton);

    // Add all sections to main layout
    m_mainInterfaceLayout->addWidget(headerWidget);
    m_mainInterfaceLayout->addWidget(userInfoFrame);
    m_mainInterfaceLayout->addStretch();
    m_mainInterfaceLayout->addWidget(actionsWidget);

    // Connect signals
    connect(m_logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutButtonClicked);
    connect(dashboardButton, &QPushButton::clicked, this, &MainWindow::openDashboard);
}

QWidget* MainWindow::createWindowControls()
{
    QWidget *controlWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(controlWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    QPushButton *minimizeButton = new QPushButton("—");
    minimizeButton->setFixedSize(28, 28);
    minimizeButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            color: #2C3E50;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #BDC3C7;
        }
    )");
    minimizeButton->setCursor(Qt::PointingHandCursor);

    QPushButton *closeButton = new QPushButton("✕");
    closeButton->setFixedSize(28, 28);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            color: #2C3E50;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #E74C3C;
            color: white;
        }
    )");
    closeButton->setCursor(Qt::PointingHandCursor);

    layout->addWidget(minimizeButton);
    layout->addWidget(closeButton);

    connect(minimizeButton, &QPushButton::clicked, this, [this]() {
        this->showMinimized();
    });

    connect(closeButton, &QPushButton::clicked, this, [this]() {
        this->close();
    });

    return controlWidget;
}

void MainWindow::openDashboard()
{
    if (!m_dashboard) {
        // Create dashboard
        m_dashboard = new Dashboard();
        // m_dashboard->setUserInfo(m_currentUserId, m_currentUserEmail);

        // Connect dashboard logout signal
        connect(m_dashboard, &Dashboard::logoutRequested, this, [this]() {
            // Handle dashboard logout
            if (m_dashboard) {
                m_dashboard->close();
                m_dashboard->deleteLater();
                m_dashboard = nullptr;
            }

            // Clear user data
            m_currentUserId.clear();
            m_currentUserEmail.clear();
            m_currentIdToken.clear();

            // Clear LoginWidget passwords for security
            if (m_loginWidget) {
                m_loginWidget->clearLoginPassword();
                m_loginWidget->clearRegistrationPasswords();
            }

            // Show main window with login
            this->show();
            showLoginForm();
        });
    }

    // Show dashboard and hide main window
    m_dashboard->show();
    this->hide();
}

void MainWindow::showDashboardPlaceholder()
{
    // This method is no longer needed but kept for compatibility
    openDashboard();
}

void MainWindow::onLoginButtonClicked()
{
    // This method is handled by LoginWidget
}

void MainWindow::onAuthenticationSuccessful(const QString &userId, const QString &email, const QString &idToken)
{
    // Store user data
    m_currentUserId = userId;
    m_currentUserEmail = email;
    m_currentIdToken = idToken;

    qDebug() << "User authenticated successfully:" << email;

    // Show main interface (not directly opening dashboard)
    showMainInterface();
}

void MainWindow::onAuthenticationFailed(const QString &errorMessage)
{
    QMessageBox::warning(this, "Authentication Failed", errorMessage);
}

void MainWindow::setLoginState(bool isLoading)
{
    Q_UNUSED(isLoading)
    // Login state is handled by LoginWidget
}

void MainWindow::showLoginForm()
{
    QStackedLayout *stackedLayout = qobject_cast<QStackedLayout*>(m_centralWidget->layout());
    if (stackedLayout) {
        stackedLayout->setCurrentWidget(m_loginWidget);
        setWindowTitle("Cricket App - Login");
    }
}

void MainWindow::showMainInterface()
{
    QStackedLayout *stackedLayout = qobject_cast<QStackedLayout*>(m_centralWidget->layout());
    if (stackedLayout) {
        // Update user info display
        m_userInfoLabel->setText(QString(R"(
            <b>Email:</b> %1<br>
            <b>User ID:</b> %2<br>
            <b>Status:</b> <span style="color: #27AE60;">Authenticated</span><br>
            <b>Login Time:</b> %3
        )").arg(m_currentUserEmail)
                                     .arg(m_currentUserId)
                                     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

        stackedLayout->setCurrentWidget(m_mainWidget);
        setWindowTitle("Cricket App - Main");
    }
}

void MainWindow::onLogoutButtonClicked()
{
    // Clear user data
    m_currentUserId.clear();
    m_currentUserEmail.clear();
    m_currentIdToken.clear();

    // Close dashboard if open
    if (m_dashboard) {
        m_dashboard->close();
        m_dashboard->deleteLater();
        m_dashboard = nullptr;
    }

    // Clear LoginWidget passwords for security
    if (m_loginWidget) {
        m_loginWidget->clearLoginPassword();
        m_loginWidget->clearRegistrationPasswords();
    }

    // Show login form
    showLoginForm();
}

bool MainWindow::validateLoginInput()
{
    // Validation is handled by LoginWidget
    return true;
}

void MainWindow::clearLoginForm()
{
    // Form clearing is handled by LoginWidget
}

void MainWindow::applyStyles()
{
    setStyleSheet("QMainWindow { background-color: #F0F0F0; }");
}

// Method to allow LoginWidget to access Firebase Auth
FirebaseAuth* MainWindow::getFirebaseAuth() const
{
    return m_firebaseAuth;
}
