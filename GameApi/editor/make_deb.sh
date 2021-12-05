#!/bin/bash
if [[ $1 ]]; then
dpkg-deb -Zgzip --build gameapi-builder_1.0-$1
fi
if [[ ! $1 ]]; then
    echo "./make_deb.sh 25"
fi
