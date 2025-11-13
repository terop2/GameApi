#!/bin/bash
make clean
make -j 8 -f Makefile STEAM_ENABLE="yes" STEAM_DEPLOY_ENABLE="yes"
