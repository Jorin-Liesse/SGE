#!/usr/bin/env bash
set -euo pipefail

# ---- CONFIG ----
BUILD_DIR="build/web"

LAUNCH="false"

for ARG in "$@"; do
    case $ARG in
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
# git submodule update --init --recursive

# ---- ENSURE CHOCOLATEY IS INSTALLED ----
if ! command -v choco &> /dev/null; then
    echo "Chocolatey not found! Please install Chocolatey first: https://chocolatey.org/install"
    exit 1
fi

# ---- INSTALL OR CHECK REQUIRED TOOLS ----
check_or_install python python
check_or_install git git
check_or_install cmake cmake
check_or_install emsdk emsdk

# ---- LOAD PROJECT ENV VARS ----
PROJECT=$(python -c "import json; print(json.load(open('assets/data/info.json'))['project'])")
COMPANY=$(python -c "import json; print(json.load(open('assets/data/info.json'))['company'])")
MANIFEST_ORIENTATION=$(python -c "import json; print(json.load(open('assets/data/info.json'))['manifest-orientation'])")
MANIFEST_WINDOW_MODE=$(python -c "import json; print(json.load(open('assets/data/info.json'))['manifest-window-mode'])")
ICON_PATH=$(python -c "import json; print(json.load(open('assets/data/info.json'))['icon-path'])")
RESOURCES_PATH=$(python -c "import json; print(json.load(open('assets/data/info.json'))['resources-path'])")
BUILD_MODE=$(python -c "import json; print(json.load(open('assets/data/info.json'))['build-mode'])")

echo "PROJECT=$PROJECT"
echo "COMPANY=$COMPANY"
echo "MANIFEST_ORIENTATION=$MANIFEST_ORIENTATION"
echo "MANIFEST_WINDOW_MODE=$MANIFEST_WINDOW_MODE"
echo "ICON_PATH=$ICON_PATH"
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

# ---- BUILD ----
mkdir -p "$BUILD_DIR"
emcmake cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --config "$CONFIG" --parallel

# ---- PREPARE _site FOLDER ----
rm -rf "${BUILD_DIR}/${CONFIG}"
mkdir -p "${BUILD_DIR}/${CONFIG}"

cp "${BUILD_DIR}/${PROJECT}.js" "${BUILD_DIR}/${CONFIG}/"
cp "${BUILD_DIR}/${PROJECT}.data" "${BUILD_DIR}/${CONFIG}/"
cp "${BUILD_DIR}/${PROJECT}.wasm" "${BUILD_DIR}/${CONFIG}/"

cp "$RESOURCES_PATH/index.html" "${BUILD_DIR}/${CONFIG}/"
cp "$RESOURCES_PATH/style.css" "${BUILD_DIR}/${CONFIG}/"
cp "$RESOURCES_PATH/script.js" "${BUILD_DIR}/${CONFIG}/"
cp "$RESOURCES_PATH/manifest.json" "${BUILD_DIR}/${CONFIG}/"

# ---- REPLACE PLACEHOLDERS ----
export PROJECT MANIFEST_ORIENTATION MANIFEST_WINDOW_MODE ICON_PATH

for file in "${BUILD_DIR}/${CONFIG}/index.html" "${BUILD_DIR}/${CONFIG}/script.js" "${BUILD_DIR}/${CONFIG}/style.css" "${BUILD_DIR}/${CONFIG}/manifest.json"; do
    sed -i "s|\${PROJECT}|$PROJECT|g" "$file"
    sed -i "s|\${MANIFEST_ORIENTATION}|$MANIFEST_ORIENTATION|g" "$file"
    sed -i "s|\${MANIFEST_WINDOW_MODE}|$MANIFEST_WINDOW_MODE|g" "$file"
    sed -i "s|\${ICON_PATH}|$ICON_PATH|g" "$file"
done

# ---- COPY ICON FOLDER ----
ICON_DIR=$(dirname "$ICON_PATH")
mkdir -p "${BUILD_DIR}/${CONFIG}/$ICON_DIR"
cp "$ICON_PATH" "${BUILD_DIR}/${CONFIG}/$ICON_DIR/"

echo "Web build complete ✅. Ready in '$BUILD_DIR/$CONFIG/'"

# ---- START LOCAL SERVER ----

if [[ "$LAUNCH" == "true" ]]; then
    echo "Starting local web server at http://localhost:8000"
    cd "${BUILD_DIR}/${CONFIG}"
    
    python -m http.server 8000 &
    
    cmd.exe /c start http://localhost:8000
    
    wait
fi
