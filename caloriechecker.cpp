#include "caloriechecker.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

CalorieChecker::CalorieChecker(QObject *parent)
    : QObject(parent),
    networkManager(new QNetworkAccessManager(this)),
    apiKey("QbSUCwz+h2Yys2b94MgPdA==GOQxN9lihtcINFj9")
{
}

void CalorieChecker::checkCalories(const QString &foodName)
{
    QUrl url("https://api.calorieninjas.com/v1/nutrition");

    QUrlQuery query;
    query.addQueryItem("query", foodName);
    url.setQuery(query);

    QNetworkRequest request{url};
    request.setRawHeader("X-Api-Key", apiKey.toUtf8());

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray responseData = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            emit resultReady("API error: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(responseData);

        if (!doc.isObject()) {
            emit resultReady("Could not read nutrition data.");
            reply->deleteLater();
            return;
        }

        QJsonObject root = doc.object();
        QJsonArray items = root["items"].toArray();

        if (items.isEmpty()) {
            emit resultReady("No nutrition result found for this food.");
            reply->deleteLater();
            return;
        }

        QString result;

        for (const QJsonValue &value : items) {
            QJsonObject item = value.toObject();

            QString name = item["name"].toString("Food");
            double serving = item["serving_size_g"].toDouble();
            double calories = item["calories"].toDouble();
            double protein = item["protein_g"].toDouble();
            double carbs = item["carbohydrates_total_g"].toDouble();
            double fat = item["fat_total_g"].toDouble();

            result += "🍽 " + name.toUpper() + "\n\n";
            result += "Serving size: " + QString::number(serving, 'f', 0) + " g\n";
            result += "🔥 Calories: " + QString::number(calories, 'f', 0) + " kcal\n";
            result += "🥩 Protein: " + QString::number(protein, 'f', 1) + " g\n";
            result += "🍞 Carbs: " + QString::number(carbs, 'f', 1) + " g\n";
            result += "🧈 Fat: " + QString::number(fat, 'f', 1) + " g\n\n";
        }

        emit resultReady(result.trimmed());
        reply->deleteLater();
    });
}