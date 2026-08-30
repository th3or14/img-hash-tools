[![Linux Docker](https://github.com/th3or14/img-hash-tools/actions/workflows/linux-docker.yml/badge.svg)](https://github.com/th3or14/img-hash-tools/actions/workflows/linux-docker.yml)

[![Windows Build](https://github.com/th3or14/img-hash-tools/actions/workflows/windows-build.yml/badge.svg)](https://github.com/th3or14/img-hash-tools/actions/workflows/windows-build.yml)

## Description

#### Key frames extractor

Converts an input video into a serie of key frames for easier browsing.

<p align="center">
  <img src="https://user-images.githubusercontent.com/37025359/45453867-ba5c1700-b6ea-11e8-9cae-2847bc745f14.jpg">
</p>

Usage: `./key-frames-extractor -i <input video filename> -o <output directory>`

#### Similar images finder

Helps to remove duplicates and almost duplicates from a specified pictures collection.

<p align="center">
  <img src="https://user-images.githubusercontent.com/37025359/88987759-93f3f480-d2df-11ea-9a54-7fa39a72ffcd.png">
</p>

## Requirements

* CMake 3.16+

* A C++17 compliant compiler

* OpenCV compiled with FFmpeg and extra module img_hash

* Qt 6

## Building

Use `CMakeLists.txt` from the top directory. You may refer to the workflows in `.github/workflows`. On Linux/X11 you can also build and run this project in a Docker container. Then Docker is required. Run the script `docker-start.sh` for a quick start. Afterwards, you can run the built executables `build/similar-images-finder/similar-images-finder` and `build/key-frames-extractor/key-frames-extractor` in the running container. Input data in the running container can be accessed via the shared folder `shared-folder`, which will be mounted by the script `docker-start.sh` to this repository on your host.
