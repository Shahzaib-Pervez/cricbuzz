#pragma once
#include <QWidget>
#include "matchinfo.h"

class QLabel;
class QPushButton;

class MatchCard : public QWidget
{
    Q_OBJECT
public:
    explicit MatchCard(const MatchInfo &info, QWidget *parent = nullptr);

    QString matchId() const;

    void updateData(const QString &status,
                    const QString &t1score,
                    const QString &t2score);

signals:
    void scorecardClicked(const QString &matchId);

private:
    void updateStatusStyle(const QString &status);
    QString extractMatchType(const QString &title);

private:
    QString m_matchId;
    QLabel *m_title;
    QLabel *m_status;
    QLabel *m_score;
    QPushButton *m_btnDetails;
};
