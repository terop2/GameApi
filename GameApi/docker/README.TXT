
The following steps are needed (in ubuntu) to install:
 1) need to install docker, nvidia-container-runtime to host computer
 2) from Dockerfile, you need to create a docker container:
      sudo docker build -t builder-test:latest .
 3) ./run.sh   // this will run gameapi-builder if all goes well.  
