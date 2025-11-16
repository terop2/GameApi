#!/bin/bash
(cd gameapi.builder;dpkg-deb -x ../gameapi-builder_1.0-28.deb files/)
(cd gameapi.builder;flatpak-builder build-dir manifest.json --force-clean)
