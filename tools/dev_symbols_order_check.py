#!/usr/bin/env python3

###
# Helper file to add custom rules to the ninja build in order to check the symbols order at the
# end of the build.
# It adds:
# - post-build check of the symbols order of all modified cpp files found in git against main branch
# This is to automate the symbols check in the workflow and ease the developer experience.
###

import subprocess
import sys
from pathlib import Path

def configure_symbols_order_check(
        config) -> None:
    """Add custom rules to the project in order to check automatically
    the symbols order with the current git diff."""

    map_out = f"orig/{config.version}/files/debugInfoM.MAP"

    config.custom_build_rules = [
        {
            "name": "check_symbols_order",
            "command": "python3 tools/dev_symbols_order_check.py",
            "description": "CHECK symbols order",
            "pool": "console",
        },
    ]

    config.custom_build_steps = {
        "post-build": [
            {
                "rule": "check_symbols_order",
                "outputs": "check_symbols_order.stamp",
                "implicit": [map_out],
            },
        ],
    }

def get_changed_cpp_files(base="main"):
    """Helper function performing a git diff against a given branch
    in order to find which cpp files changed."""
    merge_base = subprocess.check_output(
        ["git", "merge-base", base, "HEAD"], text=True
    ).strip()
    out = subprocess.check_output(
        ["git", "diff", "--name-only", merge_base, "--", "*.cpp"], text=True
    )
    return out.split()

def main() -> int:
    """Perform a git diff and run the symbol order check on all found cpp files.
    This main will be used during ninja build phase configured in 'configure_symbol_order_check'.
    """
    changed = get_changed_cpp_files()
    if not changed:
        print("No changed .cpp files vs main — nothing to check.")
        return 0

    script = Path(__file__).parent / "check-changed-symbol-order.py"
    return subprocess.call([sys.executable, str(script), *changed])

if __name__ == "__main__":
    sys.exit(main())
