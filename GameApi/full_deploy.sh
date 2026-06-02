#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Usage: ./full_deploy.sh password [clean] [incremental] [steam] [noweb]"
    exit
fi
if [[ $2 == "clean" || $3 == "clean" || $4 == "clean" || $5 == "clean" ]]; then
    make clean
fi
if [[ $2 == "incremental" || $3 == "incremental" || $4 == "incremental" || $5 == "incremental" ]]; then
if [[ $2 == "steam" || $3 == "steam" || $4 == "steam" || $5 == "steam" ]]; then
   make -j 8 fast STEAM_ENABLE=yes STEAM_DEPLOY_ENABLE=yes
   (cd editor;make -f Makefile.Linux STEAM_ENABLE=yes STEAM_DEPLOY_ENABLE=yes)
else
    make -j 8 fast
    (cd editor;make -f Makefile.Linux)
fi
else
if [[ $2 == "steam" || $3 == "steam" || $4 == "steam" || $5 == "steam" ]]; then
   make -j 8 depbuild STEAM_ENABLE=yes STEAM_DEPLOY_ENABLE=yes
   (cd editor;make -f Makefile.Linux STEAM_ENABLE=yes STEAM_DEPLOY_ENABLE=yes)
else
    make -j 8 depbuild
    (cd editor;make -f Makefile.Linux)
fi       
fi
if [[ $2 == "clean" || $3 == "clean" || $4 == "clean" || $5 == "clean" ]]; then
    make -f Makefile.LinuxEm clean
fi
if [[ $2 != "noweb" && $3 != "noweb" && $4 != "noweb" && $5 != "noweb" ]]; then
    ./emmake.sh RELEASE=true
    (cd web_page;./ftp_release.sh)
    (cd display;./ftp_release.sh)
    (cd display;./ftp_package.sh $1)
    (cd php;./ftp.sh)
fi
if [[ $2 == "steam" || $3 == "steam" || $4 == "steam" || $5 == "steam" ]]; then
    echo "SKIP deploytool"
else
    (cd deploytool;./ftp_release.sh)
fi
(cd editor;./copy_files_to_deb.sh `cat ../VERSION.TXT`)
(cd editor;./make_deb.sh `cat ../VERSION.TXT`)
cp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb docker/
echo $1 |sudo -S dpkg -i editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb
#(cd docker;sudo docker build -t builder-test:latest .)
(cd docker;./build_tar.sh `cat ../VERSION.TXT`)
(cd cmdline_docker;./copy_files.sh)
(cd cmdline_docker;./build_tar.sh)
if [[ $2 == "steam" || $3 == "steam" ]]; then
    echo "SKIP pushing to servers"
else
  echo "STEP#1/9 web_page" 
  scp web_page/engine* terop@ssh.meshpage.org:/home/terop/meshpage.org/
  echo "STEP#2/9 cmdline_docker"
  scp cmdline_docker/gameapi-builder-docker-cmdline.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/gameapi-builder-docker-cmdline-`cat ./VERSION.TXT`.tar.gz
  echo "STEP#3/9 cmdline_docker2"
  scp cmdline_docker/gameapi-builder-docker-cmdline.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/gameapi-builder-docker-cmdline-`cat ./VERSION.TXT`.tar.gz
  echo "STEP#4/9 docker"
  scp docker/gameapi-builder-docker-container.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/gameapi-builder-docker-container-`cat ./VERSION.TXT`.tar.gz
  echo "STEP#5/9 docker2"
  scp docker/gameapi-builder-docker-container.tar.gz terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/gameapi-builder-docker-container-`cat ./VERSION.TXT`.tar.gz
  echo "STEP#6/9 editor"
  scp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb terop@ssh.meshpage.org:/home/terop/meshpage.org/
  echo "STEP#7/9 editor2"
  scp editor/gameapi-builder_1.0-`cat ./VERSION.TXT`.deb terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/
  echo "STEP#8/9 version.txt"
  scp ./VERSION.TXT terop@ssh.meshpage.org:/home/terop/meshpage.org/assets/
  echo "STEP#9/9 commit gameapi_display.zip to version control"
  if [[ $2 == "steam" || $3 == "steam" || $2 == "noweb" || $3 == "noweb" || $4 == "noweb" || $5 == "noweb" ]]; then
      echo "SKIP gameapi_display.zip commit"
  else
      git add display/gameapi_display.zip
      git commit -m "automatic commit"
      git push origin master
  fi
fi
echo "SUCCESS"
