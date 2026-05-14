import json
import os
import subprocess
import tempfile
import unittest
from pathlib import Path

from PySide6.QtWidgets import QApplication, QFileDialog, QLabel

from modlab.core.pack import Loader, Pack, PackStatus
from modlab.core.pack_manager import PackManager
from modlab.ipc.backend_process import BackendProcess
from modlab.ui.detail_panel import DetailPanel
from modlab.ui.main_window import MainWindow


app = QApplication.instance() or QApplication([])


class ModLabFrontendTests(unittest.TestCase):
    def make_manager(self, root: Path) -> PackManager:
        manager = PackManager()
        manager.launcher_directory = root
        manager.packs_directory = root / "packs"
        manager.mapping_path = root / "packmapping" / "packmapping.json"
        return manager

    def write_pack(self, root: Path, key: str = "testpack") -> Path:
        pack_dir = root / "packs" / "TestPack_1.20.1"
        mods_dir = pack_dir / "mods"
        mods_dir.mkdir(parents=True)
        (mods_dir / "Sodium.jar").write_bytes(b"jar-one")
        (mods_dir / "Iris.jar").write_bytes(b"jar-two")
        mapping_dir = root / "packmapping"
        mapping_dir.mkdir(parents=True)
        (mapping_dir / "packmapping.json").write_text(
            json.dumps(
                {
                    key: {
                        "dir": "TestPack_1.20.1",
                        "version": "1.20.1",
                        "launch_version": "1.20.1",
                        "name": "Test Pack",
                        "loader": "fabric",
                    }
                }
            ),
            encoding="utf-8",
        )
        return pack_dir

    def test_pack_manager_discovers_mod_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.write_pack(root)
            manager = self.make_manager(root)
            manager.reload_from_disk()

            pack = manager.find_pack("testpack")
            self.assertIsNotNone(pack)
            self.assertEqual(pack.mod_count, 2)
            self.assertEqual([mod.name for mod in pack.mods], ["Iris.jar", "Sodium.jar"])
            self.assertEqual(pack.status, PackStatus.READY)

    def test_detail_panel_renders_real_mod_names(self):
        panel = DetailPanel()
        pack = Pack(
            key="testpack",
            name="Test Pack",
            mc_version="1.20.1",
            loader=Loader.FABRIC,
            status=PackStatus.READY,
            mod_count=1,
            mods=[],
        )
        panel.show_pack(pack)
        self.assertIn("No mods installed.", [label.text() for label in panel.findChildren(QLabel)])

        with tempfile.TemporaryDirectory() as tmp:
            mod_path = Path(tmp) / "Sodium.jar"
            mod_path.write_bytes(b"jar")
            pack.mods = [type("Mod", (), {"name": "Sodium.jar", "size_label": "3 B"})()]
            pack.mod_count = 1
            panel.show_pack(pack)
            self.assertIn("Sodium.jar", [label.text() for label in panel.findChildren(QLabel)])

    def test_add_mods_copies_jars_and_refreshes_count(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.write_pack(root)
            source = root / "ExampleMod.jar"
            source.write_bytes(b"new-jar")

            window = MainWindow("python3", str(Path("main.py").resolve()))
            window.pack_manager.launcher_directory = root
            window.pack_manager.packs_directory = root / "packs"
            window.pack_manager.mapping_path = root / "packmapping" / "packmapping.json"
            window.pack_manager.reload_from_disk()

            original = QFileDialog.getOpenFileNames
            QFileDialog.getOpenFileNames = staticmethod(lambda *args, **kwargs: ([str(source)], ""))
            try:
                window.on_add_mods_requested("testpack")
            finally:
                QFileDialog.getOpenFileNames = original
                window.close()

            pack = window.pack_manager.find_pack("testpack")
            self.assertTrue((root / "packs" / "TestPack_1.20.1" / "mods" / "ExampleMod.jar").exists())
            self.assertEqual(pack.mod_count, 3)

    def test_backend_process_parses_ipc_lines(self):
        process = BackendProcess("python3", "main.py")
        seen = {"status": [], "progress": [], "done": [], "error": []}
        process.status_line.connect(lambda key, msg: seen["status"].append((key, msg)))
        process.progress_update.connect(lambda key, pct: seen["progress"].append((key, pct)))
        process.operation_done.connect(lambda key: seen["done"].append(key))
        process.operation_error.connect(lambda key, msg: seen["error"].append((key, msg)))

        process._parse_line("STATUS:testpack:Installing Minecraft")
        process._parse_line("PROGRESS:testpack:42")
        process._parse_line("DONE:testpack")
        process._parse_line("ERROR:testpack:bad:thing")

        self.assertEqual(seen["status"], [("testpack", "Installing Minecraft")])
        self.assertEqual(seen["progress"], [("testpack", 42)])
        self.assertEqual(seen["done"], ["testpack"])
        self.assertEqual(seen["error"], [("testpack", "bad:thing")])

    def test_backend_duplicate_make_fails_before_install(self):
        with tempfile.TemporaryDirectory() as tmp:
            home = Path(tmp)
            mapping_dir = home / ".modlab" / "packmapping"
            mapping_dir.mkdir(parents=True)
            (mapping_dir / "packmapping.json").write_text(
                json.dumps(
                    {
                        "dupe": {
                            "dir": "Dupe_1.20.1",
                            "version": "1.20.1",
                            "launch_version": "1.20.1",
                            "name": "Dupe",
                            "loader": "vanilla",
                        }
                    }
                ),
                encoding="utf-8",
            )
            env = {**os.environ, "HOME": str(home)}
            result = subprocess.run(
                [
                    os.sys.executable,
                    "main.py",
                    "--action",
                    "make",
                    "--name",
                    "Dupe",
                    "--version",
                    "1.20.1",
                    "--loader",
                    "vanilla",
                ],
                cwd=Path(__file__).resolve().parents[1],
                env=env,
                text=True,
                capture_output=True,
                timeout=20,
            )
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("ERROR:dupe:A pack named Dupe already exists", result.stdout)

    def test_backend_make_pack_creates_mapping_and_mods_dir(self):
        import main

        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp) / ".modlab"
            old_paths = (
                main.launcher_directory,
                main.packs_directory,
                main.minecraft_directory,
                main.packmapping_directory,
                main.cache_directory,
                main.packmapping_file,
                main.packdata,
            )
            old_install_pack = main.install_pack
            old_list_loaders = main.minecraft_launcher_lib.mod_loader.list_mod_loader
            try:
                main.launcher_directory = root
                main.packs_directory = root / "packs"
                main.minecraft_directory = root / "minecraft"
                main.packmapping_directory = root / "packmapping"
                main.cache_directory = root / "cache"
                main.packmapping_file = main.packmapping_directory / "packmapping.json"
                main.packdata = {}
                main.install_pack = lambda _pack_key: None
                main.minecraft_launcher_lib.mod_loader.list_mod_loader = lambda: ["fabric", "forge"]

                main.initialize_launcher()
                main.make_pack("Local Test", "1.20.1", "fabric")

                mapping = json.loads(main.packmapping_file.read_text(encoding="utf-8"))
                self.assertIn("local test", mapping)
                self.assertEqual(mapping["local test"]["loader"], "fabric")
                self.assertTrue((root / "packs" / "Local Test_1.20.1").exists())
                self.assertTrue((root / "packs" / "Local Test_1.20.1" / "mods").exists())
            finally:
                (
                    main.launcher_directory,
                    main.packs_directory,
                    main.minecraft_directory,
                    main.packmapping_directory,
                    main.cache_directory,
                    main.packmapping_file,
                    main.packdata,
                ) = old_paths
                main.install_pack = old_install_pack
                main.minecraft_launcher_lib.mod_loader.list_mod_loader = old_list_loaders


if __name__ == "__main__":
    unittest.main()
