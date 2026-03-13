from __future__ import annotations

from datetime import datetime
from pathlib import Path
from typing import Dict, List

from flask import Flask, jsonify, request

from device_models import SignalController, SwitchController, TrackSensor, RailDevice
from diagnostics import (
    get_valid_faults,
    get_valid_states,
    run_diagnostic_test,
    validate_fault,
    validate_state,
)

app = Flask(__name__)

LOG_DIR = Path(__file__).resolve().parent.parent / "logs" / "exports"
LOG_DIR.mkdir(parents=True, exist_ok=True)
LOG_FILE = LOG_DIR / "simulator_events.csv"


def append_log(device_id: str, action: str, result: str, details: str) -> None:
    is_new_file = not LOG_FILE.exists()

    with LOG_FILE.open("a", encoding="utf-8") as log_file:
        if is_new_file:
            log_file.write("timestamp,device_id,action,result,details\n")

        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        safe_details = details.replace(",", ";")
        log_file.write(f"{timestamp},{device_id},{action},{result},{safe_details}\n")


devices: Dict[str, RailDevice] = {
    "SIG-01": SignalController("SIG-01", "GREEN"),
    "SIG-02": SignalController("SIG-02", "RED"),
    "TRK-01": TrackSensor("TRK-01", "CLEAR"),
    "TRK-02": TrackSensor("TRK-02", "OCCUPIED"),
    "SWT-01": SwitchController("SWT-01", "STRAIGHT"),
    "SWT-02": SwitchController("SWT-02", "DIVERGING"),
}


@app.route("/")
def home():
    return jsonify(
        {
            "message": "TrackGuard simulator running",
            "endpoints": [
                "GET /devices",
                "GET /devices/<device_id>",
                "POST /devices/<device_id>/run_test",
                "POST /devices/<device_id>/inject_fault",
                "POST /devices/<device_id>/clear_fault",
                "POST /devices/<device_id>/set_state",
                "GET /device_types/<device_type>/metadata",
            ],
        }
    )


@app.route("/devices", methods=["GET"])
def get_devices():
    return jsonify([device.to_dict() for device in devices.values()])


@app.route("/devices/<device_id>", methods=["GET"])
def get_device(device_id: str):
    device = devices.get(device_id)
    if device is None:
        return jsonify({"error": f"Device {device_id} not found"}), 404

    return jsonify(device.to_dict())


@app.route("/devices/<device_id>/run_test", methods=["POST"])
def run_test(device_id: str):
    device = devices.get(device_id)
    if device is None:
        return jsonify({"error": f"Device {device_id} not found"}), 404

    test_result = run_diagnostic_test(device)
    append_log(
        device_id=device.device_id,
        action="run_test",
        result=test_result["result"],
        details=test_result["details"],
    )

    response = {
        "device": device.to_dict(),
        "diagnostic": test_result,
    }
    return jsonify(response)


@app.route("/devices/<device_id>/inject_fault", methods=["POST"])
def inject_fault(device_id: str):
    device = devices.get(device_id)
    if device is None:
        return jsonify({"error": f"Device {device_id} not found"}), 404

    payload = request.get_json(silent=True) or {}
    fault = payload.get("fault")

    if not fault:
        return jsonify({"error": "Request body must include 'fault'"}), 400

    if not validate_fault(device.device_type, fault):
        return (
            jsonify(
                {
                    "error": f"Invalid fault '{fault}' for device type '{device.device_type}'",
                    "valid_faults": get_valid_faults(device.device_type),
                }
            ),
            400,
        )

    device.fault = fault
    details = f"Fault injected on {device.device_id}: {fault}"
    append_log(device.device_id, "inject_fault", "OK", details)

    return jsonify(
        {
            "message": details,
            "device": device.to_dict(),
        }
    )


@app.route("/devices/<device_id>/clear_fault", methods=["POST"])
def clear_fault(device_id: str):
    device = devices.get(device_id)
    if device is None:
        return jsonify({"error": f"Device {device_id} not found"}), 404

    device.fault = "NONE"
    details = f"Fault cleared on {device.device_id}"
    append_log(device.device_id, "clear_fault", "OK", details)

    return jsonify(
        {
            "message": details,
            "device": device.to_dict(),
        }
    )


@app.route("/devices/<device_id>/set_state", methods=["POST"])
def set_state(device_id: str):
    device = devices.get(device_id)
    if device is None:
        return jsonify({"error": f"Device {device_id} not found"}), 404

    payload = request.get_json(silent=True) or {}
    new_state = payload.get("state")

    if not new_state:
        return jsonify({"error": "Request body must include 'state'"}), 400

    if not validate_state(device.device_type, new_state):
        return (
            jsonify(
                {
                    "error": f"Invalid state '{new_state}' for device type '{device.device_type}'",
                    "valid_states": get_valid_states(device.device_type),
                }
            ),
            400,
        )

    old_state = device.state
    device.state = new_state
    details = f"State changed for {device.device_id}: {old_state} -> {new_state}"
    append_log(device.device_id, "set_state", "OK", details)

    return jsonify(
        {
            "message": details,
            "device": device.to_dict(),
        }
    )


@app.route("/device_types/<device_type>/metadata", methods=["GET"])
def get_device_type_metadata(device_type: str):
    sample_type_map = {
        "signal": "signal",
        "track_sensor": "track_sensor",
        "switch": "switch",
    }

    normalized_type = sample_type_map.get(device_type)
    if normalized_type is None:
        return jsonify({"error": f"Unknown device type: {device_type}"}), 404

    return jsonify(
        {
            "device_type": normalized_type,
            "valid_states": get_valid_states(normalized_type),
            "valid_faults": get_valid_faults(normalized_type),
        }
    )


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=5000, debug=True)