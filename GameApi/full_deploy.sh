#!/bin/sh
make clean
make -j 8
(cd editor;make -f Makefile.Linux)
make -f Makefile.LinuxEm clean
./emmake.sh
(cd web_page;./ftp_release.sh)
(cd display;./ftp_release.sh)
(cd display;./ftp_package.sh)
(cd php;./ftp.sh)
(cd deploytool;./ftp_release.sh)
(cd editor;./copy_files_to_deb.sh `cat ../VERSION.TXT`)
(cd editor;./make_deb.sh `cat ../VERSION.TXT`)
cp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb docker/
sudo dpkg -i editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb
#(cd docker;sudo docker build -t builder-test:latest .)
(cd docker;./build_tar.sh `cat ../VERSION.TXT`)
scp web_page/web_page* terop@ssh.meshpage.org:/home/terop/meshpage.org/
scp docker/gameapi-builder-docker-container.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/gameapi-builder-docker-container-`cat ./VERSION.TXT`.tar.gz
scp docker/gameapi-builder-docker-container.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/gameapi-builder-docker-container-`cat ./VERSION.TXT`.tar.gz
scp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb terop@ssh.meshpage.org:/home/terop/meshpage.org/
scp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/
scp ./VERSION.TXT terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/
echo "SUCCESS"
