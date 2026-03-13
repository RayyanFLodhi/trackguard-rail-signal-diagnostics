# TrackGuard – Rail Signal Diagnostics Simulator

TrackGuard is a **railway signaling diagnostics simulator** that demonstrates a full **client-server architecture** for monitoring and testing rail control devices.

The system simulates railway infrastructure devices such as:

- Signals
- Track occupancy sensors
- Track switches

A **Qt C++ control center** communicates with a **Python Flask backend** over **HTTP (TCP/IP)** to monitor device states, inject faults, and run diagnostics.

Exported diagnostic data can be processed using **Perl automation scripts** and **Excel VBA reporting tools**.

This project demonstrates how real industrial control systems are structured, separating:

- control interface
- device simulation
- diagnostic logic
- reporting / automation tools

---

# Demo Videos

### Qt Control Center (C++ / Qt GUI)
Shows the TrackGuard control center monitoring devices, running diagnostics, and injecting faults.

▶ https://streamable.com/7kjtpk

---

### Perl Log Parsing Automation
Demonstrates automated log analysis using Perl.

▶ https://streamable.com/wlhsip

---

### Excel VBA Report Automation
Demonstrates automated formatting and reporting using Excel VBA.

▶ https://streamable.com/r9bnlx

---

# System Architecture

The system is composed of three major layers.

```
Qt Control Center (C++ / Qt GUI)
        |
        | HTTP REST API (TCP/IP)
        |
Python Flask Simulator (Backend)
        |
        | Simulated rail devices
        |
Exported CSV logs
      |          |
      |          +---- Excel VBA formatting
      |
      +---- Perl automation scripts
```

### Control Layer
The Qt application acts as the **rail control center dashboard**.

It allows operators to:

- view device states
- run diagnostics
- inject device faults
- clear device faults
- export device status reports

### Simulation Layer
The Flask backend simulates railway hardware.

Device state is stored in memory and exposed through a REST API.

### Reporting Layer
Exported device data can be analyzed using:

- **Perl** scripts for automated summaries
- **Excel VBA** for formatted engineering reports

---

# Technologies Used

| Technology | Purpose |
|--------|--------|
| C++ | Qt control center application |
| Qt | GUI framework and networking |
| Python | Backend simulation server |
| Flask | REST API for device communication |
| HTTP / TCP/IP | Communication between Qt and Flask |
| JSON | Data exchange format |
| Perl | Log parsing and automation |
| Excel VBA | Automated report formatting |
| CSV | Exported device data format |

---

# Project Structure

```
trackguard-rail-signal-diagnostics
│
├── control_center
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── mainwindow.ui
│   └── network
│       ├── api_client.cpp
│       └── api_client.h
│
├── simulator
│   ├── server.py
│   ├── device_models.py
│   ├── diagnostics.py
│   └── requirements.txt
│
├── automation
│   ├── perl
│   │   └── parse_logs.pl
│   └── vba
│       └── format_report.bas
│
├── logs
│   └── exports
│
└── README.md
```

---

# Backend Components

### device_models.py
Defines the structure of simulated railway devices.

Examples include:

- signal controllers
- track occupancy sensors
- switch controllers

Each device includes:

```
device id
device type
state
fault status
```

---

### diagnostics.py
Contains diagnostic logic used to test simulated devices.

Example diagnostic behavior:

```
If fault == NONE -> PASS
If fault != NONE -> FAIL
```

---

### server.py
The main runtime system.

Responsibilities:

- stores simulated device state
- exposes REST endpoints
- runs diagnostics
- injects and clears faults

Example device state storage:

```
devices = {
    "SIG-01": SignalController(...),
    "TRK-01": TrackSensor(...),
}
```

---

# REST API Endpoints

The Qt application communicates with Flask using HTTP.

### Retrieve Device List

```
GET /devices
```

Returns JSON describing all simulated devices.

---

### Run Device Diagnostic

```
POST /devices/<device_id>/run_test
```

Returns PASS/FAIL diagnostic result.

---

### Inject Fault

```
POST /devices/<device_id>/inject_fault
```

Updates device state with a simulated fault.

---

### Clear Fault

```
POST /devices/<device_id>/clear_fault
```

Removes any injected fault from a device.

---

# Qt Control Center Features

The Qt GUI provides a monitoring dashboard with:

- live device table
- color-coded device state
- diagnostic execution
- fault injection
- fault clearing
- event logging
- CSV export

Device states are visually highlighted:

| State | Color |
|-----|-----|
| GREEN | Light green |
| YELLOW | Yellow |
| RED | Red |
| Fault present | Highlighted row |

---

# Perl Log Automation

Perl is used for automated parsing of exported CSV logs.

The script:

```
automation/perl/parse_logs.pl
```

Reads exported device data and generates a summary report.

Example output:

```
TrackGuard Device Summary
-------------------------
Total devices: 6
Healthy devices: 4
Faulted devices: 2

Fault breakdown:
LAMP_FAILURE: 1
STUCK_OCCUPIED: 1
```

Run the script from the repository root:

```
perl automation/perl/parse_logs.pl
```

---

# Excel VBA Reporting

The project includes an Excel macro that automatically formats exported device reports.

File:

```
automation/vba/format_report.bas
```

Features:

- highlights healthy rows
- highlights faulted rows
- creates summary statistics
- formats headers
- automatically resizes columns

---

# How to Run the Project

## 1. Start the Backend Simulator

Install dependencies:

```
pip install -r simulator/requirements.txt
```

Start the Flask server:

```
python simulator/server.py
```

The server will start on:

```
http://127.0.0.1:5000
```

---

## 2. Launch the Qt Control Center

Open the project in **Qt Creator**.

Build and run the `TrackGuard` application.

---

## 3. Use the Dashboard

Inside the UI:

1. Click **Refresh Status**
2. Select a device
3. Run **Run Diagnostic**
4. Inject faults
5. Clear faults
6. Export logs

---

## 4. Export Device Data

Click:

```
Export Logs
```

This generates:

```
logs/exports/control_center_export.csv
```

---

## 5. Run the Perl Parser

From the project root:

```
perl automation/perl/parse_logs.pl
```

This generates a fault summary report.

---

## 6. Use Excel VBA Report Formatting

1. Open the exported CSV in Excel
2. Press `ALT + F11`
3. Insert a module
4. Paste contents of:

```
automation/vba/format_report.bas
```

5. Run:

```
FormatTrackGuardReport
```

The spreadsheet will automatically format and summarize the data.

---

# Key Design Concepts Demonstrated

This project demonstrates several important engineering concepts:

### Client-Server Architecture
Qt GUI acts as a client communicating with a backend API.

### REST API Design
Flask exposes endpoints that simulate real industrial device interfaces.

### TCP/IP Networking
Communication occurs over HTTP built on TCP/IP.

### Hardware Simulation
Rail devices are simulated in software using object models.

### Diagnostics Workflow
Device faults can be injected and tested using diagnostic routines.

### Automation and Reporting
Exported logs can be automatically processed using scripting tools.

---

# Potential Extensions

Possible improvements include:

- real-time device state updates
- graphical rail track map visualization
- persistent database storage
- WebSocket live telemetry
- historical diagnostics tracking

---

# License

MIT License

---

# Author

TrackGuard Rail Signal Diagnostics Simulator  
Developed as a demonstration of control system monitoring architecture using Qt, Flask, and automation tools.