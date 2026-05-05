#ifndef CALORIECHECKER_H
#define CALORIECHECKER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class CalorieChecker : public QObject
{
    Q_OBJECT

public:
    explicit CalorieChecker(QObject *parent = nullptr);
    void checkCalories(const QString &foodName);

signals:
    void resultReady(const QString &result);

private:
    QNetworkAccessManager *networkManager;
    QString apiKey;
};

#endif