#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

BIN="./ooe_main"
SYS="./sys_server/c17.sys"
TMP_CNF="./sys_server/c17.wcnf"
SYSPP="./sys_server/c17.syspp"
OBS="./iscas85/c17_iscas85.obs"
OBS_INDEX="1"

echo "[1/2] Building ooe_main..."
g++ -std=c++11 -O2 -o "$BIN" pms.cpp

echo "[2/2] Running smoke case (c17, obs=1)..."
"$BIN" "$SYS" "$TMP_CNF" "$SYSPP" "$OBS" "$OBS_INDEX"

echo "Done. Generated files:"
ls -l remain_clauses.wcnf remain_clauses1.wcnf remain_clauses2.wcnf 2>/dev/null || true
