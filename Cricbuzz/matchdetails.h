// matchdetails.h
#ifndef MATCHDETAILS_H
#define MATCHDETAILS_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QString>
#include "matchinfo.h"
class QVBoxLayout;
class QLabel;
class QPushButton;

class MatchDetails : public QWidget
{
    Q_OBJECT

public:
    explicit MatchDetails(QWidget *parent = nullptr);

public slots:
    void loadMatch(const QString &matchId);

private slots:
    void onNetworkFinished(QNetworkReply *reply);
signals:
    void backClicked();
private:
    QVBoxLayout *m_layout;

    QLabel *m_title;
    QLabel *m_scoreLabel;
    QLabel *m_statusLabel;
    QLabel *m_venueLabel;
    QLabel *m_dateLabel;

    QPushButton *m_backBtn;

    QNetworkAccessManager *m_manager;
    QString m_currentMatchId;

    const QString m_apiBaseUrl;
    const QString m_apiKey;
};

#endif // MATCHDETAILS_H
