#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    void fetchDevices();
    void runDiagnostic(const QString &deviceId);
    void injectFault(const QString &deviceId, const QString &fault);
    void clearFault(const QString &deviceId);

signals:
    void devicesReceived(QJsonArray devices);
    void diagnosticReceived(QString result);
    void actionSucceeded(QString message);
    void requestFailed(QString errorMessage);

private:
    QNetworkAccessManager *networkManager;
};

#endif // API_CLIENT_H