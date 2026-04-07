#!/usr/bin/env python3
import sys

def format_table():
    blocks = []
    current = {}
    inj_faults = ""

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        if line.startswith("====="):
            if current:
                blocks.append(current)
                current = {}
            continue
        
        if "=" in line:
            k, v = line.split("=", 1)
            current[k] = v
            if k == "INJECTED_FAULTS":
                inj_faults = v
                
    if current:
        blocks.append(current)

    if not blocks:
        print("No valid output found.")
        return

    modes = [b.get("MODE", "UNKNOWN").upper() for b in blocks]
    
    metrics = [
        "STATUS",
        "REDUCED_CLAUSES",
        "REDUCED_VARS",
        "SOLUTIONS_FOUND",
        "SOLUTION_SIZE",
        "ENCODING_TIME_SEC",
        "SOLVE_TIME_SEC",
        "TOTAL_TIME_SEC"
    ]

    col_w = 20
    header = f"| {'METRIC':<{col_w}} |"
    for m in modes:
        header += f" {m:<{col_w}} |"

    sep = "+" + "-"*(col_w+2) + "+" + ("-"*(col_w+2) + "+") * len(modes)

    print()
    print(sep)
    print(header)
    print(sep)

    for metric in metrics:
        row = f"| {metric:<{col_w}} |"
        all_missing = True
        for b in blocks:
            val = b.get(metric, "-")
            if val != "-":
                all_missing = False
            row += f" {val:<{col_w}} |"
        if not all_missing:
            print(row)
    print(sep)
    print()

    print("[FAULT COMPONENTS]")
    for b in blocks:
        mode = b.get("MODE", "UNKNOWN").upper()
        print(f"--- {mode} ---")
        faults = []
        for k, v in b.items():
            if k.startswith("FAULT_COMPONENTS_"):
                try:
                    idx = int(k.split("_")[-1])
                    faults.append((idx, v))
                except ValueError:
                    pass
        faults.sort()
        for idx, v in faults:
            print(f"  {idx:>2}: {v}")
        if not faults:
            if b.get("STATUS") == "UNSAT":
                print("  UNSAT (No solution found)")
            else:
                print("  None")
        
        # Print NOTES if any
        for k, v in b.items():
            if k.startswith("NOTE="):
                print(f"  ({v})")
    
    if inj_faults:
        print(f"\n[INJECTED FAULTS]\n  {inj_faults}\n")

if __name__ == "__main__":
    format_table()