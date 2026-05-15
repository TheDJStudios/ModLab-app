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



def cli_main():
    global CURRENT_PACK_KEY
    parser = argparse.ArgumentParser(description="ModLab backend")
    parser.add_argument("--action",  required=True,
                        choices=["run","install","delete","make"])
    parser.add_argument("--pack",    default="")
    parser.add_argument("--name",    default="")
    parser.add_argument("--version", default="")
    parser.add_argument("--loader",  default="vanilla")
    args = parser.parse_args()

    initialize_launcher()

    try:
        if args.action == "run":
            CURRENT_PACK_KEY = args.pack
            run_minecraft(args.pack)

        elif args.action == "install":
            CURRENT_PACK_KEY = args.pack
            install_minecraft(args.pack)

        elif args.action == "delete":
            CURRENT_PACK_KEY = args.pack
            delete_pack(args.pack)

        elif args.action == "make":
            CURRENT_PACK_KEY = args.name.lower()
            make_pack(args.name, args.version, args.loader)

        print(f"DONE:{CURRENT_PACK_KEY}", flush=True)

    except Exception as e:
        print(f"ERROR:{CURRENT_PACK_KEY}:{e}", flush=True)
        raise SystemExit(1)

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
