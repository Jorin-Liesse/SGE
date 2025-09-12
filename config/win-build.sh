#!/usr/bin/env bash
set -euo pipefail

# ---- CONFIG ----
BUILD_DIR="build/win"

ARCH=""
LAUNCH="false"

for ARG in "$@"; do
    case $ARG in
        --architecture=*)
            ARCH="${ARG#*=}"
            shift
            ;;
        --launch=*)
            LAUNCH="${ARG#*=}"
            shift
            ;;
        *)
            echo "Unknown argument: $ARG"
            ;;
    esac
done

# ---- FUNCTIONS ----
check_or_install() {
    local cmd="$1"
    local choco_pkg="$2"
    
    if ! command -v "$cmd" &> /dev/null; then
        echo "'$cmd' not found. Installing via Chocolatey..."
        choco install "$choco_pkg" --yes
    else
        echo "Found $cmd: $(command -v "$cmd")"
    fi
}

# ---- UPDATE GIT SUBMODULES ----
git submodule update --init --recursive

# ---- ENSURE CHOCOLATEY IS INSTALLED ----
if ! command -v choco &> /dev/null; then
    echo "Chocolatey not found! Please install Chocolatey first: https://chocolatey.org/install"
    exit 1
fi

# ---- INSTALL OR CHECK REQUIRED TOOLS ----
check_or_install python python
check_or_install cmake cmake
check_or_install magick imagemagick
check_or_install git git

# ---- LOAD PROJECT ENV VARS ----
ICON_PATH=$(python -c "import json; print(json.load(open('assets/data/info.json'))['icon-path'])")
PROJECT=$(python -c "import json; print(json.load(open('assets/data/info.json'))['project'])")
ASSETS_PATH=$(python -c "import json; print(json.load(open('assets/data/info.json'))['assets-path'])")
RESOURCES_PATH=$(python -c "import json; print(json.load(open('assets/data/info.json'))['resources-path'])")
BUILD_MODE=$(python -c "import json; print(json.load(open('assets/data/info.json'))['build-mode'])")

echo "ICON_PATH=$ICON_PATH"
echo "PROJECT=$PROJECT"
echo "ASSETS_PATH=$ASSETS_PATH"
echo "RESOURCES_PATH=$RESOURCES_PATH"
echo "BUILD_MODE=$BUILD_MODE"

# ---- SET BUILD CONFIG ----
if [[ "$BUILD_MODE" == "debug" ]]; then
    CONFIG="Debug"
    elif [[ "$BUILD_MODE" == "release" ]]; then
    CONFIG="Release"
else
    echo "Unknown build mode '$BUILD_MODE' in info.json. Use 'debug' or 'release'."
    exit 1
fi

# ---- CREATE WINDOWS ICON ----
if command -v magick &> /dev/null; then
    magick "$ICON_PATH" -define icon:auto-resize="16,32,48,256" "$RESOURCES_PATH/icon.ico"
else
    echo "ImageMagick not found! Skipping icon generation."
fi

# ---- BUILD ----
cmake -DCMAKE_SYSTEM_NAME="Windows" \
-DCMAKE_SYSTEM_VERSION="10.0" \
-A"$ARCH" \
-S . -B "$BUILD_DIR"

cmake --build "$BUILD_DIR" --target "$PROJECT" --config "$CONFIG"

# ---- COPY ASSETS ----
mkdir -p "$BUILD_DIR/$CONFIG/assets"
cp -r "$ASSETS_PATH"/* "$BUILD_DIR/$CONFIG/$ASSETS_PATH"

echo "Build finished for x64. Output:"
ls -lh "$BUILD_DIR/$CONFIG"

echo "Win build complete ✅. Ready in '$BUILD_DIR/$CONFIG/'"

# ---- START EXE ----

if [[ "$LAUNCH" == "true" ]]; then
    echo "Launching $PROJECT.exe..."
    cmd.exe /c start "" "$BUILD_DIR/$CONFIG/$PROJECT.exe"
fi

