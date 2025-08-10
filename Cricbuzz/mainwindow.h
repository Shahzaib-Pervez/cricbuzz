#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QProgressBar>
#include <QFrame>
#include <QDateTime>
#include "firebaseauth.h"
#include "loginwidget.h"  // Add this include

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Login functionality
    void onLoginButtonClicked();
    void onAuthenticationSuccessful(const QString &userId, const QString &email, const QString &idToken);
    void onAuthenticationFailed(const QString &errorMessage);

    // UI state management
    void setLoginState(bool isLoading);
    void showLoginForm();
    void showMainInterface();
    void onLogoutButtonClicked();

private:
    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    // Login widget (your existing beautiful UI)
    LoginWidget *m_loginWidget;

    // Legacy login form components (not used but kept for compatibility)
    QVBoxLayout *m_loginLayout;
    QFormLayout *m_loginFormLayout;
    QLabel *m_titleLabel;
    QLineEdit *m_emailLineEdit;
    QLineEdit *m_passwordLineEdit;
    QPushButton *m_loginButton;
    QProgressBar *m_loadingBar;
    QLabel *m_statusLabel;

    // Main interface components (after login)
    QWidget *m_mainWidget;
    QVBoxLayout *m_mainInterfaceLayout;
    QLabel *m_welcomeLabel;
    QLabel *m_userInfoLabel;
    QPushButton *m_logoutButton;

    // Firebase authentication
    FirebaseAuth *m_firebaseAuth;

    // User data
    QString m_currentUserId;
    QString m_currentUserEmail;
    QString m_currentIdToken;

    // Helper methods
    void setupUI();
    void setupLoginForm();
    void setupMainInterface();
    void setupFirebaseAuth();
    bool validateLoginInput();
    void clearLoginForm();
    void applyStyles();
    QWidget* createWindowControls(); // Add this method
};

#endif // MAINWINDOW_H
