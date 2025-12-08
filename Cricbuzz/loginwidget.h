#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "matchinfo.h"
class FirebaseAuth;

// Input field widget for email/password inputs
class InputField : public QWidget
{
    Q_OBJECT

public:
    explicit InputField(const QString &iconPath,
                        const QString &placeholder,
                        bool isPassword = false,
                        QWidget *parent = nullptr);

    QString text() const;
    void setText(const QString &text);
    void clear();
    void setFocus();

signals:
    void textChanged(const QString &text);

private:
    QLabel *m_iconLabel;
    QLineEdit *m_lineEdit;
};

// Registration form widget
class RegistrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);

    QString getFullName() const;
    void clearForm();

    // Public UI elements for external access
    InputField *nameInput;
    InputField *emailInput;
    InputField *passwordInput;
    QPushButton *registerButton;
    QLabel *errorLabel;

signals:
    void registrationAttempted(const QString &name, const QString &email, const QString &password);
    void validateFormSignal();

private slots:
    void onRegisterClicked();
    void onInputChanged();

private:
    void setupUI();
};

// Login form widget
class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);

    void clearForm();

    // Public UI elements for external access
    InputField *emailInput;
    InputField *passwordInput;
    QPushButton *loginButton;
    QLabel *errorLabel;

signals:
    void loginAttempted(const QString &email, const QString &password);
    void validateFormSignal();

private slots:
    void onLoginClicked();
    void onInputChanged();

private:
    void setupUI();
};

// Main login widget
class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);

    // Profile update methods (can be called from outside)
    void updateUserProfile(const QString &idToken, const QString &displayName, const QString &photoUrl = "");
    void updateUserEmail(const QString &idToken, const QString &newEmail);
    void updateUserPassword(const QString &idToken, const QString &newPassword);

    // Password clearing methods
    void clearLoginPassword();
    void clearRegistrationPasswords();

signals:
    void userAuthenticated(const QString &uid, const QString &idToken, const QString &refreshToken,
                           const QString &email, const QString &displayName);
    void profileUpdateSuccess(const QString &operation, const QJsonObject &userData);
    void profileUpdateFailed(const QString &operation, const QString &errorMessage);
    void minimizeApp();
    void closeApp();

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    // Firebase Auth slots
    void onSignInSuccess(const QString &idToken, const QString &refreshToken,
                         const QString &localId, const QString &email, const QString &displayName);
    void onSignInError(const QString &errorMessage);
    void onSignUpSuccess(const QString &idToken, const QString &refreshToken,
                         const QString &localId, const QString &email);
    void onSignUpError(const QString &errorMessage);

    // Form handling slots
    void handleLoginAttempt(const QString &email, const QString &password);
    void handleRegistrationAttempt(const QString &name, const QString &email, const QString &password);

    // Validation slots
    void onLoginValidate();
    void onRegisterValidate();

    // UI slots
    void slideToLogin();
    void slideToRegister();
    void onInternalMinimizeClicked();
    void onInternalCloseClicked();

private:
    // UI setup methods
    void setupUI();
    void setupAnimations();
    void setupInitialLayout();
    void applyStyles();
    QWidget *createOverlayPanel(bool isLoginInvitePanel);
    QWidget *createCustomWindowControls();
    void updateOverlayContent(bool showLoginInvite);

    // Error display methods
    void showLoginError(const QString &message);
    void showRegistrationError(const QString &message);

    // Firebase services
    FirebaseAuth *m_firebaseAuth;
    QString m_firebaseApiKey;

    // Pending display name for registration
    QString m_pendingDisplayName;

    // UI components
    LoginForm *loginForm;
    RegistrationForm *registrationForm;
    QFrame *panelsFrame;
    QWidget *formPanelContainer;
    QWidget *overlayPanel;
    QPushButton *internalMinimizeButton;
    QPushButton *internalCloseButton;

    // Overlay components
    QLabel *overlayTitle;
    QLabel *overlayText;
    QPushButton *overlayButton;

    // Animation
    QPropertyAnimation *overlayAnimation;

    // State variables
    bool isLoginVisible;
    bool m_initialPositionsSet;
    bool m_dragging = false;
    QPoint m_dragPosition;
};

#endif // LOGINWIDGET_H
