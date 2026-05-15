#!/bin/python3

from pathlib import Path
import json
from datetime import datetime
import subprocess
import shutil
import tkinter as tk
import argparse

# Custom imports
import data
import packs
import gui


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
cached_packs = []

# code starts here
def initialize_launcher():
    global packdata
    if not launcher_directory.exists():
        launcher_directory.mkdir()
    if not packs_directory.exists():
        packs_directory.mkdir()
    if not packmapping_directory.exists():
        packmapping_directory.mkdir()
    if not packmapping_file.exists():
        packmapping_file.touch()
    if not packmapping_file.exists():
        print("Cannot find packmapping.json, Making file base")
        packmapping_file.touch(exist_ok=True)
        packmapping_file.write_text("{}")
        packdata = json.loads(packmapping_file.read_text(encoding="utf-8"))
    else:
        packdata = json.loads(packmapping_file.read_text(encoding="utf-8"))
def update_packmapping():
    packmapping_file.write_text(
        json.dumps(packdata, indent=4),
        encoding="utf-8"
    )



def initialize_packs():
    print("Initializing packs...")
    for pack_name, pack_info in packdata.items():
        currentpack = packs.Pack(pack_name, packdata, pack_info["version"], pack_info["loader"])
        cached_packs.append(currentpack)
    print(cached_packs)

def create_pack(name, version, loader):
    global packdata
    pack = packs.Pack(name, packdata, version, loader)
    pack.make_pack()
    packdata = pack.packdata
    cached_packs.append(pack)
    update_packmapping()


def make_modsfolder(directory):
    modsfolder = Path(directory) / "mods"
    if not modsfolder.exists():
        modsfolder.mkdir()






def main():
    testpack = packs.Pack("test", packdata, "1.20.1", "fabric")
    if not testpack.find_pack():
        testpack.make_pack()
    print(testpack.packdata)
    testpacktwo = packs.Pack("taketwo", packdata, "1.20.1", "fabric")
    if not testpacktwo.find_pack():
        testpacktwo.make_pack()
    print(testpacktwo.packdata)
    def mkmodpack():
        main_gui.popup()
        title = tk.Label(main_gui.popup, text="Make a modpack")
        title.grid(row=0, column=0, padx=5, pady=5)

        exitbtn = tk.Button(main_gui.popup, text="Exit", command=main_gui.popup.destroy)
        exitbtn.grid(row=0, column=1, padx=5, pady=5, sticky=tk.S)
    main_gui = gui.Gui()

    main_gui.mkbutton("Initpacks", "Initialize packs", 1, 0, command=initialize_packs)
    main_gui.mkbutton("Updatepacks", "Update packs", 2, 0, command=update_packmapping)
    main_gui.mkbutton("mkmp", "Create pack", 3, 0, command=mkmodpack)
    main_gui.mklabel("Title", "Packs", 0, )
    for i, (packname, packinfo) in enumerate(packdata.items()):
        main_gui.mklabel(packname, f"Pack: {packinfo['name']}, on {packinfo['version']} for {packinfo['loader']} in {packinfo['folder']}", 0, i + 1)
        main_gui.mkbutton("Mkmoddir", "Make mods folder", col=1, row=i+1, command=lambda folder=packinfo["folder"]: make_modsfolder(folder))
    main_gui.root.mainloop()


if __name__ == "__main__":
    initialize_launcher()
    initialize_packs()
    main()
