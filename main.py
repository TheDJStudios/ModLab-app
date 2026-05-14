#!/bin/python3

import pathlib
from pathlib import Path
import json
from datetime import datetime
import subprocess
import shutil
import tkinter as tk
import minecraft_launcher_lib
import argparse


# directories

#launcher_directory = Path(".modlab-pyshit-client")
launcher_directory = Path().home() / ".modlab-pyshit-client"
packs_directory = launcher_directory / "packs"
minecraft_directory = launcher_directory / "minecraft"
packmapping_directory = launcher_directory / "packmapping"
cache_directory = launcher_directory / "cache"
packmapping_file = packmapping_directory / "packmapping.json"

packdata = {}


current_max = 0

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
    global packdata
    if not launcher_directory.exists():
        print("Launcher directory does not exist, Making directory")
        launcher_directory.mkdir(parents=True, exist_ok=True)
    if not packs_directory.exists():
        print("Packs directory does not exist, Making directory")
        packs_directory.mkdir(parents=True, exist_ok=True)
    if not minecraft_directory.exists():
        print("Minecraft directory does not exist, Making directory")
        minecraft_directory.mkdir(parents=True, exist_ok=True)
    if not packmapping_directory.exists():
        print("Packmapping directory does not exist, Making directory")
        packmapping_directory.mkdir(parents=True, exist_ok=True)
    if not cache_directory.exists():
        print("Cache directory does not exist, Making directory")
        cache_directory.mkdir(parents=True, exist_ok=True)

    if not packmapping_file.exists():
        print("Cannot find packmapping.json, Making file base")
        packmapping_file.touch(exist_ok=True)
        packmapping_file.write_text("{}")
        packdata = json.loads(packmapping_file.read_text(encoding="utf-8"))
    else:
        packdata = json.loads(packmapping_file.read_text(encoding="utf-8"))


def save_packdata():
    packmapping_file.write_text(
        json.dumps(packdata, indent=4),
        encoding="utf-8"
    )


def get_pack(packname):
    pack_key = packname.lower()
    if pack_key not in packdata:
        raise KeyError(f"No pack named {packname}")
    return pack_key, packdata[pack_key]

CURRENT_PACK_KEY = ""

def set_status(status: str):
    print(f"STATUS:{CURRENT_PACK_KEY}:{status}", flush=True)

def set_progress(progress: int):
    if current_max != 0:
        percent = int((progress / current_max) * 100)
        print(f"PROGRESS:{CURRENT_PACK_KEY}:{percent}", flush=True)

def set_max(new_max: int):
    global current_max
    current_max = new_max


callback = {
    "setStatus": set_status,
    "setProgress": set_progress,
    "setMax": set_max
}


def install_java_runtime(version, mcdirectory, java_directory):
    runtime_info = minecraft_launcher_lib.runtime.get_version_runtime_information(version, mcdirectory)
    if runtime_info is None:
        return None

    runtime_name = runtime_info["name"]
    java_directory.mkdir(parents=True, exist_ok=True)
    minecraft_launcher_lib.runtime.install_jvm_runtime(
        runtime_name,
        java_directory,
        callback=callback,
    )
    return minecraft_launcher_lib.runtime.get_executable_path(runtime_name, java_directory)


def install_pack(packname):
    print("Installing Minecraft")
    pack_key, pack = get_pack(packname)
    version = pack["version"]
    loader = pack.get("loader", "vanilla").lower()
    mcdirectory = (packs_directory / pack["dir"]).resolve()
    java_directory = mcdirectory / "java"
    mods_directory = mcdirectory / "mods"
    mcdirectory.mkdir(parents=True, exist_ok=True)
    mods_directory.mkdir(parents=True, exist_ok=True)

    if loader == "vanilla":
        minecraft_launcher_lib.install.install_minecraft_version(
            version=version,
            minecraft_directory=mcdirectory,
            callback=callback,
        )
        launch_version = version
        java_executable = install_java_runtime(launch_version, mcdirectory, java_directory)
    else:
        minecraft_launcher_lib.install.install_minecraft_version(
            version=version,
            minecraft_directory=mcdirectory,
            callback=callback,
        )
        java_executable = install_java_runtime(version, mcdirectory, java_directory)
        mod_loader = minecraft_launcher_lib.mod_loader.get_mod_loader(loader)
        launch_version = mod_loader.install(
            version,
            mcdirectory,
            callback=callback,
            java=java_executable,
        )
        java_executable = install_java_runtime(launch_version, mcdirectory, java_directory) or java_executable

    root_runtime_directory = mcdirectory / "runtime"
    if root_runtime_directory.exists():
        shutil.rmtree(root_runtime_directory)
    pack["launch_version"] = launch_version
    pack["java_dir"] = str(java_directory)
    if java_executable is not None:
        pack["java_executable"] = java_executable
    save_packdata()
    print(f"Minecraft {launch_version} installed")


def install_minecraft(packname):
    install_pack(packname)


def run_minecraft(packname):
    print("Running Minecraft")
    packname, pack = get_pack(packname)
    mcdirectory = (packs_directory / pack["dir"]).resolve()
    java_directory = Path(pack.get("java_dir", mcdirectory / "java")).resolve()
    mcdirectory.mkdir(parents=True, exist_ok=True)

    options = minecraft_launcher_lib.utils.generate_test_options()
    options["gameDirectory"] = str(mcdirectory)
    options["launcherName"] = "ModLab"
    if pack.get("java_executable") and Path(pack["java_executable"]).exists():
        options["executablePath"] = pack["java_executable"]
    else:
        java_executable = install_java_runtime(pack.get("launch_version", pack["version"]), mcdirectory, java_directory)
        if java_executable is not None:
            pack["java_dir"] = str(java_directory)
            pack["java_executable"] = java_executable
            save_packdata()
            options["executablePath"] = java_executable

    command = minecraft_launcher_lib.command.get_minecraft_command(
        pack.get("launch_version", pack["version"]),
        mcdirectory,
        options,
    )

    subprocess.run(command, cwd=mcdirectory)

def clear_cache():
    if cache_directory.exists():
        shutil.rmtree(cache_directory)
    cache_directory.mkdir(parents=True, exist_ok=True)
    print("Cache directory cleared")


def make_pack(packname, minecraft_version, loader="vanilla"):
    pack_key = packname.lower()
    loader = loader.lower()

    if pack_key in packdata:
        raise ValueError(f"A pack named {packname} already exists")

    valid_loaders = ["vanilla", *minecraft_launcher_lib.mod_loader.list_mod_loader()]
    if loader not in valid_loaders:
        raise ValueError(f"Unknown loader: {loader}. Valid loaders: {', '.join(valid_loaders)}")

    pack_folder = f"{packname}_{minecraft_version}"
    pack_path = packs_directory / pack_folder

    if pack_path.exists():
        pack_folder = f"{packname}_{minecraft_version}_{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}"
        pack_path = packs_directory / pack_folder
        pack_path.mkdir()
    else:
        pack_path.mkdir()
    (pack_path / "mods").mkdir(exist_ok=True)

    packdata[pack_key] = {
        "dir": pack_folder,
        "version": minecraft_version,
        "launch_version": minecraft_version,
        "name": packname,
        "loader": loader,
    }
    save_packdata()
    install_pack(pack_key)

def list_packs():
    for pack in packdata:
        print(f"- {pack}")
def delete_pack(packname):
    pack_key = packname.lower()
    if pack_key not in packdata:
        print(f"No pack named {packname}")
        return

    deldir = packdata[pack_key]["dir"]
    remove_dir = packs_directory / deldir
    if remove_dir.exists():
        print(remove_dir)
        shutil.rmtree(remove_dir)
    else:
        print(f"No directory was found for {packname}'s listed directory: {remove_dir}")

    del packdata[pack_key]
    save_packdata()
    print(f"{packname} removed")


def main():
    while True:
        print(f"Welcome to ModLab Launcher\nSupported actions:\n\nPack (Sub actions available)\nExit - Fully exit ModLab Launcher")
        func = input("Home >> ")
        if func.lower() == "pack":
            print(f"make\ndelete\ninstall\nrun\nlist")
            func = input("Pack >> ")
            if func.lower() == "make":
                print("To make your pack, We need a name.\ntype anything alphanumeric here (abcdefghijklmnopqrstuvwxyz 1234567890)")
                pkname = input("Pack Name >> ")
                print("Awesome! Now we need a Minecraft version. Minecraft versions available in minecraft > versions In the main menu!")
                pkversion = input("Pack Version >> ")
                print("Almost there, We need to know if this pack is going to be modded. please type 'True' or 'False'")
                pkloader = "vanilla"
                if input("Pack modded? >> ").strip().lower() in {"true", "yes", "y"}:
                    print("Finally, we need to know what loader its going to be. Please type 'Fabric' or 'Forge'")
                    pkloader = input("Pack Loader >> ")
                print(f"Awesome! We are currently making pack {pkname} for Minecraft {pkversion}.")
                make_pack(pkname, pkversion, pkloader)

            elif func.lower() == "list":
                list_packs()
            elif func.lower() == "delete":
                pkname = input("Pack name >> ")
                delete_pack(pkname)
            elif func.lower() == "install":
                pkname = input("install to pack >> ")
                install_minecraft(pkname)
            elif func.lower() == "run":
                pkname = input("Run pack with name >> ")
                run_minecraft(pkname)
        elif func.lower() == "exit":
            exit()


def gui_main():
    root = tk.Tk()
    root.title("ModLab")


    tk.Label(root, text="Pack Name").grid(row=0, column=0, padx=8, pady=6, sticky="w")
    tk.Label(root, text="Actions").grid(row=0, column=1, columnspan=3, padx=8, pady=6, sticky="w")

    def popup_create():
        popup = tk.Toplevel(root)
        popup.title("Create Minecraft pack")
        popup.geometry("300x270")
        popup.grab_set()

        label = tk.Label(popup, text="Create Minecraft pack")
        label.pack(pady=10)

        tk.Label(popup, text="Enter Name:").pack(pady=2)
        name_entry = tk.Entry(popup)
        name_entry.pack(pady=2)

        tk.Label(popup, text="Select version").pack(pady=2)
        version_entry = tk.Entry(popup)
        version_entry.pack(pady=2)

        tk.Label(popup, text="Select Loader:").pack(pady=2)
        loader_var = tk.StringVar(popup)
        loader_var.set("vanilla")
        dropdown_1 = tk.OptionMenu(popup, loader_var, "vanilla", "fabric", "forge")
        dropdown_1.pack(pady=2)


        def on_submit():
            name = name_entry.get()
            version = version_entry.get()
            loader = loader_var.get()

            make_pack(name, version, loader)

            popup.destroy()
            refresh()
        submit_button = tk.Button(popup, text="Submit", command=on_submit)
        submit_button.pack(pady=10)

    create_button = tk.Button(root, text="Create Minecraft pack", command=popup_create)
    create_button.grid(row=1, column=0, columnspan=4, padx=8, pady=8, sticky="w")

    def refresh():
        root.destroy()
        gui_main()

    for row, pack_key in enumerate(packdata, start=2):
        pack_name = packdata[pack_key].get("name", pack_key)
        tk.Label(root, text=pack_name).grid(row=row, column=0, padx=8, pady=4, sticky="w")
        tk.Button(root, text="Run", command=lambda name=pack_key: run_minecraft(name)).grid(row=row, column=1, padx=4, pady=4)
        tk.Button(root, text="Install", command=lambda name=pack_key: install_minecraft(name)).grid(row=row, column=2, padx=4, pady=4)
        tk.Button(root, text="Delete", command=lambda name=pack_key: (delete_pack(name), refresh())).grid(row=row, column=3, padx=4, pady=4)

    root.mainloop()


if __name__ == "__main__":
    import sys
    # If any --action flag is present, run headless CLI mode for Qt IPC
    if "--action" in sys.argv:
        cli_main()
    else:
        # Original GUI entrypoint (kept for direct testing)
        initialize_launcher()
        gui_main()
