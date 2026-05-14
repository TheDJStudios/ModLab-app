import json
import pathlib


def save_packdata(packmapping_file, packdata):
    packmapping_file.write_text(
        json.dumps(packdata, indent=4),
        encoding="utf-8"
    )


def get_pack(packname, packdata):
    pack_key = packname.lower()
    if pack_key not in packdata:
        raise KeyError(f"No pack named {packname}")
    return pack_key, packdata[pack_key]