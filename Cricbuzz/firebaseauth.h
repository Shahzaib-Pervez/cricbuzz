#ifndef FIREBASEAUTH_H
#define FIREBASEAUTH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
<<<<<<< HEAD
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "matchinfo.h"
=======

>>>>>>> a4ce4ea0f5a1005722be20c3afda11ecdfaa26df
class FirebaseAuth : public QObject
{
    Q_OBJECT

public:
    explicit FirebaseAuth(const QString &apiKey, QObject *parent = nullptr);

    // Email/Password Authentication
    void signInWithEmailAndPassword(const QString &email, const QString &password);
    void createUserWithEmailAndPassword(const QString &email, const QString &password);

    // User Profile Management
    void updateUserProfile(const QString &idToken, const QString &displayName, const QString &photoUrl = "");
    void updateUserEmail(const QString &idToken, const QString &newEmail);
    void updateUserPassword(const QString &idToken, const QString &newPassword);

    // Token Management
    void refreshIdToken(const QString &refreshToken);

signals:
    // Authentication Success Signals
    void signInSuccess(const QString &idToken, const QString &refreshToken, const QString &localId, const QString &email, const QString &displayName);
    void signUpSuccess(const QString &idToken, const QString &refreshToken, const QString &localId, const QString &email);

    // Authentication Error Signals
    void signInError(const QString &errorMessage);
    void signUpError(const QString &errorMessage);

    // Profile Update Signals
    void profileUpdateSuccess(const QString &operation, const QJsonObject &userData);
    void profileUpdateError(const QString &operation, const QString &errorMessage);

    // Token Refresh Signals
    void tokenRefreshSuccess(const QString &newIdToken, const QString &newRefreshToken);
    void tokenRefreshError(const QString &errorMessage);

private slots:
    void onNetworkReplyFinished(QNetworkReply *reply);

private:
    enum class AuthOperation {
        None,
        SignIn,
        SignUp,
        UpdateProfile,
        UpdateEmail,
        UpdatePassword,
        RefreshToken
    };

    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    AuthOperation m_currentOperation;
    QString m_currentOperationName;

    // Helper methods
    QString parseFirebaseError(const QJsonObject &errorResponse);
    void handleSignInResponse(const QJsonObject &response);
    void handleSignUpResponse(const QJsonObject &response);
    void handleProfileUpdateResponse(const QJsonObject &response);
    void handleTokenRefreshResponse(const QJsonObject &response);
    void handleErrorResponse(const QJsonObject &response);
};

#endif // FIREBASEAUTH_H
