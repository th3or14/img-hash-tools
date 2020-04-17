#!/bin/bash

IMAGE_NAME=repository/img-hash-tools
CONTAINER_NAME=img-hash-tools
MOUNT_SRC=`pwd`
MOUNT_TARGET=/shared-folder

docker build -t $IMAGE_NAME .
docker rm -f $CONTAINER_NAME > /dev/null 2>&1
xhost +local:`docker inspect --format='{{ .Config.Hostname }}' $CONTAINER_NAME 2> /dev/null`
docker run -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --mount src=$MOUNT_SRC,target=$MOUNT_TARGET,type=bind --name $CONTAINER_NAME $IMAGE_NAME
