#!/bin/sh
# Fake apolloc that succeeds: requires --tbc <src> -o <out>, writes .tbc to -o.
set -e

out=
while [ "$#" -gt 0 ]; do
    case "$1" in
        --tbc)
            shift
            [ "$#" -gt 0 ] || exit 2
            shift
            ;;
        -o)
            shift
            [ "$#" -gt 0 ] || exit 2
            out=$1
            shift
            ;;
        *)
            shift
            ;;
    esac
done

if [ -z "$out" ]; then
    echo "error: fake_apolloc_success.sh requires -o <path>" >&2
    exit 2
fi

printf '%s\n' 'LINE 1' 'PRINT "hello"' >"$out"
exit 0
