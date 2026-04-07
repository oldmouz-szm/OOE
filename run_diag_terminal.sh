#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage:
  ./run_diag_terminal.sh \
    --bin ./main_test \
    --sys sys_server/c6288.sys \
    --txt sys_server/c6288.txt \
    --syspp sys_server/c6288.syspp \
    --obs obs/c6288_obs_1.txt \
    --obs-index 1 \
    --mode all

Optional:
  --max-solutions N   Max optimal solutions to enumerate (default: 20)

Output is printed to terminal only (no CSV).
Mode notes:
  --mode be|doe|ooe : run single mode
  --mode all        : run BE/DOE/OOE and print side-by-side sections
EOF
}

require_arg() {
  local name="$1"
  local value="$2"
  if [[ -z "$value" ]]; then
    echo "[ERROR] Missing required argument: $name" >&2
    usage
    exit 2
  fi
}

normalize_for_target() {
  local p="$1"
  if [[ "$p" == ./* ]]; then
    echo "${p:2}"
  else
    echo "$p"
  fi
}

BIN=""
SYS=""
TXT=""
SYSPP=""
OBS=""
OBS_INDEX=""
MODE="doe"
MAX_SOLUTIONS="20"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --bin) BIN="$2"; shift 2 ;;
    --sys) SYS="$2"; shift 2 ;;
    --txt) TXT="$2"; shift 2 ;;
    --syspp) SYSPP="$2"; shift 2 ;;
    --obs) OBS="$2"; shift 2 ;;
    --obs-index) OBS_INDEX="$2"; shift 2 ;;
    --mode) MODE="$2"; shift 2 ;;
    --max-solutions) MAX_SOLUTIONS="$2"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *)
      echo "[ERROR] Unknown argument: $1" >&2
      usage
      exit 2
      ;;
  esac
done

require_arg --bin "$BIN"
require_arg --sys "$SYS"
require_arg --txt "$TXT"
require_arg --syspp "$SYSPP"
require_arg --obs "$OBS"
require_arg --obs-index "$OBS_INDEX"

if [[ ! -x "$BIN" ]]; then
  echo "[ERROR] Binary not executable: $BIN" >&2
  exit 2
fi

if ! [[ "$OBS_INDEX" =~ ^[0-9]+$ ]]; then
  echo "[ERROR] Invalid --obs-index: $OBS_INDEX" >&2
  exit 2
fi

case "$MODE" in
  be|doe|ooe|all) ;;
  *)
    echo "[ERROR] Invalid --mode: $MODE (use be|doe|ooe|all)" >&2
    exit 2
    ;;
esac

SYS_N="$(normalize_for_target "$SYS")"
TXT_N="$(normalize_for_target "$TXT")"
SYSPP_N="$(normalize_for_target "$SYSPP")"
OBS_N="$(normalize_for_target "$OBS")"

python_exec="python3"
if [[ -x ".venv/bin/python" ]]; then
  python_exec=".venv/bin/python"
fi

run_one_mode() {
  local one_mode="$1"
  local wcnf_file="$2"

  rm -f remain_clauses.wcnf remain_clauses1.wcnf remain_clauses2.wcnf

  local enc_t0 enc_t1 enc_sec run_rc
  enc_t0="$(date +%s%3N)"
  set +e
  "$BIN" "$SYS_N" "$TXT_N" "$SYSPP_N" "$OBS_N" "$OBS_INDEX" "$one_mode" >/tmp/run_diag_terminal_${one_mode}.log 2>&1
  run_rc=$?
  set -e
  enc_t1="$(date +%s%3N)"
  enc_sec="$(awk -v a="$enc_t0" -v b="$enc_t1" 'BEGIN{printf "%.6f", (b-a)/1000.0}')"

  echo "=============================="
  echo "MODE=$one_mode"
  echo "RUN_EXIT_CODE=$run_rc"

  if [[ $run_rc -ne 0 ]]; then
    echo "[ERROR] Target binary failed in mode=$one_mode. Log: /tmp/run_diag_terminal_${one_mode}.log"
    tail -n 30 /tmp/run_diag_terminal_${one_mode}.log || true
    return 0
  fi

  if [[ ! -f "$wcnf_file" ]]; then
    echo "[ERROR] Expected WCNF not found: $wcnf_file"
    return 0
  fi

  echo "WCNF_FILE=$wcnf_file"
  "$python_exec" tools/print_diag_components.py \
    --sys "$SYS" \
    --wcnf "$wcnf_file" \
    --obs "$OBS" \
    --encoding-sec "$enc_sec" \
    --max-solutions "$MAX_SOLUTIONS"
}

(
if [[ "$MODE" == "all" ]]; then
  run_one_mode be remain_clauses.wcnf
  run_one_mode doe remain_clauses1.wcnf
  run_one_mode ooe remain_clauses2.wcnf
else
  case "$MODE" in
    be) run_one_mode be remain_clauses.wcnf ;;
    doe) run_one_mode doe remain_clauses1.wcnf ;;
    ooe) run_one_mode ooe remain_clauses2.wcnf ;;
  esac
fi
) | "$python_exec" tools/format_compare.py
