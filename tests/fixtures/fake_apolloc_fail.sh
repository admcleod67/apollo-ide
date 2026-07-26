#!/bin/sh
# Fake apolloc that fails: diagnostics on stderr, exit 1.
echo "error: intentional compile failure" >&2
exit 1
