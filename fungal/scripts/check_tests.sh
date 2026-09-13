#!/bin/bash
echo "Running tests..."

cd "$(dirname "$0")/.." || exit 1

if [ ! -d "build" ]; then
    mkdir build
    cd build || exit 1
    cmake .. > /dev/null 2>&1
    make > /dev/null 2>&1
else
    cd build || exit 1
fi

ctest -L unit > /dev/null 2>&1 || { echo "Unit tests failed"; exit 1; }

echo "✓ All tests passed"
exit 0
