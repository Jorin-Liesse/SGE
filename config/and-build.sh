#!/bin/bash
set -e

# --- Setup ---
echo "==> Cloning submodules..."
git submodule update --init --recursive

echo "==> Checking Java version..."
java -version

# --- Update build.gradle ---
echo "==> Copying build.gradle..."
cp build.gradle SDL/android-project/app/

# --- Extract env vars ---
echo "==> Reading icon path from assets/data/info.json..."
ICON_PATH=$(python -c "import json; print(json.load(open('assets/data/info.json'))['icon-path'])")
echo "ICON_PATH=$ICON_PATH"

# --- Inject app icon ---
echo "==> Installing ImageMagick if missing..."
if ! command -v magick &> /dev/null; then
  sudo apt-get update && sudo apt-get install -y imagemagick
fi

echo "==> Injecting app icons..."
for entry in "mdpi:48" "hdpi:72" "xhdpi:96" "xxhdpi:144" "xxxhdpi:192"; do
  d="${entry%%:*}"
  size="${entry##*:}"
  mkdir -p SDL/android-project/app/src/main/res/mipmap-$d
magick "$ICON_PATH" -resize ${size}x${size} \
  SDL/android-project/app/src/main/res/mipmap-$d/ic_launcher.png
done

# --- Build APK ---
echo "==> Building APK..."
cd SDL/android-project/
SDL/android-project/gradlew.bat assembleDebug

# --- Output ---
APK_PATH="app/build/outputs/apk/debug/app-debug.apk"
if [ -f "$APK_PATH" ]; then
  echo "==> APK built successfully: $APK_PATH"
else
  echo "ERROR: APK not found!"
  exit 1
fi
