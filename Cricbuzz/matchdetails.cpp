#include "matchdetails.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QScrollArea>
#include <QDebug>

MatchDetails::MatchDetails(QWidget *parent)
    : QWidget(parent),
    m_layout(new QVBoxLayout(this)),
    m_title(new QLabel("Match Details")),
    m_scoreLabel(new QLabel),
    m_statusLabel(new QLabel),
    m_venueLabel(new QLabel),
    m_dateLabel(new QLabel),
    m_backBtn(new QPushButton("Back")),
    m_manager(new QNetworkAccessManager(this)),
    m_apiBaseUrl("https://api.cricapi.com/v1"),
    m_apiKey("70474f9c-275b-4a9f-8209-ef11ba9f877c")
{
    setStyleSheet("background:#f8f9fa;");

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("border:none; background:transparent;");

    QWidget *container = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(container);

    m_title->setStyleSheet("font-size:20px; font-weight:700; color:#09B83E; padding:10px;");
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setWordWrap(true);

    m_backBtn->setStyleSheet(
        "QPushButton { padding:10px 24px; background:#09B83E; color:white; border-radius:8px; font-weight:600; }"
        "QPushButton:hover { background:#078C32; }"
        );
    m_backBtn->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(m_backBtn);
    headerLayout->addWidget(m_title, 1);
    headerLayout->addStretch();

    m_scoreLabel->setStyleSheet("font-size:16px; background:white; padding:15px; border-radius:10px; margin:8px;");
    m_scoreLabel->setWordWrap(true);

    m_statusLabel->setStyleSheet("font-size:15px; background:#ffebee; padding:12px; border-radius:8px; margin:8px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    m_venueLabel->setStyleSheet("color:#555; font-size:14px; padding:5px 10px;");
    m_dateLabel->setStyleSheet("color:#555; font-size:14px; padding:5px 10px;");

    contentLayout->addLayout(headerLayout);
    contentLayout->addWidget(m_dateLabel);
    contentLayout->addWidget(m_venueLabel);
    contentLayout->addSpacing(10);
    contentLayout->addWidget(m_scoreLabel);
    contentLayout->addWidget(m_statusLabel);
    contentLayout->addStretch();

    scroll->setWidget(container);
    m_layout->addWidget(scroll);

    connect(m_backBtn, &QPushButton::clicked, this, [this]() {
        emit backClicked();
        hide();
    });

    connect(m_manager, &QNetworkAccessManager::finished, this, &MatchDetails::onNetworkFinished);
}

void MatchDetails::loadMatch(const QString &matchId)
{
    m_currentMatchId = matchId;

    m_title->setText("Loading match details...");
    m_scoreLabel->setText("Fetching scorecard...");
    m_statusLabel->setText("");
    m_venueLabel->setText("");
    m_dateLabel->setText("");

    QString url = QString("%1/match_info?id=%2&apikey=%3")
                      .arg(m_apiBaseUrl, matchId, m_apiKey);

    QNetworkRequest request{ QUrl(url) };  // Fixed: Use {} not ()
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    m_manager->get(request);
}

void MatchDetails::onNetworkFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_title->setText("Network Error");
        m_statusLabel->setText("<b style='color:red;'>Failed: " + reply->errorString() + "</b>");
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        m_title->setText("Invalid Data");
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    QJsonObject dataObj = root["data"].toObject();
    if (dataObj.isEmpty()) {
        m_title->setText("Match Not Found");
        m_statusLabel->setText("No data for this match.");
        reply->deleteLater();
        return;
    }

    // Title
    m_title->setText(dataObj["name"].toString("Unknown Match"));

    // Date & Time
    QString dateTimeGMT = dataObj["dateTimeGMT"].toString();
    QString dateTimeDisplay = "Time not available";
    if (!dateTimeGMT.isEmpty()) {
        QDateTime utc = QDateTime::fromString(dateTimeGMT, Qt::ISODate);
        if (utc.isValid()) {
            utc = utc.toUTC();
            dateTimeDisplay = utc.toLocalTime().toString("ddd, dd MMM yyyy • hh:mm AP");
        }
    }
    m_dateLabel->setText("Date: " + dateTimeDisplay);

    // Venue
    m_venueLabel->setText("Venue: " + dataObj["venue"].toString("Not specified"));

    // Scorecard
    QJsonArray teams = dataObj["teams"].toArray();
    QJsonArray scores = dataObj["score"].toArray();

    QString scoreHtml = "<div style='line-height:2.2; font-size:16px;'>";
    if (teams.size() >= 2 && !scores.isEmpty()) {
        for (int i = 0; i < 2 && i < scores.size(); ++i) {
            QJsonObject s = scores[i].toObject();
            QString teamName = teams[i].toString();
            int runs = s["r"].toInt();
            int wickets = s["w"].toInt();
            double overs = s["o"].toDouble();
            QString inning = s["inning"].toString();

            scoreHtml += QString("<b style='color:#09B83E;'>%1</b><br>")
                             .arg(teamName.toHtmlEscaped());
            scoreHtml += QString("%1/%2 (%3 overs)<br><small>%4</small><br><br>")
                             .arg(runs).arg(wickets).arg(overs, 0, 'f', 1)
                             .arg(inning.toHtmlEscaped());
        }
    } else {
        scoreHtml += "<i>No score available yet</i>";
    }
    scoreHtml += "</div>";
    m_scoreLabel->setText(scoreHtml);

    // Status
    QString status = dataObj["status"].toString("In Progress");
    if (status.contains("won", Qt::CaseInsensitive)) {
        m_statusLabel->setStyleSheet("background:#4caf50; color:white; padding:14px; border-radius:10px; margin:8px;");
    } else if (status.contains("live", Qt::CaseInsensitive)) {
        m_statusLabel->setStyleSheet("background:#f57c00; color:white; padding:14px; border-radius:10px; margin:8px;");
    }
    m_statusLabel->setText("<b>Status:</b> " + status.toHtmlEscaped());

    reply->deleteLater();
}
