#!/usr/bin/env bash
#################################################################################
#   setup_cbapp.command – one-click bootstrap for Collin’s Dear ImGui project   #
#                                                                               #
#   SECTIONS:                                                                   #
#       1.  Initial safeguards & helpers                                        #
#       2.  Environment check (Xcode, Homebrew, packages)                       #
#       3.  Workspace: clone *or* pull latest code                              #
#       4.  Ensure build/ dir, then run CMake                                   #
#       5.  Completion                                                          #
#                                                                               #
#  USAGE:                                                                       #
#       ./setup_cbapp.command [--branch <name>] [--force]                       #
#                                                                               #
#################################################################################
#   EXAMPLE INVOCATIONS:
#       1.  DEFAULT :           // Clone/update code from the main branch of my repository.
#               ./setup_cbapp.command
#
#       2.  DIFFERENT BRANCH :  // Check out code from a different branch of my repository (e.g., dev).
#               ./setup_cbapp.command --branch dev
#           Or, the short form,
#               ./setup_cbapp.command -b dev
#
#       3.  HARD-RESET :        // Discard any local changes and revert to the code on my repository.
#               ./setup_cbapp.command --force
#
#       4.  HYBRID :            // Combine (2.) and (3.) : Discards local edits AND installs from a different branch.
#               ./setup_cbapp.command --branch feature/ui-overhaul --force
#
#       5.  HELP :              // Display a short "help" message to the CMD-Line and exit.
#               ./setup_cbapp.command --help
#               ./setup_cbapp.command -h
#
#################################################################################
#################################################################################
set -Eeuo pipefail
IFS=$'\n\t'


#################################################################################
#   0.      ARGUMENT PARSING...                                                 #
#################################################################################
BRANCH="main"       # default branch
FORCE_RESET=false

print_help() {
    cat <<'EOF'
Usage: ./setup_cbapp.command [--branch <name>] [--force]

Options:
  -b, --branch <name>   Checkout the named branch instead of 'main'.
  --force               Discard local edits and hard-reset to origin/<branch>.
  -h, --help            Show this help and exit.

EXAMPLE INVOCATIONS:
    1.  DEFAULT :           // Clone/update code from the main branch of my repository.
            ./setup_cbapp.command

    2.  DIFFERENT BRANCH :  // Check out code from a different branch of my repository (e.g., dev).
            ./setup_cbapp.command --branch dev
        Or, the short form,
            ./setup_cbapp.command -b dev

    3.  HARD-RESET :        // Discard any local changes and revert to the code on my repository.
            ./setup_cbapp.command --force

    4.  HYBRID :            // Combine (2.) and (3.) : Discards local edits AND installs from a different branch.
            ./setup_cbapp.command --branch feature/ui-overhaul --force

    5.  HELP :              // Display a short "help" message to the CMD-Line and exit.
            ./setup_cbapp.command --help
            ./setup_cbapp.command -h
EOF
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--branch)
            [[ $# -lt 2 ]] && { echo "Error: --branch requires an argument" >&2; exit 1; }
            BRANCH="$2"; shift 2 ;;
        --force)   FORCE_RESET=true; shift ;;
        -h|--help) print_help ;;
        *) echo "Unknown argument: $1" >&2; exit 1 ;;
    esac
done



#################################################################################
#   1.      GLOBALS AND HELPERS...                                              #
#################################################################################
APP_NAME="CBApp"
REPO_URL="https://github.com/collin-ballin/CBApp.git"
TARGET_DIR="$HOME/Desktop/$APP_NAME"
BUILD_DIR="$TARGET_DIR/build"

log_ok()        { printf "      [\e[1m\e[32m  OK   \e[0m]   : \e[32m%s\e[0m\n"        "$1"; }
log_info()      { printf "      [\e[2m\e[39m INFO  \e[0m]   : \e[2m\e[39m%s\e[0m\n"   "$1"; }
log_err()       { printf "      [\e[1m\e[31m ERROR \e[0m]   : \e[1m\e[31m%s\e[0m\n"   "$1" >&2; }
log_notify()    { printf "\e[34m==>\e[0m \e[1m\e[39m%s\e[0m\n"   "$1"; }
abort()         { log_err "$1"; exit 1; }






#################################################################################
#   2.      ENVIRONMENT CHECK (Xcode, Homebrew, etc)...                         #
#################################################################################
log_notify  "BEGINNING CBAPP BOOTSTRAP SHELL SCRIPT (branch=$BRANCH  force=$FORCE_RESET)..."
log_info    "(re-run with -h or --help for more info)"
echo ""


# -- Xcode CLT --------------------------------------------------------------
if xcode-select -p >/dev/null 2>&1; then
    log_ok "Xcode CLT is present ($(xcodebuild -version | head -n1))"
else
    log_info "Installing Xcode Command-Line Tools..."
    xcode-select --install || true
    log_info "Waiting for Xcode CLT to finish installing..."
    until xcode-select -p >/dev/null 2>&1; do sleep 5; done
    log_ok "Xcode CLT installed"
fi

# -- Homebrew ---------------------------------------------------------------
if command -v brew >/dev/null 2>&1; then
    log_ok "Homebrew is present ($(brew --version | head -n1))"
else
    log_info "Homebrew was NOT found.  Proceeding to install..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    # shellcheck disable=SC1091
    eval "$(/opt/homebrew/bin/brew shellenv 2>/dev/null || /usr/local/bin/brew shellenv)"
    log_ok "Homebrew installed"
fi

brew update --quiet

# -- Required formulae ------------------------------------------------------
PKGS=(cmake git glfw)
MISSING_PKGS=()
for pkg in "${PKGS[@]}"; do
    if brew ls --versions "$pkg" >/dev/null 2>&1; then
        log_ok "$pkg is present ($(brew ls --versions "$pkg" | awk '{print $2}'))"
    else
        log_info "$pkg NOT found.  Proceeding to install..."
        MISSING_PKGS+=("$pkg")
    fi
done
[[ ${#MISSING_PKGS[@]} -gt 0 ]] && brew install "${MISSING_PKGS[@]}"


#################################################################################
#   3.      WORKSPACE: Clone / Update / Reset...                                #
#################################################################################
log_info "Preparing the project workspace at $TARGET_DIR..."
if [[ -d "$TARGET_DIR/.git" ]]; then
    # Existing repo ---------------------------------------------------------
    CURRENT_BRANCH=$(git -C "$TARGET_DIR" rev-parse --abbrev-ref HEAD)
    if [[ "$CURRENT_BRANCH" != "$BRANCH" ]]; then
        log_info "Switching branch: $CURRENT_BRANCH → $BRANCH"
        git -C "$TARGET_DIR" fetch --prune origin "$BRANCH"
        git -C "$TARGET_DIR" checkout "$BRANCH"
    fi

    if $FORCE_RESET; then
        log_info "Force reset option is ENABLED.  Discarding local changes..."
        git -C "$TARGET_DIR" fetch --prune
        git -C "$TARGET_DIR" reset --hard "origin/$BRANCH"
        git -C "$TARGET_DIR" clean -fd
    else
        if ! git -C "$TARGET_DIR" diff --quiet || ! git -C "$TARGET_DIR" diff --cached --quiet; then
            log_info "Local edits were detected.  Staching, Pulling, and re‑applying"
            git -C "$TARGET_DIR" stash push -u -m "cbapp-auto-stash $(date +%s)"
            git -C "$TARGET_DIR" pull --ff-only || abort "Fast-forward merge failed"
            git -C "$TARGET_DIR" stash pop || true
        else
            git -C "$TARGET_DIR" pull --ff-only
        fi
    fi
else
    # Fresh clone -----------------------------------------------------------
    if [[ -e "$TARGET_DIR" ]]; then
        abort "The directory '$TARGET_DIR' exists, but it is NOT a git repository!\nPlease rename or remove the script and re-run this script."
    fi
    log_info "Cloning the project repository (branch=$BRANCH)..."
    git clone --branch "$BRANCH" --single-branch "$REPO_URL" "$TARGET_DIR"
fi


#################################################################################
#   4.      CMAKE CONFIGURATION (NO MORE PRE-BUILD)...                          #
#################################################################################
log_info "Ensuring the build directory exists: $BUILD_DIR..."
mkdir -p "$BUILD_DIR"

pushd "$BUILD_DIR" >/dev/null
log_info "Running CMake (Xcode generator)..."
cmake -G "Xcode" -DCMAKE_XCODE_GENERATE_SCHEME=ON -DCBAPP_GIT_BRANCH="$BRANCH" ..

# --- Fix default scheme so ⌘R builds/launches CBApp ------------------------
DEFAULT_SCHEME="$APP_NAME"
SCHEME_PLIST="$BUILD_DIR/${DEFAULT_SCHEME}.xcodeproj/xcshareddata/xcschemes/${DEFAULT_SCHEME}.xcscheme"
if [[ -f "$SCHEME_PLIST" ]]; then
    /usr/libexec/PlistBuddy -c "Set :LaunchAction:selectedDebuggerIdentifier com.apple.debugger.lldb" "$SCHEME_PLIST" || true
    /usr/libexec/PlistBuddy -c "Set :LaunchAction:selectedLauncherIdentifier Xcode.IDEFoundation.Launcher.LLDB" "$SCHEME_PLIST" || true
fi
popd >/dev/null


#################################################################################
#   5.      SCRIPT COMPLETION...                                                #
#################################################################################
PROJECT_PATH="$BUILD_DIR/${APP_NAME}.xcodeproj"

log_notify  "SUCCESS.  CBAPP SETUP COMPLETE."
log_info    "all project build dependencies are present"
log_info    "project materials installed at: $PROJECT_PATH"

echo ""
log_info    "Opening project in Xcode..."

open "$PROJECT_PATH"




#################################################################################
#################################################################################   END.
