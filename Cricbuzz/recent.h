#ifndef RECENT_H
#define RECENT_H

#include <QWidget>
#include <QList>
#include <QNetworkReply>
struct RecentMatchInfo {
    QString id;
    QString team1;
    QString team2;
    QString team1Score;
    QString team2Score;
    QString result;     // e.g., "India won by 6 wickets"
    QString dateTime;
    QString venue;
};

class QLabel;
class QVBoxLayout;
class QScrollArea;
class QNetworkAccessManager;
class QTimer;

class Recent : public QWidget
{
    Q_OBJECT

public:
    explicit Recent(QWidget *parent = nullptr);

signals:
    void matchClicked(const QString &matchId);

private slots:
    void fetchRecent();
    void onReply(QNetworkReply *reply);

private:
    void clearCards();
    void addCard(const RecentMatchInfo &info);

    QVBoxLayout *m_layout;
    QScrollArea *m_scroll;
    QWidget *m_container;
    QVBoxLayout *m_cardsLayout;
    QLabel *m_statusLabel;

    QNetworkAccessManager *m_manager;
    QTimer *m_timer;
    QList<QLabel*> m_cards;
};

#endif // RECENT_H
