#include "mainwindow.h"

#include <QWidget>
#include <QLabel>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QBrush>
#include <QColor>
#include <QTableWidgetItem>
#include <QJsonObject>
#include <QAbstractItemView>
#include <QItemSelectionModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      deviceTable(nullptr),
      refreshButton(nullptr),
      diagnosticButton(nullptr),
      injectFaultButton(nullptr),
      clearFaultButton(nullptr),
      exportLogsButton(nullptr),
      logViewer(nullptr),
      apiClient(nullptr)
{
    setupUi();

    apiClient = new ApiClient(this);

    connect(apiClient, &ApiClient::devicesReceived,
            this, &MainWindow::updateDeviceTable);

    connect(apiClient, &ApiClient::diagnosticReceived,
            this, &MainWindow::diagnosticResult);

    connect(apiClient, &ApiClient::actionSucceeded,
            this, &MainWindow::handleActionSucceeded);

    connect(apiClient, &ApiClient::requestFailed,
            this, &MainWindow::handleRequestFailure);

    appendLog("TrackGuard control center started.");
    appendLog("Click 'Refresh Status' to load live devices from the simulator.");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle("TrackGuard - Rail Signal Control & Diagnostics");
    resize(1000, 650);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QLabel *titleLabel = new QLabel("TrackGuard Control Center");
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; padding: 8px;");
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("Rail Signal Control & Diagnostics Simulator");
    subtitleLabel->setStyleSheet("font-size: 13px; color: gray; padding-left: 8px;");
    mainLayout->addWidget(subtitleLabel);

    deviceTable = new QTableWidget(this);
    deviceTable->setColumnCount(4);
    deviceTable->setHorizontalHeaderLabels({"Device ID", "Type", "State", "Fault"});
    deviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    deviceTable->setSelectionMode(QAbstractItemView::SingleSelection);
    deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    deviceTable->setAlternatingRowColors(true);
    mainLayout->addWidget(deviceTable);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    refreshButton = new QPushButton("Refresh Status");
    diagnosticButton = new QPushButton("Run Diagnostic");
    injectFaultButton = new QPushButton("Inject Fault");
    clearFaultButton = new QPushButton("Clear Fault");
    exportLogsButton = new QPushButton("Export Logs");

    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(diagnosticButton);
    buttonLayout->addWidget(injectFaultButton);
    buttonLayout->addWidget(clearFaultButton);
    buttonLayout->addWidget(exportLogsButton);

    mainLayout->addLayout(buttonLayout);

    QLabel *logLabel = new QLabel("Event Log");
    logLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding-top: 8px;");
    mainLayout->addWidget(logLabel);

    logViewer = new QTextEdit(this);
    logViewer->setReadOnly(true);
    mainLayout->addWidget(logViewer);

    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshDevices);
    connect(diagnosticButton, &QPushButton::clicked, this, &MainWindow::runDiagnostic);
    connect(injectFaultButton, &QPushButton::clicked, this, &MainWindow::injectFault);
    connect(clearFaultButton, &QPushButton::clicked, this, &MainWindow::clearFault);
    connect(exportLogsButton, &QPushButton::clicked, this, &MainWindow::exportLogs);
}

void MainWindow::appendLog(const QString &message)
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    logViewer->append(QString("[%1] %2").arg(timestamp, message));
}

int MainWindow::getSelectedRow() const
{
    QModelIndexList selectedRows = deviceTable->selectionModel()->selectedRows();
    if (selectedRows.isEmpty())
    {
        return -1;
    }

    return selectedRows.first().row();
}

void MainWindow::refreshDevices()
{
    appendLog("Fetching device list from simulator...");
    apiClient->fetchDevices();
}

void MainWindow::runDiagnostic()
{
    const int row = getSelectedRow();

    if (row < 0)
    {
        QMessageBox::warning(this, "No Device Selected", "Please select a device first.");
        return;
    }

    QTableWidgetItem *idItem = deviceTable->item(row, 0);
    if (idItem == nullptr)
    {
        QMessageBox::warning(this, "Invalid Selection", "Could not determine selected device.");
        return;
    }

    const QString deviceId = idItem->text();

    appendLog("Running diagnostic for " + deviceId);
    apiClient->runDiagnostic(deviceId);
}

void MainWindow::injectFault()
{
    const int row = getSelectedRow();

    if (row < 0)
    {
        QMessageBox::warning(this, "No Device Selected", "Please select a device first.");
        return;
    }

    QTableWidgetItem *idItem = deviceTable->item(row, 0);
    QTableWidgetItem *typeItem = deviceTable->item(row, 1);

    if (idItem == nullptr || typeItem == nullptr)
    {
        QMessageBox::warning(this, "Invalid Selection", "Could not determine selected device.");
        return;
    }

    const QString deviceId = idItem->text();
    const QString deviceType = typeItem->text();
    const QString fault = getSuggestedFaultForType(deviceType);

    appendLog("Injecting fault on " + deviceId + ": " + fault);
    apiClient->injectFault(deviceId, fault);
}

void MainWindow::clearFault()
{
    const int row = getSelectedRow();

    if (row < 0)
    {
        QMessageBox::warning(this, "No Device Selected", "Please select a device first.");
        return;
    }

    QTableWidgetItem *idItem = deviceTable->item(row, 0);

    if (idItem == nullptr)
    {
        QMessageBox::warning(this, "Invalid Selection", "Could not determine selected device.");
        return;
    }

    const QString deviceId = idItem->text();

    appendLog("Clearing fault on " + deviceId);
    apiClient->clearFault(deviceId);
}

QString MainWindow::getSuggestedFaultForType(const QString &type) const
{
    if (type == "signal")
    {
        return "LAMP_FAILURE";
    }
    else if (type == "track_sensor")
    {
        return "STUCK_OCCUPIED";
    }
    else if (type == "switch")
    {
        return "FAILED_TO_MOVE";
    }

    return "NONE";
}

void MainWindow::handleActionSucceeded(QString message)
{
    appendLog(message);
    apiClient->fetchDevices();
}

void MainWindow::exportLogs()
{
    QDir projectRoot(QDir::currentPath());

    if (!projectRoot.exists("logs"))
    {
        projectRoot.mkpath("logs");
    }

    if (!projectRoot.exists("logs/exports"))
    {
        projectRoot.mkpath("logs/exports");
    }

    const QString filePath = projectRoot.filePath("logs/exports/control_center_export.csv");
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Export Failed", "Could not write export log file.");
        return;
    }

    QTextStream out(&file);
    out << "device_id,type,state,fault\n";

    for (int row = 0; row < deviceTable->rowCount(); ++row)
    {
        const QString id = deviceTable->item(row, 0) ? deviceTable->item(row, 0)->text() : "";
        const QString type = deviceTable->item(row, 1) ? deviceTable->item(row, 1)->text() : "";
        const QString state = deviceTable->item(row, 2) ? deviceTable->item(row, 2)->text() : "";
        const QString fault = deviceTable->item(row, 3) ? deviceTable->item(row, 3)->text() : "";

        out << id << "," << type << "," << state << "," << fault << "\n";
    }

    file.close();

    appendLog(QString("Exported device table to %1").arg(filePath));
    QMessageBox::information(this, "Export Complete", "Device data exported successfully.");
}

void MainWindow::updateDeviceTable(QJsonArray devicesJson)
{
    deviceTable->clearContents();
    deviceTable->setRowCount(devicesJson.size());

    for (int i = 0; i < devicesJson.size(); ++i)
    {
        const QJsonObject obj = devicesJson[i].toObject();

        const QString id = obj.value("id").toString();
        const QString type = obj.value("type").toString();
        const QString state = obj.value("state").toString();
        const QString fault = obj.value("fault").toString();

        deviceTable->setItem(i, 0, new QTableWidgetItem(id));
        deviceTable->setItem(i, 1, new QTableWidgetItem(type));
        deviceTable->setItem(i, 2, new QTableWidgetItem(state));
        deviceTable->setItem(i, 3, new QTableWidgetItem(fault));

        colorizeRow(i, type, state, fault);
    }

    appendLog("Device table updated from server.");
}

void MainWindow::diagnosticResult(QString result)
{
    appendLog("Diagnostic result: " + result);
}

void MainWindow::handleRequestFailure(QString errorMessage)
{
    appendLog(errorMessage);
    QMessageBox::warning(this, "Request Failed", errorMessage);
}

void MainWindow::colorizeRow(int row, const QString &type, const QString &state, const QString &fault)
{
    QTableWidgetItem *idItem = deviceTable->item(row, 0);
    QTableWidgetItem *typeItem = deviceTable->item(row, 1);
    QTableWidgetItem *stateItem = deviceTable->item(row, 2);
    QTableWidgetItem *faultItem = deviceTable->item(row, 3);

    QList<QTableWidgetItem*> rowItems = {idItem, typeItem, stateItem, faultItem};

    for (QTableWidgetItem *item : rowItems)
    {
        if (item != nullptr)
        {
            item->setForeground(QBrush(QColor(20, 20, 20)));
        }
    }

    if (stateItem != nullptr)
    {
        if (type == "signal")
        {
            if (state == "GREEN")
            {
                stateItem->setBackground(QBrush(QColor(144, 238, 144)));
            }
            else if (state == "YELLOW")
            {
                stateItem->setBackground(QBrush(QColor(255, 255, 153)));
            }
            else if (state == "RED")
            {
                stateItem->setBackground(QBrush(QColor(255, 160, 160)));
            }
        }
        else if (type == "track_sensor")
        {
            if (state == "OCCUPIED")
            {
                stateItem->setBackground(QBrush(QColor(255, 204, 153)));
            }
            else if (state == "CLEAR")
            {
                stateItem->setBackground(QBrush(QColor(204, 255, 204)));
            }
        }
        else if (type == "switch")
        {
            if (state == "STRAIGHT")
            {
                stateItem->setBackground(QBrush(QColor(204, 229, 255)));
            }
            else if (state == "DIVERGING")
            {
                stateItem->setBackground(QBrush(QColor(221, 204, 255)));
            }
        }
    }

    if (faultItem != nullptr)
    {
        if (fault != "NONE")
        {
            faultItem->setBackground(QBrush(QColor(255, 120, 120)));
            faultItem->setForeground(QBrush(QColor(0, 0, 0)));
        }
        else
        {
            faultItem->setBackground(QBrush(QColor(204, 255, 204)));
            faultItem->setForeground(QBrush(QColor(20, 20, 20)));
        }
    }

    if (fault != "NONE")
    {
        for (QTableWidgetItem *item : rowItems)
        {
            if (item != nullptr && item != faultItem)
            {
                item->setBackground(QBrush(QColor(255, 235, 235)));
            }
        }
    }
}