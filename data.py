import pathlib
import json


class Status:
    def __init__(self):
        self.installstatus = ""
        self.progress = 0
        self.max_progress = 0

    def set_status(self, new_status):
        self.installstatus = new_status

    def set_progress(self, new_progress):
        self.progress = new_progress

    def set_max(self, new_max):
        self.max_progress = new_max

    def get_callback(self):
        return {
            "setStatus": self.set_status,
            "setProgress": self.set_progress,
            "setMax": self.set_max,
        }

    def get_status(self):
        return self.installstatus

    def get_progress(self):
        return self.progress

    def get_max(self):
        return self.max_progress


status = Status()

callback = {
    "setStatus": status.set_status,
    "setProgress": status.set_progress,
    "setMax": status.set_max
}

