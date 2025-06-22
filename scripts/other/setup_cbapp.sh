#!/usr/bin/env bash
##############################################################################
#  setup_cbapp.command – one-click bootstrap for Collin’s Dear ImGui project #
#                                                                            #
#  Sections                                                                  #
#    1. Initial safeguards & helpers                                         #
#    2. Environment check (Xcode, Homebrew, packages)                        #
#    3. Workspace: clone *or* pull latest code                               #
#    4. Ensure build/ dir, then run CMake                                    #
#    5. Completion                                                           #
##############################################################################

##############################################################################
# 1. Initial safeguards & helpers
##############################################################################
set -Eeuo pipefail
IFS=$'\n\t'

APP_NAME="CBApp"
REPO_URL="https://github.com/collin-ballin/CBApp.git"
TARGET_DIR="$HOME/Desktop/$APP_NAME"
BUILD_DIR="$TARGET_DIR/build"           # <-- changed to match required layout

log_ok()   { printf "      [OK]  %s\n" "$1"; }
log_info() { printf "      [..]  %s\n" "$1"; }
log_err()  { printf "      [ERR] %s\n" "$1" >&2; }
abort()    { log_err "$1"; exit 1; }

echo "==> $APP_NAME setup started"

##############################################################################
# 2. Environment check
##############################################################################
if xcode-select -p >/dev/null 2>&1; then
    log_ok "Xcode Command-Line Tools present ($(xcodebuild -version | head -n1))"
else
    log_info "Installing Xcode Command-Line Tools…"
    xcode-select --install || true
    abort "Re-run this script after the install completes."
fi

if command -v brew >/dev/null 2>&1; then
    log_ok "Homebrew present ($(brew --version | head -n1))"
else
    log_info "Homebrew not found – installing…"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    eval "$(/opt/homebrew/bin/brew shellenv)"
    log_ok "Homebrew installed ($(brew --version | head -n1))"
fi

PKGS=(cmake git glfw)
MISSING_PKGS=()
for pkg in "${PKGS[@]}"; do
    if brew ls --versions "$pkg" >/dev/null 2>&1; then
        log_ok "$pkg present ($(brew ls --versions "$pkg" | awk '{print $2}'))"
    else
        log_info "$pkg NOT found – will install"
        MISSING_PKGS+=("$pkg")
    fi
done
[[ ${#MISSING_PKGS[@]} -gt 0 ]] && brew install "${MISSING_PKGS[@]}"

##############################################################################
# 3. Workspace: clone or update repo
##############################################################################
echo "==> Preparing workspace at $TARGET_DIR"
if [[ -d "$TARGET_DIR/.git" ]]; then
    log_info "Repo already present – pulling latest changes"
    git -C "$TARGET_DIR" pull --ff-only
else
    if [[ -e "$TARGET_DIR" ]]; then
        abort "'$TARGET_DIR' exists but isn’t a git repo – rename/remove it and re-run."
    fi
    log_info "Cloning repository"
    git clone "$REPO_URL" "$TARGET_DIR"
fi

##############################################################################
# 4. Ensure build/ dir, then run CMake
##############################################################################
log_info "Ensuring build directory exists: $BUILD_DIR"
mkdir -p "$BUILD_DIR"

log_info "Running CMake inside build directory"
pushd "$BUILD_DIR" >/dev/null
cmake -G "Xcode" ..          # exactly as required: parent source dir
popd >/dev/null

##############################################################################
# 5. Completion
##############################################################################
PROJECT_PATH="$BUILD_DIR/${APP_NAME}.xcodeproj"
echo "==> Opening project in Xcode"
open "$PROJECT_PATH"

echo -e "\n==> SUCCESS – setup complete."
echo "    Project path: $PROJECT_PATH"


##############################################################################
##############################################################################
#   END.
