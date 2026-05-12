import pathlib
from pathlib import Path
import os
import random

def initialize_launcher():
    launcher_directory = Path(".modlab")
    if not launcher_directory.exists():
        print("Launcher directory does not exist, Making directory")
        launcher_directory.mkdir()





if __name__ == "__main__":
    initialize_launcher()