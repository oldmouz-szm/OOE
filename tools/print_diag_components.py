#!/usr/bin/env python3
import argparse
import re
import time
from pysat.examples.rc2 import RC2
from pysat.formula import WCNF


def split_outer(token: str) -> str:
    left = token.find("[")
    right = token.rfind("]")
    if left == -1 or right == -1 or right <= left:
        return ""
    return token[left + 1:right]


def split_csv(text: str):
    if not text:
        return []
    return [x.strip() for x in text.split(",") if x.strip()]


def build_symbol_map(sys_file: str):
    m = {}
    in_len = 0
    out_len = 0
    gate_num = 0
    m_cur_idx = 1

    with open(sys_file, "r", encoding="utf-8") as f:
        tokens = f.read().split()

    first = True
    for token in tokens:
        if first:
            first = False
            continue

        item = split_outer(token)
        if not item:
            continue

        res = split_csv(item)
        if not res:
            continue

        head = res[0]
        if head.startswith("i") and head != "in":
            in_len += len(res)
        elif head.startswith("o") and head != "or":
            for name in reversed(res):
                if name not in m:
                    m[name] = m_cur_idx
                    m_cur_idx += 1
                    out_len += 1
        else:
            gate_num += 1
            if len(res) > 1 and res[1] not in m:
                m[res[1]] = m_cur_idx
                m_cur_idx += 1

    for i in range(1, in_len + 1):
        key = f"i{i}"
        if key not in m:
            m[key] = m_cur_idx
            m_cur_idx += 1

    for i in range(1, out_len + 1):
        key = f"o{i}"
        if key not in m:
            m[key] = m_cur_idx
            m_cur_idx += 1

    for i in range(1, gate_num + 1):
        key = f"z{i}"
        if key not in m:
            m[key] = m_cur_idx
            m_cur_idx += 1

    return m


def parse_injected_faults(obs_file: str):
    with open(obs_file, "r", encoding="utf-8") as f:
        for line in f:
            s = line.strip().lower()
            if s.startswith("#") and "faults=" in s:
                right = s.split("faults=", 1)[1]
                return [int(x) for x in re.findall(r"\d+", right)]
    return []


def parse_wcnf_header(path: str):
    vars_n = None
    clauses_n = None
    top_w = None
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            s = line.strip()
            if not s:
                continue
            if s.startswith("p "):
                parts = s.split()
                if len(parts) >= 5 and parts[1] == "wcnf":
                    vars_n = int(parts[2])
                    clauses_n = int(parts[3])
                    top_w = int(parts[4])
                break
    return vars_n, clauses_n, top_w


def main():
    parser = argparse.ArgumentParser(
        description="Decode MaxSAT diagnosis vars into gate component names"
    )
    parser.add_argument("--sys", required=True, help="Path to .sys file")
    parser.add_argument("--wcnf", required=True, help="Path to WCNF file")
    parser.add_argument("--obs", help="Optional obs file to compare injected faults")
    parser.add_argument("--encoding-sec", type=float, default=None, help="Optional encoding time in seconds")
    parser.add_argument("--max-solutions", type=int, default=20, help="Max number of optimal solutions to enumerate")
    args = parser.parse_args()

    m = build_symbol_map(args.sys)
    inv = {v: k for k, v in m.items()}

    w = WCNF(from_file=args.wcnf)
    hdr_vars, hdr_clauses, _ = parse_wcnf_header(args.wcnf)

    t0 = time.perf_counter()
    all_opt_solutions = []
    with RC2(w) as rc2:
        model = rc2.compute()
        if model is None:
            print("STATUS=UNSAT")
            return
        opt_cost = rc2.cost

        while model is not None and rc2.cost == opt_cost and len(all_opt_solutions) < args.max_solutions:
            assigned = set(model)
            violated_soft = [cl for cl in w.soft if not any(lit in assigned for lit in cl)]
            diag_vars = tuple(sorted({abs(cl[0]) for cl in violated_soft if len(cl) == 1}))
            diag_components = tuple(inv.get(v, f"<unknown:{v}>") for v in diag_vars)
            all_opt_solutions.append((diag_vars, diag_components))

            block = [-lit for lit in model if 0 < abs(lit) <= w.nv]
            if not block:
                break
            rc2.add_clause(block)
            model = rc2.compute()

    solve_sec = time.perf_counter() - t0

    # Keep unique diagnoses by component tuple, preserving order.
    seen = set()
    uniq = []
    for diag_vars, diag_components in all_opt_solutions:
        if diag_components in seen:
            continue
        seen.add(diag_components)
        uniq.append((diag_vars, diag_components))

    print("STATUS=OK")
    print(f"REDUCED_CLAUSES={hdr_clauses if hdr_clauses is not None else (len(w.hard) + len(w.soft))}")
    print(f"REDUCED_VARS={hdr_vars if hdr_vars is not None else w.nv}")
    print(f"SOLUTIONS_FOUND={len(uniq)}")
    print(f"SOLUTION_SIZE={opt_cost}")
    print(f"SOLVE_TIME_SEC={solve_sec:.6f}")
    if args.encoding_sec is not None:
        print(f"ENCODING_TIME_SEC={args.encoding_sec:.6f}")
        print(f"TOTAL_TIME_SEC={args.encoding_sec + solve_sec:.6f}")

    for idx, (_, diag_components) in enumerate(uniq, start=1):
        print("FAULT_COMPONENTS_" + str(idx) + "=" + ",".join(diag_components))

    if len(all_opt_solutions) >= args.max_solutions:
        print(f"NOTE=enumeration_stopped_at_max_solutions_{args.max_solutions}")

    if args.obs:
        injected = parse_injected_faults(args.obs)
        injected_names = [f"gate{x}" for x in injected]
        print("INJECTED_FAULTS=" + ",".join(injected_names))


if __name__ == "__main__":
    main()
