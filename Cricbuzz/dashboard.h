<<<<<<< HEAD
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
=======
#ifndef DASHBOARD_H
#define DASHBOARD_H
#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QTabWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QGroupBox>
#include <random>

// Forward declaration for match data
struct MatchInfo {
    QString matchId;
    QString team1;
    QString team2;
    QString team1Score;
    QString team2Score;
    QString status;
    QString venue;
    QString time;
    QString series;
    bool isLive;
    QString currentOver;
    QString runRate;
    QString requiredRunRate;
    QString team1Flag;
    QString team2Flag;
    QString matchType; // ODI, T20I, Test
};

class MatchCard : public QFrame
{
    Q_OBJECT

public:
    explicit MatchCard(const MatchInfo &matchInfo, QWidget *parent = nullptr);
    void updateMatchData(const MatchInfo &matchInfo);
    const MatchInfo& getMatchInfo() const { return m_matchInfo; }

signals:
    void matchClicked(const QString &matchId);
    void scorecardRequested(const QString &matchId);
    void statsRequested(const QString &matchId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onScorecardClicked();
    void onStatsClicked();
    void updateLiveIndicator();

private:
    void setupUI();
    void applyStyles();

    MatchInfo m_matchInfo;

    // UI components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_headerLayout;
    QHBoxLayout *m_scoreLayout;
    QHBoxLayout *m_bottomLayout;

    QLabel *m_seriesLabel;
    QLabel *m_liveIndicator;
    QLabel *m_matchTypeLabel;
    QLabel *m_team1Label;
    QLabel *m_team1Score;
    QLabel *m_vsLabel;
    QLabel *m_team2Label;
    QLabel *m_team2Score;
    QLabel *m_statusLabel;
    QLabel *m_venueLabel;
    QLabel *m_overLabel;
    QLabel *m_runRateLabel;

    QPushButton *m_scorecardBtn;
    QPushButton *m_statsBtn;

    QTimer *m_blinkTimer;
};

class Dashboard : public QMainWindow
>>>>>>> a4ce4ea0f5a1005722be20c3afda11ecdfaa26df
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
<<<<<<< HEAD
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

=======
    ~Dashboard();

    void setUserInfo(const QString &userId, const QString &email);

signals:
    void logoutRequested();
    void matchSelected(const QString &matchId);
    void scorecardRequested(const QString &matchId);
    void statsRequested(const QString &matchId);

public slots:
    void refreshMatches();
    void updateLiveScores();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onProfileMenuTriggered();
    void onLogoutClicked();
    void onRefreshClicked();
    void onMatchCardClicked(const QString &matchId);
    void onScorecardRequested(const QString &matchId);
    void onStatsRequested(const QString &matchId);
    void onTabChanged(int index);
    void onMinimizeClicked();
    void onCloseClicked();

private:
    void setupUI();
    void setupHeader();
    void setupTabs();
    void setupMatchesTab();
    void setupLiveTab();
    void setupSeriesTab();
    void applyStyles();
    void loadSampleMatches();
    void addMatchCard(const MatchInfo &matchInfo, QWidget *container);
    void createProfileMenu();
    QWidget *createWindowControls();

    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    // Header
    QWidget *m_headerWidget;
    QHBoxLayout *m_headerLayout;
    QLabel *m_logoLabel;
    QLabel *m_titleLabel;
    QPushButton *m_refreshButton;
    QPushButton *m_profileButton;
    QPushButton *m_minimizeButton;
    QPushButton *m_closeButton;
    QMenu *m_profileMenu;
    QAction *m_logoutAction;

    // Content area
    QTabWidget *m_tabWidget;
    QWidget *m_matchesTab;
    QWidget *m_liveTab;
    QWidget *m_seriesTab;

    // Matches tab
    QScrollArea *m_matchesScrollArea;
    QWidget *m_matchesContainer;
    QVBoxLayout *m_matchesLayout;

    // Live matches tab
    QScrollArea *m_liveScrollArea;
    QWidget *m_liveContainer;
    QVBoxLayout *m_liveLayout;

    // Series tab
    QScrollArea *m_seriesScrollArea;
    QWidget *m_seriesContainer;
    QVBoxLayout *m_seriesLayout;

    // Data
    QList<MatchInfo> m_matches;
    QList<MatchCard*> m_matchCards;

    // User info
    QString m_userId;
    QString m_userEmail;

    // Auto refresh timer
    QTimer *m_refreshTimer;

    // Random engine for score updates
    std::mt19937 m_randomEngine;

    // Window dragging
    bool m_dragging = false;
    QPoint m_dragPosition;
};

#endif // DASHBOARD_H
>>>>>>> a4ce4ea0f5a1005722be20c3afda11ecdfaa26df
