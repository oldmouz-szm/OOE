#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

if [[ $# -lt 4 ]]; then
  echo "Usage: $0 <sys_file> <syspp_file> <obs_file> <tmp_cnf_file> [max_obs] [encode_timeout_sec] [solve_timeout_sec] [out_prefix]"
  exit 1
fi

SYS_FILE="$1"
SYSPP_FILE="$2"
OBS_FILE="$3"
TMP_CNF_FILE="$4"
MAX_OBS="${5:-}"
ENCODE_TIMEOUT="${6:-30}"
SOLVE_TIMEOUT="${7:-30}"
OUT_PREFIX="${8:-solver_cmp}"

BIN="./ooe_main"
PYTHON_BIN="${PYTHON_BIN:-/home/ubuntu/.pyenv/versions/3.13.1/bin/python}"
SOLVER_SCRIPT="./solve_wcnf_rc2.py"
SKIP_BUILD="${SKIP_BUILD:-0}"

if [[ ! -f "$SYS_FILE" || ! -f "$SYSPP_FILE" || ! -f "$OBS_FILE" || ! -f "$SOLVER_SCRIPT" ]]; then
  echo "Input file not found."
  exit 1
fi

if [[ -z "$MAX_OBS" ]]; then
  MAX_OBS="$(awk -F'[(),]' '/^\(/ {if(($3+0)>m) m=$3+0} END {print m+0}' "$OBS_FILE")"
fi

if [[ "$MAX_OBS" -le 0 ]]; then
  echo "Cannot infer max observation index from $OBS_FILE"
  exit 1
fi

if [[ "$SKIP_BUILD" == "1" ]]; then
  if [[ ! -x "$BIN" ]]; then
    echo "SKIP_BUILD=1 but $BIN is missing or not executable"
    exit 1
  fi
  echo "Skipping build (SKIP_BUILD=1), using existing $BIN"
else
  echo "Building $BIN ..."
  g++ -std=c++11 -O2 -o "$BIN" pms.cpp
fi

TS="$(date +%Y%m%d_%H%M%S)"
CSV_FILE="${OUT_PREFIX}_${TS}.csv"
SUMMARY_FILE="${OUT_PREFIX}_${TS}_summary.txt"

echo "obs,encode_exit,solve_be_exit,solve_doe_exit,solve_ooe_exit,be_vars,be_clauses,doe_vars,doe_clauses,ooe_vars,ooe_clauses,be_solve_ms,doe_solve_ms,ooe_solve_ms,be_cost,doe_cost,ooe_cost,encode_elapsed_ms" > "$CSV_FILE"

extract_json_field() {
  local field="$1"
  local file="$2"
  awk -v k="$field" -F'[,:}]' '{for(i=1;i<=NF;i++){gsub(/[ \"{]/,"",$i); if($i==k){v=$(i+1); gsub(/[ \"{]/,"",v); print v; exit}}}' "$file"
}

echo "Running encode+solve comparison on $MAX_OBS observations ..."
for ((obs=1; obs<=MAX_OBS; obs++)); do
  start_ns="$(date +%s%N)"
  run_log="/tmp/ooe_encode_${obs}.log"

  set +e
  timeout "${ENCODE_TIMEOUT}s" "$BIN" "$SYS_FILE" "$TMP_CNF_FILE" "$SYSPP_FILE" "$OBS_FILE" "$obs" > "$run_log" 2>&1
  encode_code=$?
  set -e

  end_ns="$(date +%s%N)"
  encode_elapsed_ms=$(( (end_ns - start_ns) / 1000000 ))

  be_vars=""; be_clauses=""; doe_vars=""; doe_clauses=""; ooe_vars=""; ooe_clauses=""
  be_solve_ms=""; doe_solve_ms=""; ooe_solve_ms=""
  be_cost=""; doe_cost=""; ooe_cost=""
  be_code=""; doe_code=""; ooe_code=""

  if [[ "$encode_code" -eq 0 ]]; then
    read -r be_vars be_clauses _ < <(awk 'NR==1 && $1=="p" {print $3,$4,$5}' remain_clauses.wcnf)
    read -r doe_vars doe_clauses _ < <(awk 'NR==1 && $1=="p" {print $3,$4,$5}' remain_clauses1.wcnf)
    read -r ooe_vars ooe_clauses _ < <(awk 'NR==1 && $1=="p" {print $3,$4,$5}' remain_clauses2.wcnf)

    set +e
    timeout "${SOLVE_TIMEOUT}s" "$PYTHON_BIN" "$SOLVER_SCRIPT" remain_clauses.wcnf > /tmp/rc2_be_${obs}.json 2>&1
    be_code=$?
    timeout "${SOLVE_TIMEOUT}s" "$PYTHON_BIN" "$SOLVER_SCRIPT" remain_clauses1.wcnf > /tmp/rc2_doe_${obs}.json 2>&1
    doe_code=$?
    timeout "${SOLVE_TIMEOUT}s" "$PYTHON_BIN" "$SOLVER_SCRIPT" remain_clauses2.wcnf > /tmp/rc2_ooe_${obs}.json 2>&1
    ooe_code=$?
    set -e

    if [[ "$be_code" -eq 0 ]]; then
      be_solve_ms="$(extract_json_field total_ms /tmp/rc2_be_${obs}.json)"
      be_cost="$(extract_json_field cost /tmp/rc2_be_${obs}.json)"
    fi
    if [[ "$doe_code" -eq 0 ]]; then
      doe_solve_ms="$(extract_json_field total_ms /tmp/rc2_doe_${obs}.json)"
      doe_cost="$(extract_json_field cost /tmp/rc2_doe_${obs}.json)"
    fi
    if [[ "$ooe_code" -eq 0 ]]; then
      ooe_solve_ms="$(extract_json_field total_ms /tmp/rc2_ooe_${obs}.json)"
      ooe_cost="$(extract_json_field cost /tmp/rc2_ooe_${obs}.json)"
    fi
  fi

  echo "$obs,$encode_code,${be_code:-},${doe_code:-},${ooe_code:-},$be_vars,$be_clauses,$doe_vars,$doe_clauses,$ooe_vars,$ooe_clauses,$be_solve_ms,$doe_solve_ms,$ooe_solve_ms,$be_cost,$doe_cost,$ooe_cost,$encode_elapsed_ms" >> "$CSV_FILE"
done

{
  echo "CSV: $CSV_FILE"
  echo ""
  echo "Overall"
  awk -F',' 'NR>1 {
    total++;
    if($2==0) enc_ok++;
    if($2==124) enc_to++;
    if($2!=0 && $2!=124) enc_err++;
    if($3==0 && $4==0 && $5==0) solve_all_ok++;
  } END {
    printf("total=%d encode_ok=%d encode_timeout=%d encode_error=%d solve_all_ok=%d\n", total, enc_ok, enc_to, enc_err, solve_all_ok);
  }' "$CSV_FILE"

  echo ""
  echo "Average RC2 solve time on fully successful rows"
  awk -F',' 'NR>1 && $2==0 && $3==0 && $4==0 && $5==0 {
    n++;
    be+=$12; doe+=$13; ooe+=$14;
  } END {
    if(n==0){print "n=0"; exit}
    printf("n=%d\n", n);
    printf("BE  avg_solve_ms=%.3f\n", be/n);
    printf("DOE avg_solve_ms=%.3f\n", doe/n);
    printf("OOE avg_solve_ms=%.3f\n", ooe/n);
    printf("OOE speedup vs BE = %.3fx\n", (be/n)/(ooe/n));
    printf("OOE speedup vs DOE = %.3fx\n", (doe/n)/(ooe/n));
  }' "$CSV_FILE"

  echo ""
  echo "Solve-time wins (smaller is better)"
  awk -F',' 'NR>1 && $2==0 && $3==0 && $4==0 && $5==0 {
    n++;
    if(($14+0)<($13+0)) ooe_doe++;
    if(($14+0)<($12+0)) ooe_be++;
    if(($13+0)<($12+0)) doe_be++;
  } END {
    if(n==0){print "n=0"; exit}
    printf("OOE<DOE: %d/%d\n", ooe_doe, n);
    printf("OOE<BE : %d/%d\n", ooe_be, n);
    printf("DOE<BE : %d/%d\n", doe_be, n);
  }' "$CSV_FILE"

  echo ""
  echo "Cost consistency check (all equal means encoding preserves objective)"
  awk -F',' 'NR>1 && $2==0 && $3==0 && $4==0 && $5==0 {
    n++;
    if($15==$16 && $16==$17) ok++;
  } END {
    if(n==0){print "n=0"; exit}
    printf("equal_cost_rows=%d/%d\n", ok, n);
  }' "$CSV_FILE"
} | tee "$SUMMARY_FILE"

echo "Done."
