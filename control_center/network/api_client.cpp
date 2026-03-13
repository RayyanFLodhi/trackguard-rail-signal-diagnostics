#include "api_client.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent),
      networkManager(new QNetworkAccessManager(this))
{
}

void ApiClient::fetchDevices()
{
    const QUrl url("http://127.0.0.1:5000/devices");
    QNetworkRequest request{url};

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            emit requestFailed("Failed to fetch devices: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        const QByteArray responseData = reply->readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (!jsonDoc.isArray())
        {
            emit requestFailed("Invalid device list response from server.");
            reply->deleteLater();
            return;
        }

        emit devicesReceived(jsonDoc.array());
        reply->deleteLater();
    });
}

void ApiClient::runDiagnostic(const QString &deviceId)
{
    const QString urlString = "http://127.0.0.1:5000/devices/" + deviceId + "/run_test";
    const QUrl url(urlString);
    QNetworkRequest request{url};

    QNetworkReply *reply = networkManager->post(request, QByteArray());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            emit requestFailed("Failed to run diagnostic: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        const QByteArray responseData = reply->readAll();
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (!jsonDoc.isObject())
        {
            emit requestFailed("Invalid diagnostic response from server.");
            reply->deleteLater();
            return;
        }

        const QJsonObject rootObject = jsonDoc.object();
        const QJsonObject diagnosticObject = rootObject.value("diagnostic").toObject();
        const QString result = diagnosticObject.value("result").toString();

        if (result.isEmpty())
        {
            emit requestFailed("Diagnostic response missing result field.");
            reply->deleteLater();
            return;
        }

        emit diagnosticReceived(result);
        reply->deleteLater();
    });
}