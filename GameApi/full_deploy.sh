#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Usage: ./full_deploy.sh password"
    exit
fi
make clean
make -j 8
(cd editor;make -f Makefile.Linux)
make -f Makefile.LinuxEm clean
./emmake.sh
(cd web_page;./ftp_release.sh)
(cd display;./ftp_release.sh)
(cd display;./ftp_package.sh $1)
(cd php;./ftp.sh)
(cd deploytool;./ftp_release.sh)
(cd editor;./copy_files_to_deb.sh `cat ../VERSION.TXT`)
(cd editor;./make_deb.sh `cat ../VERSION.TXT`)
cp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb docker/
echo $1 |sudo -S dpkg -i editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb
#(cd docker;sudo docker build -t builder-test:latest .)
(cd docker;./build_tar.sh `cat ../VERSION.TXT`)
(cd cmdline_docker;./copy_files.sh)
(cd cmdline_docker;./build_tar.sh)
echo "STEP#1/8 web_page" 
sshpass -p $1 scp web_page/engine* terop@ssh.meshpage.org:/home/terop/meshpage.org/
echo "STEP#2/8 cmdline_docker"
sshpass -p $1 scp cmdline_docker/gameapi-builder-docker-cmdline.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/gameapi-builder-docker-cmdline-`cat ./VERSION.TXT`.tar.gz
echo "STEP#3/8 cmdline_docker2"
sshpass -p $1 scp cmdline_docker/gameapi-builder-docker-cmdline.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/gameapi-builder-docker-cmdline-`cat ./VERSION.TXT`.tar.gz
echo "STEP#4/8 docker"
sshpass -p $1 scp docker/gameapi-builder-docker-container.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/gameapi-builder-docker-container-`cat ./VERSION.TXT`.tar.gz
echo "STEP#5/8 docker2"
sshpass -p $1 scp docker/gameapi-builder-docker-container.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/gameapi-builder-docker-container-`cat ./VERSION.TXT`.tar.gz
echo "STEP#6/8 editor"
sshpass -p $1 scp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb terop@ssh.meshpage.org:/home/terop/meshpage.org/
echo "STEP#7/8 editor2"
sshpass -p $1 scp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/
echo "STEP#8/8 version.txt"
sshpass -p $1 scp ./VERSION.TXT terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/
echo "SUCCESS"
