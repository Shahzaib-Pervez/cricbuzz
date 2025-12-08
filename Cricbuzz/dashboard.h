#pragma once

#include <QWidget>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QVector>
#include <QString>
#include <QJsonObject>

#include "matchinfo.h"
#include "matchcard.h"

class QVBoxLayout;
class QScrollArea;
class QLabel;
class QNetworkReply;
class QPushButton;

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard() override;

signals:
    void scorecardRequested(const QString &matchId);
    void logoutRequested();

private slots:
    void onNetworkFinished(QNetworkReply *reply);
    void loadMatches(const QString &type);          // NEW
    void fetchMatchData(const QString &url);        // NEW

private:
    void setupUi();
    QWidget* createHeader();                        // Top UI (Logout etc.)
    bool parseAndAddMatches(const QJsonObject &root);
    void finishLoading(bool hasMatch);

    void addMatchCard(const MatchInfo &info);
    void clearCards();

    // UI Layout
    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scroll;
    QWidget     *m_container;
    QVBoxLayout *m_cardsLayout;
    QLabel      *m_loadingLabel;

    // API
    QNetworkAccessManager *m_networkManager;
    QTimer                *m_refreshTimer;

    QVector<MatchCard*>    m_cards;

    // Buttons (for Live/Recent/Upcoming)
    QPushButton *m_btnLive;
    QPushButton *m_btnRecent;
    QPushButton *m_btnUpcoming;

    // API credentials
    QString m_apiHost;
    QString m_apiKey;

    QString m_currentRequestType;   // live / recent / upcoming
};

