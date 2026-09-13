#!/bin/bash
echo "Checking compilation..."

cd "$(dirname "$0")/.." || exit 1

if [ ! -d "build" ]; then
    mkdir build
fi

cd build || exit 1
cmake .. > /dev/null 2>&1 || { echo "CMake configuration failed"; exit 1; }
make > /dev/null 2>&1 || { echo "Compilation failed"; exit 1; }

echo "✓ Compilation successful"
exit 0
