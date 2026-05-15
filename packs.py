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

class Pack:
    def __init__(self, name, packdata, version,loader = "vanilla"):
        self.packdir = packs_directory / f"{name}"
        self.packdata = packdata
        self.name = name
        self.version = version
        self.loader = loader
        if not self.packdir.exists():
            self.packdir.mkdir()
        else:
            self.packdir = packs_directory / f"{self.name}"

    def make_pack(self):
        if not self.packdata[self.name].exists():
            self.packdata[self.name] = {
                "name": self.name,
                ""
            }





