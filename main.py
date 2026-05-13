import pathlib
from pathlib import Path
import os
import random
import json
from datetime import datetime
import subprocess


# directories

launcher_directory = Path(".modlab")
packs_directory = launcher_directory / "packs"
minecraft_directory = launcher_directory / "minecraft"
packmapping_directory = launcher_directory / "packmapping"
cache_directory = launcher_directory / "cache"
packmapping_file = packmapping_directory / "packmapping.json"

packdata = {}

# code starts here
def initialize_launcher():
    global packdata
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
    if not cache_directory.exists():
        print("Cache directory does not exist, Making directory")
        cache_directory.mkdir()

    if not packmapping_file.exists():
        print("Cannot find packmapping.json, Making file base")
        packmapping_file.touch(exist_ok=True)
        packmapping_file.write_text("{}")
        packdata = json.loads(packmapping_file.read_text(encoding="utf-8"))
    else:
        packdata = json.loads(packmapping_file.read_text(encoding="utf-8"))


def clear_cache():
    cache_directory.rmdir()
    cache_directory.mkdir()
    print("Cache directory cleared")


def make_pack(packname, minecraft_version):
    pack_folder = f"{packname}_{minecraft_version}"
    pack_path = packs_directory / pack_folder

    if pack_path.exists():
        pack_folder = f"{packname}_{minecraft_version}_{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}"
        pack_path = packs_directory / pack_folder
        pack_path.mkdir()
    else:
        pack_path.mkdir()

    packdata[f"{packname.lower()}"] = {
        "dir": pack_folder,
        "version": minecraft_version,
        "name": packname,
    }
    packmapping_file.write_text(
        json.dumps(packdata, indent=4),
        encoding="utf-8"
    )

def list_packs():
    for pack in packdata:
        print(f"- {pack}")
def delete_pack(packname):
    if packname.lower() in packdata:
        deldir = packdata[packname]["dir"]
        if Path(deldir).exists():
            print(deldir)
            rmvedir = packs_directory / deldir
            print(rmvedir)
            rmvedir.rmdir()
        else:
            print(f"No directory was found for {packname}'s listed directory")
        del packdata[packname]
        packmapping_file.write_text(
            json.dumps(packdata, indent=4),
            encoding="utf-8"
        )
        print(f"{packname} removed")

def main():
    while True:
        print(f"Welcome to ModLab Launcher\nSupported actions:\n\nPack (Sub actions available)")
        func = input("Home >> ")
        if func.lower() == "pack":
            print(f"make\ndelete\nlist")
            func = input("Pack >> ")
            if func.lower() == "make":
                print("To make your pack, We need a name.\ntype anything alphanumeric here (abcdefghijklmnopqrstuvwxyz 1234567890)")
                pkname = input("Pack Name >> ")
                print("Awesome! Now we need a Minecraft version. Minecraft versions available in minecraft > versions In the main menu!")
                pkversion = input("Pack Version >> ")
                print(f"Awesome! We are currently making pack {pkname} for Minecraft {pkversion}.")
                make_pack(pkname, pkversion)

            elif func.lower() == "list":
                list_packs()
            elif func.lower() == "delete":
                pkname = input("Pack name >> ")
                delete_pack(pkname)
        elif func.lower() == "exit":
            exit()


if __name__ == "__main__":
    initialize_launcher()
    main()
