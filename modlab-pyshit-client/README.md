# ModLab

A Minecraft launcher and modpack manager.

Python (`PySide6`) frontend + Python (`minecraft-launcher-lib`) backend.

---

## Project Structure

```
modlab/
├── __main__.py                 <- `python3 -m modlab` entry point
├── app.py                      <- QApplication setup, fonts, stylesheet
├── core/
│   ├── pack.py                 <- Pack dataclass, Loader/PackStatus enums
│   └── pack_manager.py         <- reads packmapping.json, owns pack list
├── ipc/
│   └── backend_process.py      <- QProcess wrapper for main.py calls
├── ui/
│   ├── aero_style.py           <- full QSS stylesheet and color tokens
│   ├── main_window.py          <- root window, wires everything together
│   ├── sidebar.py              <- left nav panel
│   ├── top_bar.py              <- title + search + new pack button
│   ├── pack_grid.py            <- scrollable grid of PackCards
│   ├── pack_card.py            <- individual pack tile widget
│   ├── detail_panel.py         <- right panel showing selected pack info
│   └── new_pack_dialog.py      <- modal dialog for creating a pack
└── scripts/
    ├── build.sh                <- Linux/macOS run helper
    └── build.bat               <- Windows run helper

main.py                         <- existing Python backend, sibling of modlab/
requirements.txt
```

---

## Dependencies

```bash
pip install -r requirements.txt
```

Required Python packages:

- `PySide6`
- `requests`
- `minecraft-launcher-lib`
- `pyinstaller`

Optional fonts can still be bundled or installed system-wide:

- Space Mono Regular + Bold
- Exo 2 Bold + Black

If not present, Qt falls back to system fonts.

---

## Run

```bash
python3 -m modlab-pyshit-client
```

With explicit backend/runtime paths:

```bash
MODLAB_PYTHON=/path/to/venv/bin/python3 \
MODLAB_SCRIPT=/path/to/main.py \
python3 -m modlab-pyshit-client
```

The helper scripts simply run the Python frontend:

```bash
modlab-pyshit-client/scripts/build.sh
modlab-pyshit-client\scripts\build.bat
```

---

## IPC Protocol

The Python frontend spawns `main.py` as a subprocess per operation.
The backend writes to stdout; the frontend reads line-by-line via `QProcess`.

```
frontend -> python3 main.py --action run     --pack skyblock
frontend -> python3 main.py --action install --pack skyblock
frontend -> python3 main.py --action delete  --pack skyblock
frontend -> python3 main.py --action make    --name Skyblock --version 1.21.4 --loader fabric

Python stdout ->
  STATUS:<packkey>:<message>
  PROGRESS:<packkey>:<0-100>
  DONE:<packkey>
  ERROR:<packkey>:<message>
```

The frontend mirrors the backend's mapping file:

```text
~/.modlab/packmapping/packmapping.json
```
