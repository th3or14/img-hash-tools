[![Docker Image CI](https://github.com/th3or14/img-hash-tools/actions/workflows/docker-image.yml/badge.svg)](https://github.com/th3or14/img-hash-tools/actions/workflows/docker-image.yml)

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
  <img src="https://user-images.githubusercontent.com/37025359/88987759-93f3f480-d2df-11ea-9a54-7fa39a72ffcd.png">
</p>

## Requirements

* CMake 3.7+

* A C++14 compliant compiler

* OpenCV compiled with FFmpeg and extra module img_hash

* Qt 6

## Building

Use `CMakeLists.txt` from the top directory. On Linux/X11 you can also build and run this project in a Docker container. Then Docker is required. Run `docker/start.sh` for a quick start. Afterwards you can use created symlinks to the built applications in the root directory of a running container. Input data can be accessed via a mounted shared folder.
