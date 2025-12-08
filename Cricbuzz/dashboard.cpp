#include "dashboard.h"
<<<<<<< HEAD
#include "matchcard.h"
#include "matchinfo.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDebug>
#include <QApplication>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent),
    m_mainLayout(new QVBoxLayout(this)),
    m_scroll(new QScrollArea),
    m_container(new QWidget),
    m_cardsLayout(new QVBoxLayout),
    m_loadingLabel(new QLabel("Loading matches...")),
    m_networkManager(new QNetworkAccessManager(this)),
    m_refreshTimer(new QTimer(this)),
    m_currentRequestType("live")
{
    setupUi();

    // CONNECT NETWORK REPLY
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &Dashboard::onNetworkFinished);

    // AUTO LOAD
    QTimer::singleShot(0, this, [this]() {
        loadMatches("live");
    });

    connect(m_refreshTimer, &QTimer::timeout, this, [this]() {
        loadMatches(m_currentRequestType);
    });

    m_refreshTimer->start(30000); // 30 sec refresh
}

void Dashboard::setupUi()
{
    setStyleSheet("background:#f8f9fa;");
    m_mainLayout->setContentsMargins(0,0,0,0);
    m_mainLayout->setSpacing(0);

    // HEADER (Logout / App name)
    m_mainLayout->addWidget(createHeader());

    // ---------------------- NAVIGATION BUTTONS ----------------------
    QHBoxLayout *nav = new QHBoxLayout();
    nav->setContentsMargins(20, 10, 20, 10);
    nav->setSpacing(15);

    m_btnLive = new QPushButton("LIVE");
    m_btnRecent = new QPushButton("RECENT");
    m_btnUpcoming = new QPushButton("UPCOMING");

    QList<QPushButton*> all = {m_btnLive, m_btnRecent, m_btnUpcoming};
    for (auto *b : all) {
        b->setCheckable(true);
        b->setFixedHeight(40);
        b->setStyleSheet(
            "QPushButton { background:#ddd; border-radius:18px; padding:6px 18px; }"
            "QPushButton:checked { background:#09B83E; color:white; font-weight:bold; }"
            );
        nav->addWidget(b);
    }

    m_btnLive->setChecked(true);

    connect(m_btnLive, &QPushButton::clicked, this, [this]() {
        m_btnRecent->setChecked(false);
        m_btnUpcoming->setChecked(false);
        loadMatches("live");
    });
    connect(m_btnRecent, &QPushButton::clicked, this, [this]() {
        m_btnLive->setChecked(false);
        m_btnUpcoming->setChecked(false);
        loadMatches("recent");
    });
    connect(m_btnUpcoming, &QPushButton::clicked, this, [this]() {
        m_btnLive->setChecked(false);
        m_btnRecent->setChecked(false);
        loadMatches("upcoming");
    });

    m_mainLayout->addLayout(nav);

    // ------------------ SCROLL AREA (MATCH CARDS) ------------------
    m_container->setLayout(m_cardsLayout);
    m_cardsLayout->setAlignment(Qt::AlignTop);
    m_cardsLayout->setSpacing(16);
    m_cardsLayout->setContentsMargins(20,20,20,20);

    m_scroll->setWidgetResizable(true);
    m_scroll->setWidget(m_container);

    m_mainLayout->addWidget(m_loadingLabel);
    m_mainLayout->addWidget(m_scroll, 1);

    // API KEYS (SAFE)
    m_apiHost = "cricbuzz-cricket.p.rapidapi.com";
    m_apiKey  = "a3704a727cmshbfb37f29a117ed5p15d7d3jsn34a6d602b3e3";
}

QWidget* Dashboard::createHeader()
{
    auto *h = new QWidget;
    h->setFixedHeight(70);
    h->setStyleSheet("background:#09B83E;");

    auto *l = new QHBoxLayout(h);
    l->setContentsMargins(20,0,20,0);

    QLabel *title = new QLabel("Cricbuzz Live Scores");
    title->setStyleSheet("color:white; font-size:22px; font-weight:bold;");

    l->addWidget(title);
    l->addStretch();

    // REFRESH BUTTON
    QPushButton *refresh = new QPushButton("Refresh");
    refresh->setStyleSheet(
        "background:white; color:#09B83E; border-radius:15px; padding:6px 20px;"
        );
    connect(refresh, &QPushButton::clicked, this, [this]() {
        loadMatches(m_currentRequestType);
    });

    l->addWidget(refresh);

    return h;
}

void Dashboard::loadMatches(const QString &type)
{
    m_currentRequestType = type;

    m_loadingLabel->setVisible(true);
    m_loadingLabel->setText("Loading...");

    clearCards();

    QString url;

    if (type == "live")
        url = "https://cricbuzz-cricket.p.rapidapi.com/matches/v1/live";
    else if (type == "recent")
        url = "https://cricbuzz-cricket.p.rapidapi.com/matches/v1/recent";
    else
        url = "https://cricbuzz-cricket.p.rapidapi.com/matches/v1/upcoming";

    fetchMatchData(url);
}

void Dashboard::fetchMatchData(const QString &url)
{
    QNetworkRequest req((QUrl(url)));

    req.setRawHeader("x-rapidapi-host", m_apiHost.toUtf8());
    req.setRawHeader("x-rapidapi-key", m_apiKey.toUtf8());
    req.setHeader(QNetworkRequest::UserAgentHeader, "CricbuzzQt/1.0");

    m_networkManager->get(req);
}

void Dashboard::onNetworkFinished(QNetworkReply *reply)
{
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        m_loadingLabel->setText("Network error!");
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    bool hasMatch = false;

    if (doc.isObject())
        hasMatch = parseAndAddMatches(doc.object());

    finishLoading(hasMatch);

    reply->deleteLater();
}

bool Dashboard::parseAndAddMatches(const QJsonObject &root)
{
    bool found = false;

    QJsonArray typeMatches = root.value("typeMatches").toArray();

    for (const QJsonValue &tmVal : typeMatches)
    {
        QJsonObject tmObj = tmVal.toObject();
        QJsonArray seriesMatches = tmObj.value("seriesMatches").toArray();

        for (const QJsonValue &serVal : seriesMatches)
        {
            QJsonObject wrapperObj = serVal.toObject().value("seriesAdWrapper").toObject();
            QJsonArray matches = wrapperObj.value("matches").toArray();

            for (const QJsonValue &matchVal : matches)
            {
                QJsonObject match = matchVal.toObject();
                QJsonObject info = match.value("matchInfo").toObject();
                QJsonObject score = match.value("matchScore").toObject();

                if (info.isEmpty()) continue;

                MatchInfo mi;
                mi.id     = QString::number(info.value("matchId").toInt());
                mi.title  = info.value("seriesName").toString();
                mi.team1  = info.value("team1").toObject().value("teamName").toString();
                mi.team2  = info.value("team2").toObject().value("teamName").toString();
                mi.status = info.value("status").toString();

                // team1 score
                if (score.contains("team1Score")) {
                    QJsonObject inn = score["team1Score"].toObject().value("inngs1").toObject();
                    mi.team1Score = QString("%1/%2 (%3 ov)")
                                        .arg(inn["runs"].toInt())
                                        .arg(inn["wickets"].toInt())
                                        .arg(inn["overs"].toDouble());
                }

                // team2 score
                if (score.contains("team2Score")) {
                    QJsonObject inn = score["team2Score"].toObject().value("inngs1").toObject();
                    mi.team2Score = QString("%1/%2 (%3 ov)")
                                        .arg(inn["runs"].toInt())
                                        .arg(inn["wickets"].toInt())
                                        .arg(inn["overs"].toDouble());
                }

                addMatchCard(mi);
                found = true;
=======
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>
#include <random>

// MatchCard Implementation
MatchCard::MatchCard(const MatchInfo &matchInfo, QWidget *parent)
    : QFrame(parent)
    , m_matchInfo(matchInfo)
    , m_blinkTimer(new QTimer(this))
{
    setupUI();
    applyStyles();
    updateMatchData(matchInfo);

    // Setup live indicator blinking if match is live
    if (m_matchInfo.isLive) {
        connect(m_blinkTimer, &QTimer::timeout, this, &MatchCard::updateLiveIndicator);
        m_blinkTimer->start(1000); // Blink every second
    }

    setFixedHeight(160);
    setCursor(Qt::PointingHandCursor);
}

void MatchCard::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 12, 15, 12);
    m_mainLayout->setSpacing(8);

    // Header with series, match type and live indicator
    m_headerLayout = new QHBoxLayout();
    m_seriesLabel = new QLabel();
    m_matchTypeLabel = new QLabel();
    m_liveIndicator = new QLabel("🔴 LIVE");

    m_headerLayout->addWidget(m_seriesLabel);
    m_headerLayout->addWidget(m_matchTypeLabel);
    m_headerLayout->addStretch();
    m_headerLayout->addWidget(m_liveIndicator);

    // Score section
    m_scoreLayout = new QHBoxLayout();
    m_scoreLayout->setSpacing(15);

    // Team 1
    QVBoxLayout *team1Layout = new QVBoxLayout();
    m_team1Label = new QLabel();
    m_team1Score = new QLabel();
    team1Layout->addWidget(m_team1Label);
    team1Layout->addWidget(m_team1Score);
    team1Layout->setAlignment(Qt::AlignLeft);

    // VS
    m_vsLabel = new QLabel("vs");
    m_vsLabel->setAlignment(Qt::AlignCenter);

    // Team 2
    QVBoxLayout *team2Layout = new QVBoxLayout();
    m_team2Label = new QLabel();
    m_team2Score = new QLabel();
    team2Layout->addWidget(m_team2Label);
    team2Layout->addWidget(m_team2Score);
    team2Layout->setAlignment(Qt::AlignRight);

    m_scoreLayout->addLayout(team1Layout, 2);
    m_scoreLayout->addWidget(m_vsLabel, 0);
    m_scoreLayout->addLayout(team2Layout, 2);

    // Status and venue
    m_statusLabel = new QLabel();
    m_venueLabel = new QLabel();

    // Bottom section with over info and buttons
    m_bottomLayout = new QHBoxLayout();
    m_overLabel = new QLabel();
    m_runRateLabel = new QLabel();

    m_scorecardBtn = new QPushButton("Scorecard");
    m_statsBtn = new QPushButton("Stats");

    m_bottomLayout->addWidget(m_overLabel);
    m_bottomLayout->addWidget(m_runRateLabel);
    m_bottomLayout->addStretch();
    m_bottomLayout->addWidget(m_scorecardBtn);
    m_bottomLayout->addWidget(m_statsBtn);

    // Add all layouts
    m_mainLayout->addLayout(m_headerLayout);
    m_mainLayout->addLayout(m_scoreLayout);
    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_venueLabel);
    m_mainLayout->addLayout(m_bottomLayout);

    // Connect signals
    connect(m_scorecardBtn, &QPushButton::clicked, this, &MatchCard::onScorecardClicked);
    connect(m_statsBtn, &QPushButton::clicked, this, &MatchCard::onStatsClicked);
}

void MatchCard::applyStyles()
{
    // Main card style - Cricbuzz-like design
    setStyleSheet(
        "MatchCard { "
        "background-color: #ffffff; "
        "border: 1px solid #e8e8e8; "
        "border-radius: 12px; "
        "margin: 8px 4px; "
        "} "
        "MatchCard:hover { "
        "border: 2px solid #09c372; "
        "background-color: #fafafa; "
        "box-shadow: 0px 6px 16px rgba(9, 195, 114, 0.15); "
        "}"
        );

    // Series label - Cricbuzz green theme
    m_seriesLabel->setStyleSheet("color: #09c372; font-size: 13px; font-weight: bold; background: transparent;");

    // Match type label
    m_matchTypeLabel->setStyleSheet(
        "color: #ffffff; font-size: 11px; font-weight: bold; "
        "background-color: #ff6b35; padding: 2px 8px; border-radius: 10px; margin-left: 8px;"
        );

    // Live indicator
    m_liveIndicator->setStyleSheet("color: #ff4444; font-size: 13px; font-weight: bold; background: transparent;");

    // Team labels
    m_team1Label->setStyleSheet("color: #222222; font-size: 16px; font-weight: bold; background: transparent;");
    m_team2Label->setStyleSheet("color: #222222; font-size: 16px; font-weight: bold; background: transparent;");

    // Score labels
    m_team1Score->setStyleSheet("color: #09c372; font-size: 15px; font-weight: 600; background: transparent;");
    m_team2Score->setStyleSheet("color: #09c372; font-size: 15px; font-weight: 600; background: transparent;");

    // VS label
    m_vsLabel->setStyleSheet("color: #888888; font-size: 13px; font-weight: bold; margin: 0px 15px; background: transparent;");

    // Status and venue
    m_statusLabel->setStyleSheet("color: #09c372; font-size: 14px; font-weight: 600; background: transparent;");
    m_venueLabel->setStyleSheet("color: #666666; font-size: 12px; background: transparent;");

    // Over and run rate
    m_overLabel->setStyleSheet("color: #666666; font-size: 12px; background: transparent;");
    m_runRateLabel->setStyleSheet("color: #666666; font-size: 12px; margin-left: 15px; background: transparent;");

    // Buttons - Cricbuzz style
    QString buttonStyle =
        "QPushButton { "
        "background-color: #09c372; "
        "color: white; "
        "border: none; "
        "padding: 6px 16px; "
        "border-radius: 6px; "
        "font-size: 12px; "
        "font-weight: bold; "
        "margin-left: 6px; "
        "} "
        "QPushButton:hover { "
        "background-color: #07a862; "
        "} "
        "QPushButton:pressed { "
        "background-color: #058a52; "
        "}";

    m_scorecardBtn->setStyleSheet(buttonStyle);
    m_statsBtn->setStyleSheet(buttonStyle);
}

void MatchCard::updateMatchData(const MatchInfo &matchInfo)
{
    m_matchInfo = matchInfo;

    m_seriesLabel->setText(matchInfo.series);
    m_matchTypeLabel->setText(matchInfo.matchType);
    m_team1Label->setText(matchInfo.team1);
    m_team2Label->setText(matchInfo.team2);
    m_team1Score->setText(matchInfo.team1Score);
    m_team2Score->setText(matchInfo.team2Score);
    m_statusLabel->setText(matchInfo.status);
    m_venueLabel->setText("📍 " + matchInfo.venue);

    if (!matchInfo.currentOver.isEmpty()) {
        m_overLabel->setText("⏱ Overs: " + matchInfo.currentOver);
    }

    if (!matchInfo.runRate.isEmpty()) {
        m_runRateLabel->setText("📊 RR: " + matchInfo.runRate);
    }

    m_liveIndicator->setVisible(matchInfo.isLive);
    m_matchTypeLabel->setVisible(!matchInfo.matchType.isEmpty());

    if (matchInfo.isLive && !m_blinkTimer->isActive()) {
        m_blinkTimer->start(1000);
    } else if (!matchInfo.isLive && m_blinkTimer->isActive()) {
        m_blinkTimer->stop();
        m_liveIndicator->setVisible(false);
    }
}

void MatchCard::updateLiveIndicator()
{
    static bool visible = true;
    if (m_matchInfo.isLive) {
        m_liveIndicator->setVisible(visible);
        visible = !visible;
    }
}

void MatchCard::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit matchClicked(m_matchInfo.matchId);
}

void MatchCard::onScorecardClicked()
{
    emit scorecardRequested(m_matchInfo.matchId);
}

void MatchCard::onStatsClicked()
{
    emit statsRequested(m_matchInfo.matchId);
}

// Dashboard Implementation
Dashboard::Dashboard(QWidget *parent)
    : QMainWindow(parent)
    , m_refreshTimer(new QTimer(this))
    , m_randomEngine(std::random_device{}())
{
    setupUI();
    applyStyles();
    loadSampleMatches();

    // Auto refresh every 15 seconds for live matches
    connect(m_refreshTimer, &QTimer::timeout, this, &Dashboard::updateLiveScores);
    m_refreshTimer->start(15000); // 15 seconds

    setWindowTitle("Cricket Dashboard - Live Scores & Updates");
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1400, 900);

    // Center the window
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

Dashboard::~Dashboard()
{
}

void Dashboard::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setupHeader();
    setupTabs();
}

void Dashboard::setupHeader()
{
    m_headerWidget = new QWidget();
    m_headerWidget->setFixedHeight(70);
    m_headerLayout = new QHBoxLayout(m_headerWidget);
    m_headerLayout->setContentsMargins(25, 15, 25, 15);

    // Logo and title
    m_logoLabel = new QLabel("🏏");
    m_logoLabel->setStyleSheet("font-size: 28px;");

    m_titleLabel = new QLabel("CricketLive");
    m_titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #ffffff; "
        "margin-left: 12px; font-family: 'Segoe UI', Arial;"
        );

    // Refresh button
    m_refreshButton = new QPushButton("🔄 Refresh Scores");

    // Profile button
    m_profileButton = new QPushButton("👤 Profile");

    // Window controls
    QWidget *windowControls = createWindowControls();

    createProfileMenu();

    m_headerLayout->addWidget(m_logoLabel);
    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addStretch();
    m_headerLayout->addWidget(m_refreshButton);
    m_headerLayout->addWidget(m_profileButton);
    m_headerLayout->addWidget(windowControls);

    m_mainLayout->addWidget(m_headerWidget);

    // Connect signals
    connect(m_refreshButton, &QPushButton::clicked, this, &Dashboard::onRefreshClicked);
    connect(m_profileButton, &QPushButton::clicked, this, &Dashboard::onProfileMenuTriggered);
}

QWidget *Dashboard::createWindowControls()
{
    QWidget *controlWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(controlWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    m_minimizeButton = new QPushButton("—");
    m_minimizeButton->setFixedSize(30, 30);
    m_minimizeButton->setStyleSheet(
        "QPushButton { "
        "background-color: transparent; "
        "border: none; "
        "border-radius: 4px; "
        "color: #ffffff; "
        "font-size: 16px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "background-color: rgba(255, 255, 255, 0.2); "
        "}"
        );

    m_closeButton = new QPushButton("✕");
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setStyleSheet(
        "QPushButton { "
        "background-color: transparent; "
        "border: none; "
        "border-radius: 4px; "
        "color: #ffffff; "
        "font-size: 14px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "background-color: #ff4444; "
        "}"
        );

    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_closeButton);

    connect(m_minimizeButton, &QPushButton::clicked, this, &Dashboard::onMinimizeClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &Dashboard::onCloseClicked);

    return controlWidget;
}

void Dashboard::setupTabs()
{
    m_tabWidget = new QTabWidget();

    setupMatchesTab();
    setupLiveTab();
    setupSeriesTab();

    m_tabWidget->addTab(m_matchesTab, "🏏 All Matches");
    m_tabWidget->addTab(m_liveTab, "🔴 Live Matches");
    m_tabWidget->addTab(m_seriesTab, "🏆 Series");

    m_mainLayout->addWidget(m_tabWidget);

    connect(m_tabWidget, &QTabWidget::currentChanged, this, &Dashboard::onTabChanged);
}

void Dashboard::setupMatchesTab()
{
    m_matchesTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_matchesTab);
    layout->setContentsMargins(15, 15, 15, 15);

    m_matchesScrollArea = new QScrollArea();
    m_matchesContainer = new QWidget();
    m_matchesLayout = new QVBoxLayout(m_matchesContainer);
    m_matchesLayout->setSpacing(12);
    m_matchesLayout->addStretch();

    m_matchesScrollArea->setWidget(m_matchesContainer);
    m_matchesScrollArea->setWidgetResizable(true);
    m_matchesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(m_matchesScrollArea);
}

void Dashboard::setupLiveTab()
{
    m_liveTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_liveTab);
    layout->setContentsMargins(15, 15, 15, 15);

    m_liveScrollArea = new QScrollArea();
    m_liveContainer = new QWidget();
    m_liveLayout = new QVBoxLayout(m_liveContainer);
    m_liveLayout->setSpacing(12);
    m_liveLayout->addStretch();

    m_liveScrollArea->setWidget(m_liveContainer);
    m_liveScrollArea->setWidgetResizable(true);
    m_liveScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(m_liveScrollArea);
}

void Dashboard::setupSeriesTab()
{
    m_seriesTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_seriesTab);
    layout->setContentsMargins(15, 15, 15, 15);

    m_seriesScrollArea = new QScrollArea();
    m_seriesContainer = new QWidget();
    m_seriesLayout = new QVBoxLayout(m_seriesContainer);
    m_seriesLayout->setSpacing(12);
    m_seriesLayout->addStretch();

    m_seriesScrollArea->setWidget(m_seriesContainer);
    m_seriesScrollArea->setWidgetResizable(true);
    m_seriesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(m_seriesScrollArea);
}

void Dashboard::applyStyles()
{
    // Main window background
    setStyleSheet("QMainWindow { background-color: #f8f9fa; }");

    // Header - Cricbuzz green gradient
    m_headerWidget->setStyleSheet(
        "QWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #09c372, stop:1 #07a862); "
        "border-bottom: 3px solid #058a52; "
        "}"
        );

    // Buttons in header
    QString headerButtonStyle =
        "QPushButton { "
        "background-color: rgba(255, 255, 255, 0.15); "
        "color: white; "
        "border: 1px solid rgba(255, 255, 255, 0.3); "
        "padding: 10px 20px; "
        "border-radius: 6px; "
        "font-size: 14px; "
        "font-weight: 600; "
        "margin-left: 8px; "
        "} "
        "QPushButton:hover { "
        "background-color: rgba(255, 255, 255, 0.25); "
        "border: 1px solid rgba(255, 255, 255, 0.5); "
        "} "
        "QPushButton:pressed { "
        "background-color: rgba(255, 255, 255, 0.1); "
        "}";

    m_refreshButton->setStyleSheet(headerButtonStyle);
    m_profileButton->setStyleSheet(headerButtonStyle);

    // Tabs - Cricbuzz style
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { "
        "border: none; "
        "background-color: #f8f9fa; "
        "} "
        "QTabBar::tab { "
        "background-color: #ffffff; "
        "color: #666666; "
        "padding: 12px 24px; "
        "margin-right: 4px; "
        "border-top-left-radius: 8px; "
        "border-top-right-radius: 8px; "
        "border: 1px solid #e8e8e8; "
        "border-bottom: none; "
        "font-size: 14px; "
        "font-weight: 600; "
        "} "
        "QTabBar::tab:selected { "
        "background-color: #09c372; "
        "color: #ffffff; "
        "border: 1px solid #09c372; "
        "} "
        "QTabBar::tab:hover:!selected { "
        "background-color: #f0f8f4; "
        "color: #09c372; "
        "}"
        );

    // Scroll areas
    QString scrollStyle =
        "QScrollArea { "
        "border: none; "
        "background-color: transparent; "
        "} "
        "QScrollBar:vertical { "
        "background-color: #f0f0f0; "
        "width: 14px; "
        "border-radius: 7px; "
        "margin: 2px; "
        "} "
        "QScrollBar::handle:vertical { "
        "background-color: #09c372; "
        "border-radius: 7px; "
        "min-height: 30px; "
        "margin: 2px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "background-color: #07a862; "
        "} "
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "background: none; "
        "border: none; "
        "}";

    m_matchesScrollArea->setStyleSheet(scrollStyle);
    m_liveScrollArea->setStyleSheet(scrollStyle);
    m_seriesScrollArea->setStyleSheet(scrollStyle);
}

void Dashboard::createProfileMenu()
{
    m_profileMenu = new QMenu(this);
    m_profileMenu->setStyleSheet(
        "QMenu { "
        "background-color: #ffffff; "
        "border: 2px solid #09c372; "
        "border-radius: 8px; "
        "padding: 8px; "
        "} "
        "QMenu::item { "
        "padding: 8px 16px; "
        "border-radius: 4px; "
        "} "
        "QMenu::item:selected { "
        "background-color: #f0f8f4; "
        "color: #09c372; "
        "}"
        );

    QAction *profileAction = new QAction("👤 View Profile", this);
    QAction *settingsAction = new QAction("⚙️ Settings", this);
    QAction *notificationsAction = new QAction("🔔 Notifications", this);
    m_logoutAction = new QAction("🚪 Logout", this);

    m_profileMenu->addAction(profileAction);
    m_profileMenu->addAction(settingsAction);
    m_profileMenu->addAction(notificationsAction);
    m_profileMenu->addSeparator();
    m_profileMenu->addAction(m_logoutAction);

    connect(m_logoutAction, &QAction::triggered, this, &Dashboard::onLogoutClicked);
}

void Dashboard::loadSampleMatches()
{
    QList<MatchInfo> sampleMatches;

    // Live match 1 - India vs Pakistan
    MatchInfo match1;
    match1.matchId = "match1";
    match1.team1 = "India";
    match1.team2 = "Pakistan";
    match1.team1Score = "287/6 (48.3)";
    match1.team2Score = "Yet to bat";
    match1.status = "India Innings - In Progress";
    match1.venue = "Dubai International Cricket Stadium";
    match1.series = "Asia Cup 2024";
    match1.isLive = true;
    match1.currentOver = "48.3";
    match1.runRate = "5.92";
    match1.matchType = "ODI";
    sampleMatches.append(match1);

    // Live match 2 - Australia vs England
    MatchInfo match2;
    match2.matchId = "match2";
    match2.team1 = "Australia";
    match2.team2 = "England";
    match2.team1Score = "342/8 (50)";
    match2.team2Score = "215/7 (35.2)";
    match2.status = "England need 128 runs in 88 balls";
    match2.venue = "Lord's Cricket Ground, London";
    match2.series = "ODI Series 2024";
    match2.isLive = true;
    match2.currentOver = "35.2";
    match2.runRate = "6.08";
    match2.requiredRunRate = "8.73";
    match2.matchType = "ODI";
    sampleMatches.append(match2);

    // Live match 3 - T20I
    MatchInfo match3;
    match3.matchId = "match3";
    match3.team1 = "South Africa";
    match3.team2 = "New Zealand";
    match3.team1Score = "185/4 (20)";
    match3.team2Score = "142/6 (16.4)";
    match3.status = "New Zealand need 44 runs in 20 balls";
    match3.venue = "Wanderers Stadium, Johannesburg";
    match3.series = "T20I Series 2024";
    match3.isLive = true;
    match3.currentOver = "16.4";
    match3.runRate = "8.51";
    match3.requiredRunRate = "13.20";
    match3.matchType = "T20I";
    sampleMatches.append(match3);

    // Completed match
    MatchInfo match4;
    match4.matchId = "match4";
    match4.team1 = "West Indies";
    match4.team2 = "Bangladesh";
    match4.team1Score = "298/7 (50)";
    match4.team2Score = "245 (46.3)";
    match4.status = "West Indies won by 53 runs";
    match4.venue = "Kensington Oval, Barbados";
    match4.series = "ODI Series 2024";
    match4.isLive = false;
    match4.matchType = "ODI";
    sampleMatches.append(match4);

    // Upcoming match
    MatchInfo match5;
    match5.matchId = "match5";
    match5.team1 = "Sri Lanka";
    match5.team2 = "Afghanistan";
    match5.team1Score = "";
    match5.team2Score = "";
    match5.status = "Today, 2:30 PM IST";
    match5.venue = "R.Premadasa Stadium, Colombo";
    match5.series = "Asia Cup 2024";
    match5.isLive = false;
    match5.matchType = "T20I";
    sampleMatches.append(match5);

    // Test match
    MatchInfo match6;
    match6.matchId = "match6";
    match6.team1 = "India";
    match6.team2 = "Australia";
    match6.team1Score = "425 & 234/4 (68)";
    match6.team2Score = "337";
    match6.status = "India lead by 322 runs";
    match6.venue = "Melbourne Cricket Ground";
    match6.series = "Border-Gavaskar Trophy 2024";
    match6.isLive = true;
    match6.currentOver = "68.0";
    match6.runRate = "3.44";
    match6.matchType = "Test";
    sampleMatches.append(match6);

    m_matches = sampleMatches;
    refreshMatches();
}

void Dashboard::addMatchCard(const MatchInfo &matchInfo, QWidget *container)
{
    MatchCard *card = new MatchCard(matchInfo, container);
    m_matchCards.append(card);

    // Connect signals
    connect(card, &MatchCard::matchClicked, this, &Dashboard::onMatchCardClicked);
    connect(card, &MatchCard::scorecardRequested, this, &Dashboard::onScorecardRequested);
    connect(card, &MatchCard::statsRequested, this, &Dashboard::onStatsRequested);

    // Add to appropriate layout
    if (container == m_matchesContainer) {
        m_matchesLayout->insertWidget(m_matchesLayout->count() - 1, card);
    } else if (container == m_liveContainer) {
        m_liveLayout->insertWidget(m_liveLayout->count() - 1, card);
    } else if (container == m_seriesContainer) {
        m_seriesLayout->insertWidget(m_seriesLayout->count() - 1, card);
    }
}

void Dashboard::setUserInfo(const QString &userId, const QString &email)
{
    m_userId = userId;
    m_userEmail = email;

    // Update profile button text
    QString displayName = email.split("@").first();
    if (displayName.length() > 12) {
        displayName = displayName.left(10) + "..";
    }
    m_profileButton->setText(QString("👤 %1").arg(displayName));
}

void Dashboard::refreshMatches()
{
    // Clear existing cards
    qDeleteAll(m_matchCards);
    m_matchCards.clear();

    // Clear layouts
    QLayoutItem *item;
    while ((item = m_matchesLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    while ((item = m_liveLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    while ((item = m_seriesLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Add stretch at the end
    m_matchesLayout->addStretch();
    m_liveLayout->addStretch();
    m_seriesLayout->addStretch();

    // Add matches to tabs
    for (const MatchInfo &match : m_matches) {
        // Add to all matches
        addMatchCard(match, m_matchesContainer);

        // Add to live matches if it's live
        if (match.isLive) {
            addMatchCard(match, m_liveContainer);
        }

        // Add to series tab (group by series)
        addMatchCard(match, m_seriesContainer);
    }
}

void Dashboard::updateLiveScores()
{
    qDebug() << "Updating live scores...";

    std::uniform_int_distribution<int> runDist(0, 6); // 0-6 runs
    std::uniform_int_distribution<int> eventDist(1, 10); // 1-10 for different events

    // Update sample data (simulate score updates)
    for (int i = 0; i < m_matches.size(); ++i) {
        if (!m_matches[i].isLive) continue;

        if (m_matches[i].matchId == "match1") {
            // Simulate ODI score update
            static int runs = 287, wickets = 6, balls = 291; // 48.3 overs

            int event = eventDist(m_randomEngine);
            if (event <= 6) {
                runs += runDist(m_randomEngine);
                balls++;
            } else if (event <= 8 && wickets < 10) {
                wickets++;
                balls++;
            } else {
                balls++;
            }

            if (balls >= 300) { // 50 overs completed
                m_matches[i].isLive = false;
                m_matches[i].status = "India: " + QString::number(runs) + "/" + QString::number(wickets) + " (50)";
            } else {
                int overs = balls / 6;
                int ballsInOver = balls % 6;
                m_matches[i].team1Score = QString("%1/%2 (%3.%4)").arg(runs).arg(wickets).arg(overs).arg(ballsInOver);
                m_matches[i].currentOver = QString("%1.%2").arg(overs).arg(ballsInOver);
                m_matches[i].runRate = QString::number((double)runs / (balls / 6.0), 'f', 2);
            }
        }
        else if (m_matches[i].matchId == "match2") {
            // Simulate chasing team update
            static int runs = 215, wickets = 7, balls = 212; // 35.2 overs

            int event = eventDist(m_randomEngine);
            if (event <= 6) {
                runs += runDist(m_randomEngine);
                balls++;
            } else if (event <= 8 && wickets < 10) {
                wickets++;
                balls++;
            } else {
                balls++;
            }

            int required = 342 - runs;
            int ballsLeft = 300 - balls;

            if (required <= 0) {
                m_matches[i].isLive = false;
                m_matches[i].status = "England won by " + QString::number(10 - wickets) + " wickets";
            } else if (balls >= 300 || wickets >= 10) {
                m_matches[i].isLive = false;
                m_matches[i].status = "Australia won by " + QString::number(required) + " runs";
            } else {
                int overs = balls / 6;
                int ballsInOver = balls % 6;
                m_matches[i].team2Score = QString("%1/%2 (%3.%4)").arg(runs).arg(wickets).arg(overs).arg(ballsInOver);
                m_matches[i].currentOver = QString("%1.%2").arg(overs).arg(ballsInOver);
                m_matches[i].runRate = QString::number((double)runs / (balls / 6.0), 'f', 2);
                m_matches[i].requiredRunRate = QString::number((double)required / (ballsLeft / 6.0), 'f', 2);
                m_matches[i].status = QString("England need %1 runs in %2 balls").arg(required).arg(ballsLeft);
>>>>>>> a4ce4ea0f5a1005722be20c3afda11ecdfaa26df
            }
        }
    }

<<<<<<< HEAD
    return found;
}

void Dashboard::addMatchCard(const MatchInfo &info)
{
    auto *card = new MatchCard(info, m_container);
    m_cards.append(card);
    m_cardsLayout->addWidget(card);

    connect(card, &MatchCard::scorecardClicked, this, [this](const QString &id) {
        emit scorecardRequested(id);
    });
}

void Dashboard::finishLoading(bool hasMatch)
{
    if (!hasMatch)
        m_loadingLabel->setText("No matches available.");
    else
        m_loadingLabel->setVisible(false);

    m_cardsLayout->addStretch();
}

void Dashboard::clearCards()
{
    for (auto *c : m_cards)
        delete c;

    m_cards.clear();

    while (QLayoutItem *it = m_cardsLayout->takeAt(0)) {
        if (it->widget()) delete it->widget();
        delete it;
    }
}

Dashboard::~Dashboard() {}
=======
    // Update all match cards
    for (MatchCard *card : m_matchCards) {
        for (const MatchInfo &match : m_matches) {
            if (card->getMatchInfo().matchId == match.matchId) {
                card->updateMatchData(match);
                break;
            }
        }
    }
}

void Dashboard::onRefreshClicked()
{
    m_refreshButton->setEnabled(false);
    m_refreshButton->setText("🔄 Updating...");

    // Simulate API call delay
    QTimer::singleShot(1500, this, [this]() {
        updateLiveScores();
        m_refreshButton->setEnabled(true);
        m_refreshButton->setText("🔄 Refresh Scores");

        // Show a brief success message
        m_refreshButton->setText("✓ Updated");
        QTimer::singleShot(2000, this, [this]() {
            m_refreshButton->setText("🔄 Refresh Scores");
        });
    });
}

void Dashboard::onMatchCardClicked(const QString &matchId)
{
    qDebug() << "Match clicked:" << matchId;
    emit matchSelected(matchId);

    // Find the match info
    QString matchDetails = "Match Details not found";
    for (const MatchInfo &match : m_matches) {
        if (match.matchId == matchId) {
            matchDetails = QString(
                               "🏏 %1 vs %2\n\n"
                               "📊 Scores:\n"
                               "%3: %4\n"
                               "%5: %6\n\n"
                               "📍 Venue: %7\n"
                               "🏆 Series: %8\n"
                               "⚡ Status: %9\n\n"
                               "%10"
                               ).arg(match.team1, match.team2, match.team1, match.team1Score,
                                    match.team2, match.team2Score, match.venue, match.series, match.status,
                                    match.isLive ? "🔴 LIVE - Updates every 15 seconds" : "📋 Match Completed");
            break;
        }
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Match Center");
    msgBox.setText(matchDetails);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton("📊 View Scorecard", QMessageBox::ActionRole);
    msgBox.addButton("📈 Player Stats", QMessageBox::ActionRole);
    msgBox.addButton("📰 Commentary", QMessageBox::ActionRole);
    msgBox.addButton("Close", QMessageBox::RejectRole);
    msgBox.exec();
}

void Dashboard::onScorecardRequested(const QString &matchId)
{
    qDebug() << "Scorecard requested for:" << matchId;
    emit scorecardRequested(matchId);

    QString scorecardInfo = QString(
                                "📊 DETAILED SCORECARD\n\n"
                                "Match ID: %1\n\n"
                                "🏏 Batting Details:\n"
                                "• Player-wise runs, balls, 4s, 6s\n"
                                "• Partnerships and fall of wickets\n"
                                "• Strike rates and run rates\n\n"
                                "⚾ Bowling Figures:\n"
                                "• Overs, maidens, runs, wickets\n"
                                "• Economy rates\n"
                                "• Bowling analysis\n\n"
                                "📈 Match Summary:\n"
                                "• Team totals and extras\n"
                                "• Key performance stats\n"
                                "• Match progression charts"
                                ).arg(matchId);

    QMessageBox::information(this, "Cricket Scorecard", scorecardInfo);
}

void Dashboard::onStatsRequested(const QString &matchId)
{
    qDebug() << "Stats requested for:" << matchId;
    emit statsRequested(matchId);

    QString statsInfo = QString(
                            "📈 PLAYER STATISTICS\n\n"
                            "Match ID: %1\n\n"
                            "🏆 Top Performers:\n"
                            "• Highest run scorer\n"
                            "• Best bowling figures\n"
                            "• Most valuable player\n\n"
                            "📊 Team Stats:\n"
                            "• Run rate comparison\n"
                            "• Boundary analysis\n"
                            "• Powerplay performance\n\n"
                            "🎯 Head-to-Head Records:\n"
                            "• Historical performance\n"
                            "• Recent form guide\n"
                            "• Player vs player stats"
                            ).arg(matchId);

    QMessageBox::information(this, "Match Statistics", statsInfo);
}

void Dashboard::onTabChanged(int index)
{
    Q_UNUSED(index)
    // Refresh data when tab changes
    updateLiveScores();
}

void Dashboard::onProfileMenuTriggered()
{
    QPoint buttonPos = m_profileButton->mapToGlobal(QPoint(0, m_profileButton->height()));
    m_profileMenu->exec(buttonPos);
}

void Dashboard::onLogoutClicked()
{
    int ret = QMessageBox::question(this, "Logout Confirmation",
                                    QString("Are you sure you want to logout?\n\n"
                                            "👤 Logged in as: %1\n"
                                            "⏰ Session active since login").arg(m_userEmail),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        emit logoutRequested();
    }
}

void Dashboard::onMinimizeClicked()
{
    showMinimized();
}

void Dashboard::onCloseClicked()
{
    close();
}

// Window dragging functionality
void Dashboard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Check if click is in the header area (for dragging)
        if (event->position().y() <= m_headerWidget->height()) {
            // Don't start dragging if clicked on buttons
            QWidget *clickedWidget = childAt(event->position().toPoint());
            if (qobject_cast<QPushButton*>(clickedWidget)) {
                return;
            }

            m_dragging = true;
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

void Dashboard::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void Dashboard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}
>>>>>>> a4ce4ea0f5a1005722be20c3afda11ecdfaa26df
