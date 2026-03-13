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

void ApiClient::injectFault(const QString &deviceId, const QString &fault)
{
    const QString urlString = "http://127.0.0.1:5000/devices/" + deviceId + "/inject_fault";
    QNetworkRequest request{QUrl(urlString)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["fault"] = fault;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, deviceId, fault]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            emit requestFailed("Failed to inject fault on " + deviceId + ": " + reply->errorString());
            reply->deleteLater();
            return;
        }

        emit actionSucceeded("Injected fault on " + deviceId + ": " + fault);
        reply->deleteLater();
    });
}

void ApiClient::clearFault(const QString &deviceId)
{
    const QString urlString = "http://127.0.0.1:5000/devices/" + deviceId + "/clear_fault";
    QNetworkRequest request{QUrl(urlString)};

    QNetworkReply *reply = networkManager->post(request, QByteArray());

    connect(reply, &QNetworkReply::finished, this, [this, reply, deviceId]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            emit requestFailed("Failed to clear fault on " + deviceId + ": " + reply->errorString());
            reply->deleteLater();
            return;
        }

        emit actionSucceeded("Cleared fault on " + deviceId);
        reply->deleteLater();
    });
}