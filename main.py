#!/bin/python3

from pathlib import Path
import json
from datetime import datetime
import subprocess
import shutil
import tkinter as tk
import argparse

# Custom imports
import minecraft
import packs


# temp imports
import minecraft_launcher_lib

# directories

launcher_directory = packs.get_directory("launcher")
packs_directory = packs.get_directory("packs")
minecraft_directory = packs.get_directory("minecraft")
packmapping_directory = packs.get_directory("packmapping")
cache_directory = packs.get_directory("cache")
packmapping_file = packs.get_file("packmapping")

packdata = {}

# code starts here
def initialize_launcher():
    if not launcher_directory.exists():
        launcher_directory.mkdir()
    if not packs_directory.exists():
        packs_directory.mkdir()

def main():
    for directory in packs.directories:
        print(directory)


if __name__ == "__main__":
    initialize_launcher()
    main()
