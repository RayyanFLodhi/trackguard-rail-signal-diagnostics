Architecture
============

TrackGuard uses a layered architecture to simulate a rail signal control and diagnostics platform.

High-Level Components
---------------------

1. Qt C++ Control Center
   - Displays rail devices and signal states
   - Allows diagnostics and fault injection
   - Exports device data for offline analysis

2. Python Simulator Backend
   - Simulates signal controllers, track sensors, and switches
   - Exposes a REST API over TCP/IP
   - Supports diagnostics and fault injection

3. CSV Logging and Reporting
   - Device state and test results are exported to CSV
   - Perl scripts parse logs for summaries
   - VBA can later format Excel reports

Architecture Flow
-----------------

::

   Qt Control Center  <---- REST API over TCP/IP ---->  Python Rail Device Simulator
            |
            v
       CSV Export Logs
            |
            +--> Perl Log Parser
            +--> VBA Excel Formatter