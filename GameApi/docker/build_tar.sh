#!/bin/bash
sed s/27/$1/g Dockerfile.tmp >Dockerfile
rm gameapi-builder-docker-container.tar.gz
tar czvf gameapi-builder-docker-container.tar.gz Dockerfile README.TXT run.sh gameapi-builder_1.0-$1.deb chromium
