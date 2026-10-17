#!/bin/sh
# Fake gemini-vm that fails: requires --modules <dir> <tbc>, error on stderr, exit 1.
set -e

modules=
tbc=
while [ "$#" -gt 0 ]; do
    case "$1" in
        --modules)
            shift
            [ "$#" -gt 0 ] || exit 2
            modules=$1
            shift
            ;;
        *)
            tbc=$1
            shift
            ;;
    esac
done

if [ -z "$modules" ] || [ -z "$tbc" ]; then
    echo "error: fake_gemini_vm_fail.sh requires --modules <dir> <tbc>" >&2
    exit 2
fi

echo "gemini-vm: intentional runtime failure" >&2
exit 1
