#include "upcoming.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDateTime>

Upcoming::Upcoming(QWidget *parent)
    : QWidget(parent),
    m_layout(new QVBoxLayout(this)),
    m_manager(new QNetworkAccessManager(this)),
    m_apiBaseUrl("https://api.cricapi.com/v1"),
    m_apiKey("70474f9c-275b-4a9f-8209-ef11ba9f877c")
{
    m_layout->setContentsMargins(12, 12, 12, 12);
    m_layout->setSpacing(10);

    QLabel *title = new QLabel("Upcoming Matches");
    title->setStyleSheet("font-weight:700; font-size:20px; color:#09B83E; padding:8px;");
    title->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(title);

    connect(m_manager, &QNetworkAccessManager::finished, this, &Upcoming::onNetworkFinished);
    fetchUpcoming();
}

void Upcoming::fetchUpcoming()
{
    QString url = QString("%1/matches?apikey=%2").arg(m_apiBaseUrl, m_apiKey);
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_manager->get(request);
}

void Upcoming::onNetworkFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_layout->addWidget(new QLabel("<p style='color:red;'>Failed to load matches.</p>"));
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        m_layout->addWidget(new QLabel("Invalid response."));
        reply->deleteLater();
        return;
    }

    QJsonArray matches = doc.object()["data"].toArray();
    bool hasUpcoming = false;

    for (const QJsonValue &val : matches) {
        QJsonObject match = val.toObject();

        if (match["matchStarted"].toBool() || match["matchEnded"].toBool()) {
            continue; // Skip live/finished
        }

        hasUpcoming = true;

        QString name = match["name"].toString("Upcoming Match");
        QString venue = match["venue"].toString("Venue TBD");
        QString dateTimeGMT = match["dateTimeGMT"].toString();

        QString timeDisplay = "Time TBD";
        if (!dateTimeGMT.isEmpty()) {
            QDateTime utc = QDateTime::fromString(dateTimeGMT, Qt::ISODate);
            if (utc.isValid()) {
                utc = utc.toUTC();
                timeDisplay = utc.toLocalTime().toString("ddd, dd MMM • hh:mm AP");
            }
        }

        QLabel *card = new QLabel();
        card->setWordWrap(true);
        card->setText(QString(
                          "<div style='padding:16px; background:white; border-radius:12px; "
                          "box-shadow:0 2px 10px rgba(0,0,0,0.1); margin:6px 0;'>"
                          "<b style='font-size:16px; color:#09B83E;'>%1</b><br>"
                          "<small style='color:#555;'>%2</small><br>"
                          "<small style='color:#888;'>%3</small>"
                          "</div>"
                          ).arg(name.toHtmlEscaped(), venue.toHtmlEscaped(), timeDisplay));

        m_layout->addWidget(card);
    }

    if (!hasUpcoming) {
        QLabel *lbl = new QLabel("No upcoming matches scheduled.");
        lbl->setStyleSheet("color:#888; font-style:italic; padding:30px;");
        lbl->setAlignment(Qt::AlignCenter);
        m_layout->addWidget(lbl);
    }

    m_layout->addStretch();
    reply->deleteLater();
}
