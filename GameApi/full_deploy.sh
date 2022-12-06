#!/bin/sh
make clean
make -j 8
(cd editor;make -f Makefile.Linux)
make -f Makefile.LinuxEm clean
./emmake.sh
(cd web_page;./ftp_release.sh)
(cd display;./ftp_release.sh)
(cd display;./ftp_package.sh)
(cd editor;./copy_files_to_deb.sh 27)
(cd editor;./make_deb.sh 27)
cp editor/gameapi-builder_1.0-27.deb docker/
(cd docker;sudo docker build -t builder-test:latest .)
(cd docker;./build_tar.sh)
scp docker/gameapi-builder-docker-container.tar.gz terop@meshpage.org:/home/terop/html/meshpage.org/
scp docker/gameapi-builder-docker-container.tar.gz terop@meshpage.org:/home/terop/html/tpgames.org/
scp editor/gameapi-builder_1.0-27.deb terop@meshpage.org:/home/terop/html/meshpage.org/
scp editor/gameapi-builder_1.0-27.deb terop@meshpage.org:/home/terop/html/tpgames.org/
echo "SUCCESS"
