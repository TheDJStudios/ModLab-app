# ModLab

A Minecraft launcher and modpack manager
Qt6 C++ frontend + Python (`minecraft-launcher-lib`) backend.

---

## Project Structure

```
modlab/                         ← this repo (Qt frontend)
├── CMakeLists.txt
├── src/
│   ├── main.cpp                ← entry point
│   ├── core/
│   │   ├── Pack.h              ← Pack struct, Loader/PackStatus enums
│   │   ├── PackManager.h/.cpp  ← reads packmapping.json, owns pack list
│   ├── ipc/
│   │   ├── BackendProcess.h/.cpp ← QProcess wrapper for main.py calls
│   └── ui/
│       ├── AeroStyle.h         ← full QSS stylesheet (all tokens here)
│       ├── MainWindow.h/.cpp   ← root window, wires everything together
│       ├── Sidebar.h/.cpp      ← left nav panel
│       ├── TopBar.h/.cpp       ← title + search + new pack button
│       ├── PackGrid.h/.cpp     ← scrollable grid of PackCards
│       ├── PackCard.h/.cpp     ← individual pack tile widget
│       ├── DetailPanel.h/.cpp  ← right panel showing selected pack info
│       └── NewPackDialog.h/.cpp ← modal dialog for creating a pack
├── scripts/
│   ├── build.sh                ← Linux/macOS build
│   ├── build.bat               ← Windows build
│   └── patch_backend.py        ← exact changes needed in main.py for IPC

main.py                         ← your existing Python backend (sibling dir)
requirements.txt
```

---

## Dependencies

### C++ / Qt
| Dep | Version | Install |
|-----|---------|---------|
| Qt6 Base | >= 6.4 | `apt install qt6-base-dev` / `brew install qt` / Qt Online Installer |
| CMake | >= 3.20 | `apt install cmake` / `brew install cmake` |
| GCC/Clang | C++17 | `apt install build-essential` / Xcode CLT |
| MSVC | 2019+ | Visual Studio (Windows) |

### Python (backend)
```
pip install -r requirements.txt
# requirements.txt: requests, minecraft-launcher-lib, pyinstaller
```

### Fonts (optional but recommended)
Download and place in `src/resources/fonts/`:
- [Space Mono](https://fonts.google.com/specimen/Space+Mono) — Regular + Bold
- [Exo 2](https://fonts.google.com/specimen/Exo+2) — Bold + Black (900)

If not bundled, Qt falls back to system fonts. The UI degrades gracefully.

---

## Build

### Step 1 — Patch the Python backend
Apply the changes in `scripts/patch_backend.py` to your `main.py`.
This adds `--action` CLI argument parsing and IPC-formatted stdout output.
The original `gui_main()` path is preserved for direct testing.

### Step 2 — Build

**Linux / macOS**
```bash
chmod +x scripts/build.sh
./scripts/build.sh
```

**Windows**
```bat
scripts\build.bat
```

**Manual (any platform)**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DMODLAB_BACKEND_PATH=/abs/path/to/main.py
cmake --build . --parallel
```

### Step 3 — Run
```bash
# Simple (uses python3 from PATH)
./build/ModLab

# With explicit python venv
MODLAB_PYTHON=/path/to/venv/bin/python3 ./build/ModLab

# Both paths overridable at runtime (no rebuild needed)
MODLAB_PYTHON=/usr/bin/python3 MODLAB_SCRIPT=/home/user/modlab-py/main.py ./build/ModLab
```

---

## IPC Protocol

The C++ frontend spawns `main.py` as a subprocess per operation.
Python writes to stdout; C++ reads line-by-line via `QProcess`.

```
C++ -> python3 main.py --action run     --pack skyblock
C++ -> python3 main.py --action install --pack skyblock
C++ -> python3 main.py --action delete  --pack skyblock
C++ -> python3 main.py --action make    --name Skyblock --version 1.21.4 --loader fabric

Python stdout ->
  STATUS:<packkey>:<message>
  PROGRESS:<packkey>:<0-100>
  DONE:<packkey>
  ERROR:<packkey>:<message>
```

---

## Fonts note

Qt's QSS `font-family` lookup is case-sensitive on Linux.
If Space Mono or Exo 2 aren't rendering, check:
```cpp
QFontDatabase::families()  // list what Qt can see
```
And ensure the `.ttf` files are either bundled in a Qt resource file (`.qrc`)
or installed as system fonts.
