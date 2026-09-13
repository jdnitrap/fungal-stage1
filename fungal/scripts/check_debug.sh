#!/bin/bash
echo "Checking for debug code..."

cd "$(dirname "$0")/.." || exit 1

FAILED=0

if grep -r "TODO\|FIXME\|DEBUG" src/ --include="*.cpp" --include="*.hpp" | grep -v "test"; then
    echo "✗ Found debug markers in code"
    FAILED=1
fi

if grep -r "std::cout\|std::cerr" src/ --include="*.cpp" | grep -v "test" | grep -v "main.cpp"; then
    echo "⚠ Found console output in non-main code"
fi

if [ $FAILED -eq 1 ]; then
    exit 1
fi

echo "✓ No debug code detected"
exit 0
