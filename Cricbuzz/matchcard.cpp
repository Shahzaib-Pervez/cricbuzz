#include "matchcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

MatchCard::MatchCard(const MatchInfo &info, QWidget *parent)
    : QWidget(parent),
    m_matchId(info.id),
    m_title(new QLabel(info.title)),
    m_status(new QLabel(info.status)),
    m_btnDetails(new QPushButton("View Scorecard"))
{
    setObjectName("matchCard");
    setStyleSheet(
        "QWidget#matchCard { background: white; border-radius: 12px; border: 1px solid #e0e0e0; }"
        "QWidget#matchCard:hover { border: 1px solid #09B83E; "
        "box-shadow: 0 4px 14px rgba(0,0,0,0.15); }"
        );

    setMinimumHeight(180);

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setContentsMargins(20, 16, 20, 16);
    main->setSpacing(12);

    // ---------------------- TOP ROW (STATUS + MATCH TYPE) ----------------------
    QHBoxLayout *topRow = new QHBoxLayout;
    topRow->setSpacing(8);

    // Status badge
    updateStatusStyle(info.status);
    m_status->setAlignment(Qt::AlignCenter);
    m_status->setFixedHeight(24);
    m_status->setMinimumWidth(80);

    topRow->addWidget(m_status);
    topRow->addStretch();

    // Match type (ODI/T20/Test)
    QString matchType = extractMatchType(info.title);
    if (!matchType.isEmpty()) {
        QLabel *typeLabel = new QLabel(matchType);
        typeLabel->setStyleSheet(
            "font-size: 11px; font-weight: 700; color: #444; "
            "background: #f0f0f0; border-radius: 4px; padding: 4px 10px;"
            );
        topRow->addWidget(typeLabel);
    }

    main->addLayout(topRow);

    // ---------------------- MATCH TITLE ----------------------
    m_title->setStyleSheet(
        "font-weight: 700; font-size: 15px; color: #222;"
        );
    m_title->setWordWrap(true);
    main->addWidget(m_title);

    // ---------------------- TEAM SCORES (CRICBUZZ STYLE) ----------------------
    QVBoxLayout *teamLayout = new QVBoxLayout;
    teamLayout->setSpacing(10);

    QStringList teams = {info.team1, info.team2};
    QStringList scores = {info.team1Score, info.team2Score};

    for (int i = 0; i < teams.size(); i++) {
        QHBoxLayout *row = new QHBoxLayout;
        row->setSpacing(12);

        QLabel *flag = new QLabel("🏏");
        flag->setFixedSize(26, 26);
        flag->setStyleSheet("font-size: 22px;");

        QLabel *teamName = new QLabel(teams[i]);
        teamName->setStyleSheet(
            "font-size: 15px; font-weight: 700; color: #111;"
            );
        teamName->setFixedWidth(150);

        QLabel *teamScore = new QLabel(scores[i].isEmpty() ? "-" : scores[i]);
        teamScore->setStyleSheet(
            "font-size: 15px; font-weight: 700; color: #111;"
            );
        teamScore->setAlignment(Qt::AlignRight);

        row->addWidget(flag);
        row->addWidget(teamName);
        row->addWidget(teamScore, 1);
        row->addStretch();

        teamLayout->addLayout(row);
    }

    main->addLayout(teamLayout);

    // ---------------------- DIVIDER ----------------------
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color:#eaeaea; min-height:1px;");
    main->addWidget(line);

    // ---------------------- BOTTOM BUTTON ----------------------
    QHBoxLayout *btm = new QHBoxLayout;
    btm->addStretch();

    m_btnDetails->setCursor(Qt::PointingHandCursor);
    m_btnDetails->setFixedHeight(36);
    m_btnDetails->setStyleSheet(
        "QPushButton { padding: 8px 24px; border-radius:8px; "
        "background:#09B83E; color:white; font-size:13px; font-weight:600; }"
        "QPushButton:hover { background:#078C32; }"
        "QPushButton:pressed { background:#066A2A; }"
        );

    btm->addWidget(m_btnDetails);
    main->addLayout(btm);

    connect(m_btnDetails, &QPushButton::clicked, this, [this]() {
        emit scorecardClicked(m_matchId);
    });
}

QString MatchCard::matchId() const {
    return m_matchId;
}

void MatchCard::updateData(const QString &status, const QString &t1score, const QString &t2score)
{
    m_status->setText(status);
    updateStatusStyle(status);
}

void MatchCard::updateStatusStyle(const QString &status)
{
    QString bg, text, label;

    QString s = status.toLower();

    if (s.contains("live")) {
        bg = "#ff5252"; text = "white"; label = "● LIVE";
    }
    else if (s.contains("won") || s.contains("completed") || s.contains("result")) {
        bg = "#eeeeee"; text = "#444"; label = "RESULT";
    }
    else if (s.contains("upcoming") || s.contains("fixture")) {
        bg = "#fff4e5"; text = "#e65100"; label = "UPCOMING";
    }
    else {
        bg = "#e3f2fd"; text = "#0d47a1"; label = "SCHEDULED";
    }

    m_status->setText(label);
    m_status->setStyleSheet(
        QString("background:%1;color:%2;font-weight:700;font-size:11px;"
                "border-radius:4px;padding:4px 12px;").arg(bg, text)
        );
}

QString MatchCard::extractMatchType(const QString &title)
{
    if (title.contains("T20", Qt::CaseInsensitive)) return "T20";
    if (title.contains("ODI", Qt::CaseInsensitive)) return "ODI";
    if (title.contains("Test", Qt::CaseInsensitive)) return "TEST";
    return "";
}
