#!/bin/bash

# Generate random traffic

DATALEN=$1

if [ -z "$DATALEN" ]; then
    >&2 echo "Data length is not specified"
else
    cat /dev/urandom | base64 | \
    head -c  $DATALEN
fi
