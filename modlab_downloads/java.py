import minecraft_launcher_lib
from pathlib import Path
import data

modlab_minecrat_status = data.Status()

callback = modlab_minecrat_status.get_callback()

def download(version, packdir):
    javadir = packdir / "java"
    runtime_info = minecraft_launcher_lib.runtime.get_version_runtime_information(
        version,
        packdir
    )
    if runtime_info is None:
        return None
    runtime_name = runtime_info["name"]
    javadir.mkdir(parents=True, exist_ok=True)

    minecraft_launcher_lib.runtime.install_jvm_runtime(
        runtime_name,
        javadir,
    )

