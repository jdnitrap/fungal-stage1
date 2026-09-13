#!/bin/bash
echo "Verifying critical interfaces..."

cd "$(dirname "$0")/.." || exit 1

FAILED=0

if ! grep -q "virtual void execute(FungalNetwork& network)" include/interfaces.hpp; then
    echo "✗ Phase interface missing execute method"
    FAILED=1
fi

if ! grep -q "struct FungalNetwork" include/types.hpp; then
    echo "✗ FungalNetwork type missing"
    FAILED=1
fi

if ! grep -q "struct Claim" include/types.hpp; then
    echo "✗ Claim type missing"
    FAILED=1
fi

if [ $FAILED -eq 1 ]; then
    exit 1
fi

echo "✓ All critical interfaces intact"
exit 0
