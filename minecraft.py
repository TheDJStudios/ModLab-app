import pathlib
import minecraft_launcher_lib
import json
import modlab_downloads.java

status = None
progress = None
max = None

def set_status(new_status):
    global status
    status = new_status
def set_progress(new_progress):
    global progress
    global max
    progress = f"{new_progress}/{max}"
def set_max(new_max):
    global max
    max = new_max


callback = {
    "setStatus": set_status,
    "setProgress": set_progress,
    "setMax": set_max
}


def get_callback():
    return callback
def get_status():
    return status
def get_progress():
    return progress
def get_max():
    return max