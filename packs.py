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
    "packs": packs_directory,
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
        print(f"Initializing pack {name}...")
        self.packdir = packs_directory / f"{name}"
        self.packdata = packdata
        self.name = name
        self.version = version
        self.loader = loader
        if not self.packdir.exists():
            self.packdir.mkdir()
        else:
            self.packdir = packs_directory / f"{self.name}"

        print(f"Pack {name} initialized!")

    def make_pack(self):
        print("Making pack...")
        if not self.name in self.packdata:
            self.packdata[self.name] = {
                "name": self.name,
                "version": self.version,
                "loader": self.loader,
                "folder": str(self.packdir),
            }
    def find_pack(self):
        print("Finding pack...")
        if self.name in self.packdata:
            print("Pack found!")
        else:
            print("Pack not found!")
        return self.name in self.packdata





