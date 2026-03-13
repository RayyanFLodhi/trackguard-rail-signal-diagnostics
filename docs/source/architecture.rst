Architecture
============

TrackGuard is built as a **layered client-server architecture** that simulates a rail signal monitoring and diagnostics system.

The system separates:

* the operator control interface
* the backend device simulator
* the diagnostics logic
* reporting and automation tools

This structure mirrors the design used in many industrial control systems.

System Overview
---------------

TrackGuard consists of three primary components:

1. Qt C++ Control Center
2. Python Flask Simulator Backend
3. Reporting and Automation Tools

Each component has a specific role within the system.

Qt Control Center
-----------------

The Qt desktop application acts as the **rail control center dashboard**.

Responsibilities:

* Display device state information
* Allow engineers to run diagnostics
* Allow injection and clearing of simulated faults
* Provide event logging
* Export device data to CSV

The control center communicates with the backend simulator using **HTTP REST requests over TCP/IP**.

Python Simulator Backend
------------------------

The backend is implemented using **Python and Flask**.

It simulates railway infrastructure devices including:

* signal controllers
* track occupancy sensors
* track switches

Responsibilities:

* Maintain simulated device state in memory
* Expose REST API endpoints
* Execute diagnostic routines
* Apply fault injection commands

Device behavior and diagnostic rules are separated into modules:

* ``device_models.py`` – device data structures
* ``diagnostics.py`` – diagnostic test logic
* ``server.py`` – runtime system and REST API

Reporting and Automation
------------------------

TrackGuard exports device information to CSV files for further analysis.

Two automation tools are included:

**Perl Automation**

* Parses exported logs
* Generates summaries of device health and fault statistics

**Excel VBA Reporting**

* Automatically formats exported CSV files
* Highlights healthy and faulted devices
* Generates summary statistics for engineering reports

High-Level Architecture Diagram
-------------------------------

::

      +-------------------------------+
      |   Qt C++ Control Center       |
      |   (Operator Dashboard)        |
      +---------------+---------------+
                      |
                      | HTTP REST API
                      | over TCP/IP
                      v
      +-------------------------------+
      |   Python Flask Simulator      |
      |   (Device State + Diagnostics)|
      +---------------+---------------+
                      |
                      v
            Simulated Rail Devices
        (Signals / Sensors / Switches)

                      |
                      v
               CSV Export Logs
                      |
          +-----------+-----------+
          |                       |
      Perl Log Parser       Excel VBA Formatter


Runtime Data Flow
-----------------

A typical system interaction proceeds as follows:

1. The operator refreshes device status in the Qt control center.
2. The Qt client sends a ``GET /devices`` request to the Flask backend.
3. The Flask server returns the current device states in JSON format.
4. The control center updates the dashboard table.

Diagnostics Workflow
--------------------

1. The operator selects a device.
2. The Qt client sends ``POST /devices/<device_id>/run_test``.
3. The backend runs the diagnostic routine.
4. The result is returned to the control center and logged.

Fault Injection Workflow
------------------------

1. The operator selects a device.
2. The Qt client sends a fault injection request.
3. The backend updates the device fault state.
4. The updated state is returned to the dashboard.

Component Responsibilities
--------------------------

Qt Control Center

* GUI dashboard
* device table visualization
* diagnostics control
* event logging
* CSV export

Flask Simulator

* stores device state
* executes diagnostic logic
* exposes REST API endpoints

Automation Tools

* Perl log parsing
* Excel VBA report formatting