from dataclasses import dataclass, asdict
from typing import Dict, Any


@dataclass
class RailDevice:
    device_id: str
    device_type: str
    state: str
    fault: str = "NONE"

    def to_dict(self) -> Dict[str, Any]:
        return {
            "id": self.device_id,
            "type": self.device_type,
            "state": self.state,
            "fault": self.fault,
        }


@dataclass
class SignalController(RailDevice):
    def __init__(self, device_id: str, state: str = "RED", fault: str = "NONE") -> None:
        super().__init__(device_id=device_id, device_type="signal", state=state, fault=fault)


@dataclass
class TrackSensor(RailDevice):
    def __init__(self, device_id: str, state: str = "CLEAR", fault: str = "NONE") -> None:
        super().__init__(device_id=device_id, device_type="track_sensor", state=state, fault=fault)


@dataclass
class SwitchController(RailDevice):
    def __init__(self, device_id: str, state: str = "STRAIGHT", fault: str = "NONE") -> None:
        super().__init__(device_id=device_id, device_type="switch", state=state, fault=fault)