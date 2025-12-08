#include "FirebaseAuth.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

FirebaseAuth::FirebaseAuth(const QString &apiKey, QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_apiKey(apiKey)
    , m_currentOperation(AuthOperation::None)
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &FirebaseAuth::onNetworkReplyFinished);
}

void FirebaseAuth::signInWithEmailAndPassword(const QString &email, const QString &password)
{
    if (m_apiKey.isEmpty() || m_apiKey.contains("PLACEHOLDER")) {
        emit signInError("Firebase API Key not configured.");
        return;
    }

    m_currentOperation = AuthOperation::SignIn;

    QJsonObject payload;
    payload["email"] = email;
    payload["password"] = password;
    payload["returnSecureToken"] = true;

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=%1").arg(m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager->post(request, jsonData);
}

void FirebaseAuth::createUserWithEmailAndPassword(const QString &email, const QString &password)
{
    if (m_apiKey.isEmpty() || m_apiKey.contains("PLACEHOLDER")) {
        emit signUpError("Firebase API Key not configured.");
        return;
    }

    m_currentOperation = AuthOperation::SignUp;

    QJsonObject payload;
    payload["email"] = email;
    payload["password"] = password;
    payload["returnSecureToken"] = true;

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=%1").arg(m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager->post(request, jsonData);
}

void FirebaseAuth::updateUserProfile(const QString &idToken, const QString &displayName, const QString &photoUrl)
{
    if (m_apiKey.isEmpty()) {
        emit profileUpdateError("UpdateUserProfile", "Firebase API Key not configured.");
        return;
    }

    m_currentOperation = AuthOperation::UpdateProfile;
    m_currentOperationName = "UpdateUserProfile";

    QJsonObject payload;
    payload["idToken"] = idToken;
    if (!displayName.isEmpty()) {
        payload["displayName"] = displayName;
    }
    if (!photoUrl.isEmpty()) {
        payload["photoUrl"] = photoUrl;
    }
    payload["returnSecureToken"] = true;

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:update?key=%1").arg(m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager->post(request, jsonData);
}

void FirebaseAuth::updateUserEmail(const QString &idToken, const QString &newEmail)
{
    if (m_apiKey.isEmpty()) {
        emit profileUpdateError("UpdateUserEmail", "Firebase API Key not configured.");
        return;
    }

    m_currentOperation = AuthOperation::UpdateEmail;
    m_currentOperationName = "UpdateUserEmail";

    QJsonObject payload;
    payload["idToken"] = idToken;
    payload["email"] = newEmail;
    payload["returnSecureToken"] = true;

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:update?key=%1").arg(m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager->post(request, jsonData);
}

void FirebaseAuth::updateUserPassword(const QString &idToken, const QString &newPassword)
{
    if (m_apiKey.isEmpty()) {
        emit profileUpdateError("UpdateUserPassword", "Firebase API Key not configured.");
        return;
    }

    m_currentOperation = AuthOperation::UpdatePassword;
    m_currentOperationName = "UpdateUserPassword";

    QJsonObject payload;
    payload["idToken"] = idToken;
    payload["password"] = newPassword;
    payload["returnSecureToken"] = true;

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QUrl url(QString("https://identitytoolkit.googleapis.com/v1/accounts:update?key=%1").arg(m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager->post(request, jsonData);
}

void FirebaseAuth::refreshIdToken(const QString &refreshToken)
{
    if (m_apiKey.isEmpty()) {
        emit tokenRefreshError("Firebase API Key not configured.");
        return;
    }

    m_currentOperation = AuthOperation::RefreshToken;

    QJsonObject payload;
    payload["grant_type"] = "refresh_token";
    payload["refresh_token"] = refreshToken;

    QJsonDocument doc(payload);
    QByteArray jsonData = doc.toJson();

    QUrl url(QString("https://securetoken.googleapis.com/v1/token?key=%1").arg(m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_networkManager->post(request, jsonData);
}

void FirebaseAuth::onNetworkReplyFinished(QNetworkReply *reply)
{
    if (!reply) {
        qWarning() << "FirebaseAuth: Received null reply";
        return;
    }

    QByteArray responseData = reply->readAll();
    QNetworkReply::NetworkError networkError = reply->error();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        QString errorMsg = "Invalid JSON response from Firebase server.";
        qWarning() << "FirebaseAuth Error:" << errorMsg;
        handleErrorResponse(QJsonObject());
        return;
    }

    QJsonObject jsonResponse = jsonDoc.object();

    if (networkError != QNetworkReply::NoError) {
        qWarning() << "FirebaseAuth Network Error:" << reply->errorString();
        handleErrorResponse(jsonResponse);
        return;
    }

    if (jsonResponse.contains("error")) {
        handleErrorResponse(jsonResponse);
        return;
    }

    // Handle successful responses based on current operation
    switch (m_currentOperation) {
    case AuthOperation::SignIn:
        handleSignInResponse(jsonResponse);
        break;
    case AuthOperation::SignUp:
        handleSignUpResponse(jsonResponse);
        break;
    case AuthOperation::UpdateProfile:
    case AuthOperation::UpdateEmail:
    case AuthOperation::UpdatePassword:
        handleProfileUpdateResponse(jsonResponse);
        break;
    case AuthOperation::RefreshToken:
        handleTokenRefreshResponse(jsonResponse);
        break;
    default:
        qWarning() << "FirebaseAuth: Unknown operation completed";
        break;
    }

    m_currentOperation = AuthOperation::None;
    m_currentOperationName.clear();
}

void FirebaseAuth::handleSignInResponse(const QJsonObject &response)
{
    QString idToken = response["idToken"].toString();
    QString refreshToken = response["refreshToken"].toString();
    QString localId = response["localId"].toString();
    QString email = response["email"].toString();
    QString displayName = response.value("displayName").toString();

    if (idToken.isEmpty() || localId.isEmpty()) {
        emit signInError("Firebase authentication succeeded but missing required data.");
        return;
    }

    emit signInSuccess(idToken, refreshToken, localId, email, displayName);
}

void FirebaseAuth::handleSignUpResponse(const QJsonObject &response)
{
    QString idToken = response["idToken"].toString();
    QString refreshToken = response["refreshToken"].toString();
    QString localId = response["localId"].toString();
    QString email = response["email"].toString();

    if (idToken.isEmpty() || localId.isEmpty()) {
        emit signUpError("Firebase registration succeeded but missing required data.");
        return;
    }

    emit signUpSuccess(idToken, refreshToken, localId, email);
}

void FirebaseAuth::handleProfileUpdateResponse(const QJsonObject &response)
{
    emit profileUpdateSuccess(m_currentOperationName, response);
}

void FirebaseAuth::handleTokenRefreshResponse(const QJsonObject &response)
{
    QString newIdToken = response["id_token"].toString();
    QString newRefreshToken = response["refresh_token"].toString();

    if (newIdToken.isEmpty()) {
        emit tokenRefreshError("Token refresh succeeded but no new token received.");
        return;
    }

    emit tokenRefreshSuccess(newIdToken, newRefreshToken);
}

void FirebaseAuth::handleErrorResponse(const QJsonObject &response)
{
    QString errorMessage;

    if (response.contains("error")) {
        QJsonObject errorObj = response["error"].toObject();
        errorMessage = parseFirebaseError(errorObj);
    } else {
        errorMessage = "Unknown authentication error occurred.";
    }

    switch (m_currentOperation) {
    case AuthOperation::SignIn:
        emit signInError(errorMessage);
        break;
    case AuthOperation::SignUp:
        emit signUpError(errorMessage);
        break;
    case AuthOperation::UpdateProfile:
    case AuthOperation::UpdateEmail:
    case AuthOperation::UpdatePassword:
        emit profileUpdateError(m_currentOperationName, errorMessage);
        break;
    case AuthOperation::RefreshToken:
        emit tokenRefreshError(errorMessage);
        break;
    default:
        qWarning() << "FirebaseAuth: Error for unknown operation:" << errorMessage;
        break;
    }
}

QString FirebaseAuth::parseFirebaseError(const QJsonObject &errorResponse)
{
    QString message = errorResponse["message"].toString();

    // Convert Firebase error codes to user-friendly messages
    if (message == "EMAIL_NOT_FOUND") {
        return "No account found with this email address.";
    } else if (message == "INVALID_PASSWORD") {
        return "Incorrect password.";
    } else if (message == "USER_DISABLED") {
        return "This account has been disabled.";
    } else if (message == "EMAIL_EXISTS") {
        return "An account with this email already exists.";
    } else if (message == "WEAK_PASSWORD") {
        return "Password should be at least 6 characters.";
    } else if (message == "INVALID_EMAIL") {
        return "Invalid email address format.";
    } else if (message == "TOO_MANY_ATTEMPTS_TRY_LATER") {
        return "Too many failed attempts. Please try again later.";
    } else if (message.contains("CREDENTIAL_TOO_OLD")) {
        return "Please sign in again to update your profile.";
    } else {
        return message.isEmpty() ? "Authentication error occurred." : message;
    }
}
