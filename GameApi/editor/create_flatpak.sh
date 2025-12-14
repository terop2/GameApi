#!/bin/bash
flatpak install flathub org.freedesktop.Platform//23.08
flatpak install flathub org.freedesktop.Sdk//23.08
(cd gameapi.builder;dpkg-deb -x ../gameapi-builder_1.0-28.deb files/)
(cd gameapi.builder;flatpak-builder --repo=repo build-dir manifest.json --force-clean)
(cd gameapi.builder;flatpak build-bundle repo GameApiBuilder.flatpak org.meshpage.GameApiBuilder)
