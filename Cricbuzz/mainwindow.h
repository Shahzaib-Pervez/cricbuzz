#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "matchinfo.h"
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QStackedLayout>
#include <QString>
#include"dashboard.h"
// Forward declarations
class LoginWidget;
class QTimer;
class FirebaseAuth;  // Add this forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    FirebaseAuth* getFirebaseAuth() const;  // Move this after the forward declaration

private slots:
    void onLoginButtonClicked();
    void onAuthenticationSuccessful(const QString &userId, const QString &email, const QString &idToken);
    void onAuthenticationFailed(const QString &errorMessage);
    void onLogoutButtonClicked();
    void showDashboardPlaceholder();

private:
    // Helper methods
    void setupUI();
    void setupLoginForm();
    void setupMainInterface();
    void setupFirebaseAuth();
    void setLoginState(bool isLoading);
    void showLoginForm();
    void showMainInterface();
    bool validateLoginInput();
    void clearLoginForm();
    void applyStyles();
    QWidget* createWindowControls();
    void handleDashboardLogout();
    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    // Login widget
    LoginWidget *m_loginWidget;
    QVBoxLayout *m_loginLayout;
    QVBoxLayout *m_loginFormLayout;
    QLabel *m_titleLabel;
    QLineEdit *m_emailLineEdit;
    QLineEdit *m_passwordLineEdit;
    QPushButton *m_loginButton;
    QProgressBar *m_loadingBar;
    QLabel *m_statusLabel;

    // Main interface (after login)
    QWidget *m_mainWidget;
    QVBoxLayout *m_mainInterfaceLayout;
    QLabel *m_welcomeLabel;
    QLabel *m_userInfoLabel;
    QPushButton *m_logoutButton;

    // Authentication - Use consistent type
    FirebaseAuth *m_firebaseAuth;  // Changed from QObject* to FirebaseAuth*

    // User data
    QString m_currentUserId;
    QString m_currentUserEmail;
    QString m_currentIdToken;
    Dashboard *m_dashboard; // Add this line

    // ... your existing private methods ...
    void openDashboard();
};

#endif // MAINWINDOW_H
