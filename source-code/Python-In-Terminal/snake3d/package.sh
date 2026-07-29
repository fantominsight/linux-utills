#!/bin/bash
set -e

APP=snake3d
APPDIR="$(dirname "$0")/AppDir"
BUILD="$(dirname "$0")/build"
ICON="/tmp/snake3d_icon.png"
APPIMAGETOOL="/tmp/squashfs-root/AppRun"

# Copy binary
mkdir -p "$APPDIR/usr/bin"
cp "$BUILD/$APP" "$APPDIR/usr/bin/"

# Gather dependencies
mkdir -p "$APPDIR/usr/lib"
for lib in $(ldd "$BUILD/$APP" | grep "=> /" | awk '{print $3}' | grep -v -E 'ld-linux|libc\.so|libm\.so|libstdc\+\+|libgcc|libdl|libpthread|linux-vdso'); do
    cp -L "$lib" "$APPDIR/usr/lib/"
done

# Additional GL libs
for lib in /usr/lib/x86_64-linux-gnu/libGLX.so* /usr/lib/x86_64-linux-gnu/libOpenGL.so* /usr/lib/x86_64-linux-gnu/libGLdispatch.so*; do
    cp -L "$lib" "$APPDIR/usr/lib/" 2>/dev/null || true
done

# .desktop file
cat > "$APPDIR/$APP.desktop" << EOF
[Desktop Entry]
Type=Application
Name=3D Snake
Comment=A 3D Snake game
Exec=$APP
Icon=$APP
Categories=Game;
EOF

# Icon
cp "$ICON" "$APPDIR/$APP.png"

# AppRun
cat > "$APPDIR/AppRun" << 'APPRUN'
#!/bin/bash
HERE="$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="$HERE/usr/lib:$LD_LIBRARY_PATH"
exec "$HERE/usr/bin/snake3d" "$@"
APPRUN
chmod +x "$APPDIR/AppRun"

# Build AppImage
OUTPUT="$(dirname "$0")/$APP-x86_64.AppImage"
$APPIMAGETOOL "$APPDIR" "$OUTPUT"
echo "AppImage created: $OUTPUT"
