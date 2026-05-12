import pathlib
from pathlib import Path
import os
import random
import json
from datetime import datetime
import subprocess

launcher_directory = Path(".modlab")
packs_directory = launcher_directory / "packs"
minecraft_directory = launcher_directory / "minecraft"
packmapping_directory = launcher_directory / "packmapping"
packmapping_file = packmapping_directory / "packmapping.json"

def initialize_launcher():
    if not launcher_directory.exists():
        print("Launcher directory does not exist, Making directory")
        launcher_directory.mkdir()
    if not packs_directory.exists():
        print("Packs directory does not exist, Making directory")
        packs_directory.mkdir()
    if not minecraft_directory.exists():
        print("Minecraft directory does not exist, Making directory")
        minecraft_directory.mkdir()
    if not packmapping_directory.exists():
        print("Packmapping directory does not exist, Making directory")
        packmapping_directory.mkdir()

    if not packmapping_file.exists():
        print("Cannot find packmapping.json, Making file base")
        packmapping_file.touch(exist_ok=True)

def make_pack(packname, minecraft_version):
    pack_folder = f"{packname}_{minecraft_version}"
    pack_path = packs_directory / pack_folder

    if pack_path.exists():
        pack_folder = f"{packname}_{minecraft_version}_{datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}"
        pack_path = packs_directory / pack_folder
        pack_path.mkdir()
    else:
        pack_path.mkdir()

def main():
    pkname = input("name >>")
    pkversion = input("version >>")
    make_pack(pkname, pkversion)


if __name__ == "__main__":
    initialize_launcher()
    main()