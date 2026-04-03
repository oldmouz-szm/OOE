#!/usr/bin/env python3
import json
import sys
import time
from pysat.formula import WCNF
from pysat.examples.rc2 import RC2


def main() -> int:
    if len(sys.argv) < 2:
        print(json.dumps({"ok": False, "error": "usage: solve_wcnf_rc2.py <file.wcnf>"}))
        return 2

    path = sys.argv[1]
    t0 = time.perf_counter()
    try:
        wcnf = WCNF(from_file=path)
        t1 = time.perf_counter()
        with RC2(wcnf) as rc2:
            model = rc2.compute()
            t2 = time.perf_counter()
            cost = rc2.cost
    except Exception as exc:
        print(json.dumps({"ok": False, "error": str(exc), "file": path}))
        return 1

    print(
        json.dumps(
            {
                "ok": model is not None,
                "file": path,
                "cost": int(cost),
                "parse_ms": round((t1 - t0) * 1000.0, 3),
                "solve_ms": round((t2 - t1) * 1000.0, 3),
                "total_ms": round((t2 - t0) * 1000.0, 3),
            }
        )
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
