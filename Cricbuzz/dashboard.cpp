#include "dashboard.h"
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
            }
        }
    }

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
