#!/bin/sh
# Fake gemini-vm that succeeds: requires <tbc>; accepts optional --modules <dir>.
set -e

tbc=
while [ "$#" -gt 0 ]; do
    case "$1" in
        --modules)
            shift
            [ "$#" -gt 0 ] || exit 2
            shift
            ;;
        *)
            tbc=$1
            shift
            ;;
    esac
done

if [ -z "$tbc" ]; then
    echo "error: fake_gemini_vm_success.sh requires <tbc>" >&2
    exit 2
fi

echo "Hello, world"
exit 0
