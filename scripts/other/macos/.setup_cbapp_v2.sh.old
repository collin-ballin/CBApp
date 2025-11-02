#!/usr/bin/env bash
#################################################################################################
# setup_cbapp.command – one-click bootstrap for Collin’s Dear ImGui project                     #
#                                                                                               #
#   SECTIONS                                                                                    #
#       1.  Initial safeguards & helpers                                                        #
#       2.  Environment check (Xcode, Homebrew, packages)                                       #
#       3.  Workspace: clone *or* pull latest code                                              #
#       4.  Ensure build/ dir, then run CMake                                                   #
#       5.  Completion                                                                          #
#                                                                                               #
#       --------------------------------------------------------------------------------        #
#   USAGE:                                                                                      #
#       1.  INSTALL APPLE'S XCODE IDE:                                                          #
#               - Simply go to Appstore and perform normal instal.
#                                                                                               #
#       2.  ADD EXECUTABLE PERMISSIONS TO THE SCRIPT:                                           #
#               >: chmod +x setup_cbapp_v2.sh                                                   #
#                                                                                               #
#       3.  RUN THE SCRIPT:                                                                     #
#           >: ./setup_cbapp.command            # -- safe update, keeps local edits             #
#           >: ./setup_cbapp.command --force    # -- discard local edits, force update          #
#                                                                                               #
#       --------------------------------------------------------------------------------        #
#       4.  WHAT IT DOES:                                                                       #
#           - Ensures that all tools to build and run the application are present.              #
#           - If tools are NOT present, script will install them.                               #
#           - If you RE-RUN the script, it will update the code to the most                     #
#             recent version that is available on my GitHub Repository.                         #
#           - Creates a directory  ~/Desktop/CBApp/  to host all project materials.             #
#           - Performs an initial, pre-build so that Xcode will understand                      #
#             how to correctly build and run the application.                                   #
#           - Automatically opens the project in Xcode for you.                                 #
#                                                                                               #
#       --------------------------------------------------------------------------------        #
#       --------------------------------------------------------------------------------        #
#       THAT'S IT!  From here, all you need to do is:                                           #
#           - "COMMAND B" to BUILD.     - "COMMAND R" to RUN.                                   #
#################################################################################################
#################################################################################################
set -Eeuo pipefail
IFS=$'\n\t'


##############################################################################
# 0. Argument parsing
##############################################################################
FORCE_RESET=false
[[ ${1:-} == "--force" ]] && FORCE_RESET=true


##############################################################################
# 1. Safeguards & helpers
##############################################################################
APP_NAME="CBApp"
REPO_URL="https://github.com/collin-ballin/CBApp.git"
TARGET_DIR="$HOME/Desktop/$APP_NAME"
BUILD_DIR="$TARGET_DIR/build"

log_ok()   { printf "      [OK]  %s\n" "$1"; }
log_info() { printf "      [..]  %s\n" "$1"; }
log_err()  { printf "      [ERR] %s\n" "$1" >&2; }
abort()    { log_err "$1"; exit 1; }

echo "==> $APP_NAME setup started (force-reset = $FORCE_RESET)"


##############################################################################
# 2. Environment check (Xcode, Homebrew, pkgs)
##############################################################################
# -- Xcode CLT ---------------------------------------------------------------
if xcode-select -p >/dev/null 2>&1; then
    log_ok "Xcode Command-Line Tools present ($(xcodebuild -version | head -n1))"
else
    log_info "Installing Xcode Command-Line Tools…"
    xcode-select --install || true
    abort "Re-run this script after the install completes."
fi

# -- Homebrew ---------------------------------------------------------------
if command -v brew >/dev/null 2>&1; then
    log_ok "Homebrew present ($(brew --version | head -n1))"
else
    log_info "Homebrew not found – installing…"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    eval "$(/opt/homebrew/bin/brew shellenv)"
    log_ok "Homebrew installed ($(brew --version | head -n1))"
fi

# -- Required formulae ------------------------------------------------------
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
# 3. Workspace: clone / update / reset
##############################################################################
echo "==> Preparing workspace at $TARGET_DIR"
if [[ -d "$TARGET_DIR/.git" ]]; then
    if $FORCE_RESET; then
        log_info "Force reset enabled – discarding local changes"
        git -C "$TARGET_DIR" fetch --prune
        git -C "$TARGET_DIR" reset --hard origin/main
        git -C "$TARGET_DIR" clean -fd
    else
        log_info "Repo present – pulling latest changes"
        if ! git -C "$TARGET_DIR" pull --ff-only; then
            abort "Local changes found. Re-run script as <./setup_cbapp.sh --force> to discard and force update to latest version."
        fi
    fi
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

log_info "Running CMake inside build directory with scheme generation"
pushd "$BUILD_DIR" >/dev/null
cmake -G "Xcode" -DCMAKE_XCODE_GENERATE_SCHEME=ON ..

# Optional: one upfront build so Xcode opens with a compiled app
cmake --build . --config Debug --parallel $(sysctl -n hw.ncpu)
popd >/dev/null


##############################################################################
# 5. Completion
##############################################################################
PROJECT_PATH="$BUILD_DIR/${APP_NAME}.xcodeproj"
echo "==> Opening project in Xcode..."
open "$PROJECT_PATH"

echo -e "\n==> SUCCESS – setup complete."
echo "    Project path: $PROJECT_PATH"



##############################################################################
##############################################################################
#   END.
