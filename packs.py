from pathlib import Path


launcher_directory = Path(".modlab")
#launcher_directory = Path().home() / ".modlab"
packs_directory = launcher_directory / "packs"
minecraft_directory = launcher_directory / "minecraft"
packmapping_directory = launcher_directory / "packmapping"
cache_directory = launcher_directory / "cache"
packmapping_file = packmapping_directory / "packmapping.json"

directories = {
    "launcher": launcher_directory,
    "packmapping": packmapping_directory,
    "cache": cache_directory,
    "minecraft": minecraft_directory,
}

files = {
    "packmapping": packmapping_file,
}

def get_directory(name):
    return directories[name]
def get_file(name):
    return files[name]