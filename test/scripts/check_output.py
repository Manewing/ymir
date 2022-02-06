#!/usr/bin/env python3

import os
import sys
import subprocess
import argparse
import difflib

UPDATE_REFS = os.environ.get("UPDATE_REFS", 0) == "1"


def parse_args(args):
    parser = argparse.ArgumentParser()
    parser.add_argument("--ref",
                        type=str,
                        required=True,
                        help="Output to reference")
    parser.add_argument("--target",
                        type=str,
                        required=True,
                        help="Target executable to run")
    parser.add_argument("--args",
                        type=str,
                        default=[],
                        nargs="*",
                        help="Arguments for target executable")
    return parser.parse_args(args[1:])

def runCmd(args: list) -> str:
    print(f"Run: {' '.join(args):s}", file=sys.stderr)
    return subprocess.check_output(args).decode("utf-8")

def main(args):
    args = parse_args(args)

    output = runCmd([args.target] + args.args)
    if UPDATE_REFS:
        with open(args.ref, "w") as f:
            f.write(output)
        return 0

    with open(args.ref, "r") as f:
      ref_output = f.read()
    if output == ref_output:
        return 0

    diff = difflib.ndiff(output.splitlines(keepends=True),
                         ref_output.splitlines(keepends=True))
    print(f"Reference mismatch for target '{args.target:s}'", file=sys.stderr)
    print(f"Output reference mismatch for '{args.ref:s}'", file=sys.stderr)
    print("", file=sys.stderr)
    print("".join(diff))
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
