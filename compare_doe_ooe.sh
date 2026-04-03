#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

if [[ $# -lt 4 ]]; then
  echo "Usage: $0 <sys_file> <syspp_file> <obs_file> <tmp_cnf_file> [max_obs] [timeout_sec] [out_prefix]"
  echo "Example: $0 ./sys_server/c17.sys ./sys_server/c17.syspp ./iscas85/c17_iscas85.obs ./sys_server/c17.wcnf"
  exit 1
fi

SYS_FILE="$1"
SYSPP_FILE="$2"
OBS_FILE="$3"
TMP_CNF_FILE="$4"
MAX_OBS="${5:-}"
TIMEOUT_SEC="${6:-30}"
OUT_PREFIX="${7:-comparison}"
BIN="./ooe_main"

if [[ ! -f "$SYS_FILE" || ! -f "$SYSPP_FILE" || ! -f "$OBS_FILE" ]]; then
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

echo "Building $BIN ..."
g++ -std=c++11 -O2 -o "$BIN" pms.cpp

TS="$(date +%Y%m%d_%H%M%S)"
CSV_FILE="${OUT_PREFIX}_${TS}.csv"
SUMMARY_FILE="${OUT_PREFIX}_${TS}_summary.txt"

echo "obs,exit_code,be_vars,be_clauses,be_top,doe_vars,doe_clauses,doe_top,ooe_vars,ooe_clauses,ooe_top,doe_reduction,ooe_reduction,elapsed_ms" > "$CSV_FILE"

echo "Running comparison on $MAX_OBS observations ..."
for ((obs=1; obs<=MAX_OBS; obs++)); do
  start_ns="$(date +%s%N)"
  run_log="/tmp/ooe_cmp_${obs}.log"

  set +e
  timeout "${TIMEOUT_SEC}s" "$BIN" "$SYS_FILE" "$TMP_CNF_FILE" "$SYSPP_FILE" "$OBS_FILE" "$obs" > "$run_log" 2>&1
  code=$?
  set -e

  end_ns="$(date +%s%N)"
  elapsed_ms=$(( (end_ns - start_ns) / 1000000 ))

  be_vars=""; be_clauses=""; be_top=""
  doe_vars=""; doe_clauses=""; doe_top=""
  ooe_vars=""; ooe_clauses=""; ooe_top=""
  doe_red=""; ooe_red=""

  if [[ "$code" -eq 0 ]]; then
    read -r be_vars be_clauses be_top < <(awk 'NR==1 && $1=="p" {print $3,$4,$5}' remain_clauses.wcnf)
    read -r doe_vars doe_clauses doe_top < <(awk 'NR==1 && $1=="p" {print $3,$4,$5}' remain_clauses1.wcnf)
    read -r ooe_vars ooe_clauses ooe_top < <(awk 'NR==1 && $1=="p" {print $3,$4,$5}' remain_clauses2.wcnf)

    doe_red="$(grep -o 'DOE_reduction=[0-9-]*' "$run_log" | tail -n1 | cut -d= -f2)"
    ooe_red="$(grep -o 'OOE_reduction=[0-9-]*' "$run_log" | tail -n1 | cut -d= -f2)"
  fi

  echo "$obs,$code,$be_vars,$be_clauses,$be_top,$doe_vars,$doe_clauses,$doe_top,$ooe_vars,$ooe_clauses,$ooe_top,$doe_red,$ooe_red,$elapsed_ms" >> "$CSV_FILE"
done

{
  echo "CSV: $CSV_FILE"
  echo ""
  echo "Overall"
  awk -F',' 'NR>1 {
    total++;
    if($2==0) ok++;
    if($2==124) to++;
    if($2!=0 && $2!=124) err++;
  } END {
    printf("total=%d success=%d timeout=%d error=%d\n", total, ok, to, err);
  }' "$CSV_FILE"

  echo ""
  echo "Average WCNF size on successful runs"
  awk -F',' 'NR>1 && $2==0 {
    n++;
    be_v+=$3; be_c+=$4;
    doe_v+=$6; doe_c+=$7;
    ooe_v+=$9; ooe_c+=$10;
    t+=$14;
  } END {
    if(n==0){print "n=0"; exit}
    printf("n=%d\n", n);
    printf("BE  avg_vars=%.2f avg_clauses=%.2f\n", be_v/n, be_c/n);
    printf("DOE avg_vars=%.2f avg_clauses=%.2f\n", doe_v/n, doe_c/n);
    printf("OOE avg_vars=%.2f avg_clauses=%.2f\n", ooe_v/n, ooe_c/n);
    printf("avg_elapsed_ms=%.2f\n", t/n);
  }' "$CSV_FILE"

  echo ""
  echo "Pairwise wins on successful runs (smaller clauses is better)"
  awk -F',' 'NR>1 && $2==0 {
    n++;
    if(($10+0)<($7+0)) ooe_vs_doe++;
    if(($10+0)<($4+0)) ooe_vs_be++;
    if(($7+0)<($4+0)) doe_vs_be++;
  } END {
    if(n==0){print "n=0"; exit}
    printf("OOE<DOE: %d/%d\n", ooe_vs_doe, n);
    printf("OOE<BE : %d/%d\n", ooe_vs_be, n);
    printf("DOE<BE : %d/%d\n", doe_vs_be, n);
  }' "$CSV_FILE"
} | tee "$SUMMARY_FILE"

echo "Done."