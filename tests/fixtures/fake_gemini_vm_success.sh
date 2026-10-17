#!/bin/sh
# Fake gemini-vm that succeeds: requires --modules <dir> <tbc>, output on stdout.
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
    echo "error: fake_gemini_vm_success.sh requires --modules <dir> <tbc>" >&2
    exit 2
fi

echo "Hello, world"
exit 0
