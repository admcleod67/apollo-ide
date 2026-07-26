#!/bin/sh
# Fake gemini-vm that fails: error on stderr, exit 1.
echo "gemini-vm: intentional runtime failure" >&2
exit 1
