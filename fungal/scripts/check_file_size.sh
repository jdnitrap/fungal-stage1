#!/bin/bash
echo "Checking file sizes for modularity..."

cd "$(dirname "$0")/.." || exit 1

WARN_THRESHOLD=350
FAIL_THRESHOLD=400
FAILED=0

for file in src/**/*.cpp src/**/*.hpp; do
    if [ -f "$file" ]; then
        lines=$(wc -l < "$file")
        if [ "$lines" -gt "$FAIL_THRESHOLD" ]; then
            echo "✗ $file: $lines lines (exceeds $FAIL_THRESHOLD)"
            FAILED=1
        elif [ "$lines" -gt "$WARN_THRESHOLD" ]; then
            echo "⚠ $file: $lines lines (exceeds $WARN_THRESHOLD)"
        fi
    fi
done

if [ $FAILED -eq 1 ]; then
    echo "✗ Some files exceed 400 line limit"
    exit 1
fi

echo "✓ All files within size limits"
exit 0
