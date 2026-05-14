import minecraft_launcher_lib



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

def install_minecraft(mcdirectory, version, loader = "vanilla", java_directory = None):
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
        return java_executable, launch_version