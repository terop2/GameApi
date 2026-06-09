#!/bin/bash

if [ -n "$1" ]]; then
   echo "Continuing.."
else
    echo "Usage: ./tag_git.sh tagname"
    exit
fi

git tag $1
