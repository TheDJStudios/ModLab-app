from PySide6.QtCore import QObject, QProcess, Signal


class BackendProcess(QObject):
    """Runs one backend subprocess per operation and parses ModLab IPC lines."""

    status_line = Signal(str, str)
    progress_update = Signal(str, int)
    operation_done = Signal(str)
    operation_error = Signal(str, str)
    raw_output = Signal(str)

    def __init__(self, python_path: str, script_path: str, parent=None):
        super().__init__(parent)
        self.python_path = python_path
        self.script_path = script_path
        self.process: QProcess | None = None
        self.current_pack = ""

    def is_busy(self) -> bool:
        return self.process is not None and self.process.state() != QProcess.NotRunning

    def run_pack(self, pack_key: str) -> None:
        self.current_pack = pack_key
        self._start_process(["--action", "run", "--pack", pack_key])

    def install_pack(self, pack_key: str) -> None:
        self.current_pack = pack_key
        self._start_process(["--action", "install", "--pack", pack_key])

    def delete_pack(self, pack_key: str) -> None:
        self.current_pack = pack_key
        self._start_process(["--action", "delete", "--pack", pack_key])

    def make_pack(self, name: str, version: str, loader: str) -> None:
        self.current_pack = name.lower()
        self._start_process(
            ["--action", "make", "--name", name, "--version", version, "--loader", loader]
        )

    def _start_process(self, args: list[str]) -> None:
        if self.process is not None:
            self.process.kill()
            self.process.deleteLater()

        self.process = QProcess(self)
        self.process.readyReadStandardOutput.connect(self._on_ready_read)
        self.process.finished.connect(self._on_finished)
        self.process.start(self.python_path, [self.script_path, *args])

    def _on_ready_read(self) -> None:
        if self.process is None:
            return
        while self.process.canReadLine():
            line = bytes(self.process.readLine()).decode("utf-8", errors="replace").strip()
            self.raw_output.emit(line)
            self._parse_line(line)

    def _parse_line(self, line: str) -> None:
        parts = line.split(":")
        if len(parts) < 2:
            return

        tag = parts[0].upper()
        pack = parts[1]
        if tag == "STATUS" and len(parts) >= 3:
            self.status_line.emit(pack, ":".join(parts[2:]))
        elif tag == "PROGRESS" and len(parts) >= 3:
            try:
                self.progress_update.emit(pack, int(parts[2]))
            except ValueError:
                pass
        elif tag == "DONE":
            self.operation_done.emit(pack)
        elif tag == "ERROR" and len(parts) >= 3:
            self.operation_error.emit(pack, ":".join(parts[2:]))

    def _on_finished(self, exit_code: int, _exit_status: QProcess.ExitStatus) -> None:
        if exit_code != 0 and self.current_pack:
            self.operation_error.emit(self.current_pack, f"Process exited with code {exit_code}")
        if self.process is not None:
            self.process.deleteLater()
            self.process = None
