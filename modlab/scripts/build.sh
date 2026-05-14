#!/usr/bin/env bash
# =============================================================================
# ModLab Python frontend launcher
# Requires: Python 3.10+ and dependencies from requirements.txt
# =============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$ROOT"

echo "==> ModLab Python frontend"
echo "    Root:    $ROOT"
echo "    Backend: ${MODLAB_SCRIPT:-$ROOT/main.py}"
echo ""
echo "==> Run with:"
echo "    python3 -m modlab"
echo ""

python3 -m modlab
