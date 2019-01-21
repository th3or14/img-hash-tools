## Description

#### Key frames extractor

Converts an input video into a serie of key frames for easier browsing.

<p align="center">
  <img src="https://user-images.githubusercontent.com/37025359/45453867-ba5c1700-b6ea-11e8-9cae-2847bc745f14.jpg">
</p>

Usage: `./key-frames-extractor <input video filename> <output location>`

#### Similar images finder

Helps to remove duplicates and almost duplicates from a specified pictures collection.

<p align="center">
  <img src="https://user-images.githubusercontent.com/37025359/45453877-bcbe7100-b6ea-11e8-8080-b601d207fc94.jpg">
</p>

## Requirements

* CMake (checked version 3.5.1)

* A C++14 compliant compiler (checked GCC 5.4.0)

* OpenCV with extra modules (checked version 3.4.1)

* Qt (checked version 5.10.1)

## Running in a Docker container on Linux/X11

As an alternative to installation you can build and run this project in a Docker container. Docker is required (checked version 18.03.1-ce). Run `docker/start.sh` for a quick start. Afterwards you can use created symlinks to the built applications in the root directory of a running container and give access to input data through a shared folder.

## Known issues

#### Similar images finder: window content is transparent on first start in a Docker container

Resize the window or restart the application.
