#include "recent.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

Recent::Recent(QWidget *parent)
    : QWidget(parent),
    m_layout(new QVBoxLayout(this)),
    m_scroll(new QScrollArea),
    m_container(new QWidget),
    m_cardsLayout(new QVBoxLayout(m_container)),
    m_statusLabel(new QLabel("Loading recent matches...")),
    m_manager(new QNetworkAccessManager(this)),
    m_timer(new QTimer(this))
{
    setStyleSheet("background:#f8f9fa;");

    // Header
    auto *header = new QWidget;
    header->setFixedHeight(65);
    header->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #09B83E,stop:1 #078C32);");

    auto *hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(20,0,20,0);

    auto *title = new QLabel("Recent Matches");
    title->setStyleSheet("color:white; font-size:20px; font-weight:bold;");
    auto *icon = new QLabel("Recent");
    icon->setStyleSheet("font-size:26px; padding-right:8px;");

    auto *left = new QHBoxLayout;
    left->addWidget(icon);
    left->addWidget(title);
    hLayout->addLayout(left);
    hLayout->addStretch();

    auto *refresh = new QPushButton("Refresh");
    refresh->setFixedHeight(38);
    refresh->setStyleSheet(
        "QPushButton { background:rgba(255,255,255,0.25); color:white; border-radius:19px; padding:0 24px; font-weight:600; }"
        "QPushButton:hover { background:rgba(255,255,255,0.35); }"
        );
    connect(refresh, &QPushButton::clicked, this, &Recent::fetchRecent);
    hLayout->addWidget(refresh);

    m_layout->addWidget(header);

    // Scroll area
    m_scroll->setWidgetResizable(true);
    m_scroll->setWidget(m_container);
    m_scroll->setStyleSheet("border:none; background:transparent;");

    m_cardsLayout->setAlignment(Qt::AlignTop);
    m_cardsLayout->setSpacing(12);
    m_cardsLayout->setContentsMargins(16,16,16,16);

    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size:16px; color:#666; padding:30px;");
    m_layout->addWidget(m_statusLabel);
    m_layout->addWidget(m_scroll, 1);

    // Auto refresh every 30 seconds
    connect(m_timer, &QTimer::timeout, this, &Recent::fetchRecent);
    m_timer->start(30000);

    fetchRecent(); // First load
}

void Recent::fetchRecent()
{
    m_statusLabel->setText("Loading recent matches...");
    m_statusLabel->setVisible(true);
    clearCards();

    QUrl url("https://www.cricbuzz.com/api/cricket-scorecard/fixtures");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader,
                  "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");

    m_manager->get(req);
}

void Recent::onReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_statusLabel->setText("No internet connection");
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        m_statusLabel->setText("Failed to load data");
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray typeMatches = root["typeMatches"].toArray();

    bool foundAny = false;

    for (const QJsonValue &typeVal : typeMatches) {
        QJsonObject typeObj = typeVal.toObject();
        QJsonArray seriesArray = typeObj["seriesMatches"].toArray();

        for (const QJsonValue &seriesVal : seriesArray) {
            QJsonObject series = seriesVal.toObject();
            QJsonArray matches = series["matches"].toArray();

            for (const QJsonValue &matchVal : matches) {
                QJsonObject match = matchVal.toObject();

                // Only show completed matches
                if (!match["matchEnded"].toBool() || !match["matchStarted"].toBool())
                    continue;

                foundAny = true;

                RecentMatchInfo info;
                info.id = match["id"].toString();

                QJsonObject header = match["header"].toObject();
                info.result = header["status"].toString();
                info.dateTime = header["startTime"].toString().left(10); // YYYY-MM-DD
                info.venue = header["venue"].toString();

                QJsonObject t1 = match["team1"].toObject();
                QJsonObject t2 = match["team2"].toObject();

                info.team1 = t1["teamName"].toString("Team 1");
                info.team2 = t2["teamName"].toString("Team 2");

                info.team1Score = t1["score"].toString("—");
                info.team2Score = t2["score"].toString("—");

                addCard(info);
            }
        }
    }

    if (!foundAny) {
        m_statusLabel->setText("No recent matches found.\nCheck back later!");
    } else {
        m_statusLabel->setVisible(false);
    }

    reply->deleteLater();
}

void Recent::clearCards()
{
    qDeleteAll(m_cards);
    m_cards.clear();

    QLayoutItem *item;
    while ((item = m_cardsLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
}

void Recent::addCard(const RecentMatchInfo &info)
{
    QLabel *card = new QLabel;
    card->setWordWrap(true);
    card->setCursor(Qt::PointingHandCursor);
    card->setTextFormat(Qt::RichText);

    QString html = QString(
                       "<div style='padding:16px; background:white; border-radius:12px; "
                       "box-shadow:0 4px 12px rgba(0,0,0,0.1);'>"
                       "<b style='font-size:17px; color:#09B83E;'>%1 vs %2</b><br>"
                       "<span style='color:#333; font-size:15px;'>%3</span><br>"
                       "<span style='color:#d32f2f; font-weight:600;'>%4</span><br>"
                       "<small style='color:#777;'>%5 • %6</small>"
                       "</div>"
                       ).arg(info.team1, info.team2,
                            info.team1Score + " & " + info.team2Score,
                            info.result.toHtmlEscaped(),
                            info.dateTime, info.venue);

    card->setText(html);
    card->setStyleSheet("QLabel { background:white; margin:4px 0; }");

    connect(card, &QLabel::linkActivated, this, [this, info]() {
        emit matchClicked(info.id);
    });

    // Make entire card clickable
    card->setProperty("matchId", info.id);
    card->installEventFilter(this); // Optional: for mouse press

    m_cards.append(card);
    m_cardsLayout->addWidget(card);
}
