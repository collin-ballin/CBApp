#!/usr/bin/env bash
# dir_tree.sh ─ print a project tree while pruning children of selected dirs
# ---------------------------------------------------------------------------
# Usage examples
#   ./dir_tree.sh -r .                           # full tree of current dir
#   ./dir_tree.sh -d -r ..                       # dirs-only for parent dir
#   ./dir_tree.sh -r ~/code/CBApp -s "libs,external,vendor"
#   ./dir_tree.sh -d -r /tmp/proj -s "libs vendor third_party"
#
# Notes
#   • children of each dir in --skip-dirs are hidden (dir/*)
#   • script itself can live anywhere (e.g. root/scripts/other/dir_tree.sh)

set -euo pipefail

# -------------------------------------------------------- 1. Defaults / const
ALWAYS_IGNORE='build|\.git|CMakeFiles|\.idea|\.vscode'
ROOT="."
SKIP_LIST=()          # will fill from CLI
DIRS_ONLY=""

# -------------------------------------------------------- 2. Argument parsing
print_help() {
  echo "Usage: $0 [-d] -r <root_dir> [-s \"dir1 dir2,dir3\"]"
  echo "  -d            directories only"
  echo "  -r, --root    root directory to inspect (required)"
  echo "  -s, --skip-dirs  space- or comma-separated list of dirs to prune"
  exit 1
}

# manual getopts (portable)
while [[ $# -gt 0 ]]; do
  case "$1" in
    -d) DIRS_ONLY="-d"; shift ;;
    -r|--root) [[ $# -gt 1 ]] || print_help; ROOT="$2"; shift 2 ;;
    -s|--skip-dirs)
        [[ $# -gt 1 ]] || print_help
        # split on commas or spaces into array
        IFS=', ' read -r -a SKIP_LIST <<< "$2"
        shift 2
        ;;
    -h|--help) print_help ;;
    *) echo "Unknown arg: $1"; print_help ;;
  esac
done

[[ -d "$ROOT" ]] || { echo "Root dir '$ROOT' not found"; exit 1; }

# -------------------------------------------------------- 3. Build prune pattern
skip_pattern="$ALWAYS_IGNORE"
for dir in "${SKIP_LIST[@]}"; do
  [[ -z "$dir" ]] && continue
  skip_pattern+="|${dir}/*"
done

# -------------------------------------------------------- 4. Run tree
tree -a --dirsfirst $DIRS_ONLY \
     -I "$skip_pattern" \
     --noreport \
     "$ROOT"
