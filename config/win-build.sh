#!/usr/bin/env bash
set -euo pipefail

# ---- CONFIG ----
BUILD_DIR="build/win"
CONFIG="Release"

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

echo "ICON_PATH=$ICON_PATH"
echo "PROJECT=$PROJECT"
echo "ASSETS_PATH=$ASSETS_PATH"
echo "RESOURCES_PATH=$RESOURCES_PATH"

# ---- CREATE WINDOWS ICON ----
if command -v magick &> /dev/null; then
    magick convert "$ICON_PATH" -define icon:auto-resize="16,32,48,256" "$RESOURCES_PATH/icon.ico"
else
    echo "ImageMagick not found! Skipping icon generation."
fi

# ---- BUILD ----
echo "=== Building for "Windows" - x64 ==="

cmake -DCMAKE_SYSTEM_NAME="Windows" \
      -DCMAKE_SYSTEM_VERSION="10.0" \
      -A"x64" \
      -S . -B "$BUILD_DIR"

cmake --build "$BUILD_DIR" --target "$PROJECT" --config "$CONFIG"

# ---- COPY ASSETS ----
mkdir -p "$BUILD_DIR/$CONFIG/assets"
cp -r "$ASSETS_PATH"/* "$BUILD_DIR/$CONFIG/$ASSETS_PATH"

echo "Build finished for x64. Output:"
ls -lh "$BUILD_DIR/$CONFIG"

echo "Win build complete ✅. Ready in '$BUILD_DIR/$CONFIG/'"

# ---- START EXE ----
echo "Launching $PROJECT.exe..."
cmd.exe /c start "" "$BUILD_DIR/$CONFIG/$PROJECT.exe"
