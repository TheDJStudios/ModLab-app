#!/usr/bin/env bash
# =============================================================================
# ModLab build script
# Requires: Qt6, CMake >= 3.20, a C++17 compiler (GCC/Clang/MSVC)
# =============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR"
BUILD_DIR="$ROOT/build"
BACKEND_PATH="$ROOT/../main.py"   # adjust if your layout differs

echo "==> ModLab build"
echo "    Root:    $ROOT"
echo "    Build:   $BUILD_DIR"
echo "    Backend: $BACKEND_PATH"
echo ""

# ---- Detect Qt6 ----
# Qt6 installed via:
#   Linux:   sudo apt install qt6-base-dev  OR  brew install qt  (macOS)
#   Windows: Qt Online Installer → set Qt6_DIR manually below
#
# If cmake can't find Qt6 automatically, set this:
#   export Qt6_DIR=/path/to/Qt/6.x.x/gcc_64/lib/cmake/Qt6

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake "$ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DMODLAB_BACKEND_PATH="$BACKEND_PATH"

cmake --build . --parallel "$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"

echo ""
echo "==> Build complete: $BUILD_DIR/ModLab"
echo ""
echo "==> Run with:"
echo "    cd $BUILD_DIR && ./ModLab"
echo ""
echo "    Or override paths at runtime:"
echo "    MODLAB_PYTHON=/path/to/venv/bin/python3 \\"
echo "    MODLAB_SCRIPT=/path/to/main.py \\"
echo "    ./ModLab"
