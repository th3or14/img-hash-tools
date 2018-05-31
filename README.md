## Description

Key frames extractor converts an input video into a serie of key frames for easier browsing.

Similar images finder helps to remove duplicates and almost duplicates from a specified pictures collection.

## Requirements

* CMake (checked version 3.5.1)

* A C++14 compliant compiler (checked GCC 5.4.0)

* OpenCV with extra modules (checked version 3.4.1)

* Qt (checked version 5.10.1)

## Running in a Docker container on Linux/X11

As an alternative to installation you can build and run this project in a Docker container. Docker is required (checked version 18.03.1-ce). Run `docker/start.sh` for a quick start. Afterwards you can use created symlinks to the built applications in the root directory of a running container and give access to input data through a shared folder.
