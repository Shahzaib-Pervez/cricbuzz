// upcoming.h
#ifndef UPCOMING_H
#define UPCOMING_H

#include <QWidget>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include "matchinfo.h"
class Upcoming : public QWidget
{
    Q_OBJECT

public:
    explicit Upcoming(QWidget *parent = nullptr);

private slots:
    void onNetworkFinished(QNetworkReply *reply);

private:
    void fetchUpcoming();

    QVBoxLayout *m_layout;
    QNetworkAccessManager *m_manager;

    const QString m_apiBaseUrl;
    const QString m_apiKey;
};

#endif // UPCOMING_H
