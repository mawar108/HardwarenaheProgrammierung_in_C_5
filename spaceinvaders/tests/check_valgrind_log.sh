#!/usr/bin/env bash

logfile="$1"

if grep -E 'definitely lost: [1-9][0-9,]* bytes' "$logfile"
then
    printf '\tERROR: Found memory leak.\n'
    exit 4
fi

if grep -E 'indirectly lost: [1-9][0-9,]* bytes' "$logfile"
then
    printf '\tERROR: Found memory leak.\n'
    exit 4
fi

if grep -E 'possibly lost: [1-9][0-9,]* bytes' "$logfile"
then
    printf '\tERROR: Found memory leak.\n'
    exit 4
fi

if grep -E 'still reachable: [1-9][0-9,]* bytes' "$logfile"
then
    printf '\tERROR: Found memory leak.\n'
    exit 4
fi

if grep -E 'in use at exit: [1-9][0-9,]* bytes' "$logfile"
then
    printf '\tERROR: Found memory leak.\n'
    exit 4
fi

if grep -E 'Invalid free' "$logfile"
then
    printf '\tERROR: Found invalid free.\n'
    exit 4
fi

if grep -E 'Invalid read of size' "$logfile"
then
    printf '\tERROR: Found invalid read.\n'
    exit 4
fi

if grep -E 'Invalid write of size' "$logfile"
then
    printf '\tERROR: Found invalid write.\n'
    exit 4
fi

if grep -E 'depends on uninitialised' "$logfile"
then
    printf '\tERROR: Used uninitialized variables.\n'
    exit 4
fi
