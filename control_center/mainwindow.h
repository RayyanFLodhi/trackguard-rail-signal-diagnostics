#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>

#include "network/api_client.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshDevices();
    void runDiagnostic();
    void injectFault();
    void exportLogs();

    void updateDeviceTable(QJsonArray devicesJson);
    void diagnosticResult(QString result);
    void handleRequestFailure(QString errorMessage);

private:
    void setupUi();
    void appendLog(const QString &message);
    int getSelectedRow() const;
    void colorizeRow(int row, const QString &type, const QString &state, const QString &fault);

    QTableWidget *deviceTable;
    QPushButton *refreshButton;
    QPushButton *diagnosticButton;
    QPushButton *injectFaultButton;
    QPushButton *exportLogsButton;
    QTextEdit *logViewer;

    ApiClient *apiClient;
};

#endif // MAINWINDOW_H