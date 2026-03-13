from typing import Dict

from device_models import RailDevice


VALID_STATES = {
    "signal": {"RED", "YELLOW", "GREEN"},
    "track_sensor": {"CLEAR", "OCCUPIED"},
    "switch": {"STRAIGHT", "DIVERGING"},
}

VALID_FAULTS = {
    "signal": {"NONE", "LAMP_FAILURE", "COMMUNICATION_TIMEOUT"},
    "track_sensor": {"NONE", "STUCK_OCCUPIED", "NO_RESPONSE"},
    "switch": {"NONE", "FAILED_TO_MOVE", "POSITION_MISMATCH"},
}


def run_diagnostic_test(device: RailDevice) -> Dict[str, str]:
    if device.fault != "NONE":
        return {
            "result": "FAIL",
            "details": f"{device.device_type} {device.device_id} failed diagnostics due to fault: {device.fault}",
        }

    if device.state not in VALID_STATES.get(device.device_type, set()):
        return {
            "result": "FAIL",
            "details": f"{device.device_type} {device.device_id} has invalid state: {device.state}",
        }

    return {
        "result": "PASS",
        "details": f"{device.device_type} {device.device_id} passed diagnostics successfully",
    }


def validate_state(device_type: str, state: str) -> bool:
    return state in VALID_STATES.get(device_type, set())


def validate_fault(device_type: str, fault: str) -> bool:
    return fault in VALID_FAULTS.get(device_type, set())


def get_valid_states(device_type: str) -> list[str]:
    return sorted(list(VALID_STATES.get(device_type, set())))


def get_valid_faults(device_type: str) -> list[str]:
    return sorted(list(VALID_FAULTS.get(device_type, set())))