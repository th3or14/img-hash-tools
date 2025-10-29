FROM ubuntu:24.04

ARG BUILD_ARG_OPENCV_VERSION=4.12.0

RUN apt-get update && \
apt-get install -y wget unzip cmake build-essential libfontconfig libdbus-1-3 '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libavcodec-dev libavformat-dev libswscale-dev libgtk2.0-dev qt6-base-dev qmake6 && \
wget -O opencv-$BUILD_ARG_OPENCV_VERSION.tar.gz https://codeload.github.com/opencv/opencv/tar.gz/$BUILD_ARG_OPENCV_VERSION && \
tar -xf opencv-$BUILD_ARG_OPENCV_VERSION.tar.gz && \
rm opencv-$BUILD_ARG_OPENCV_VERSION.tar.gz && \
wget -O opencv_contrib-$BUILD_ARG_OPENCV_VERSION.tar.gz https://codeload.github.com/opencv/opencv_contrib/tar.gz/$BUILD_ARG_OPENCV_VERSION && \
tar -xf opencv_contrib-$BUILD_ARG_OPENCV_VERSION.tar.gz && \
rm opencv_contrib-$BUILD_ARG_OPENCV_VERSION.tar.gz && \
mkdir only_img_hash_module && \
mv opencv_contrib-$BUILD_ARG_OPENCV_VERSION/modules/img_hash only_img_hash_module && \
rm -r opencv_contrib-$BUILD_ARG_OPENCV_VERSION && \
cd opencv-$BUILD_ARG_OPENCV_VERSION && \
mkdir build && \
cd build && \
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DOPENCV_EXTRA_MODULES_PATH=../../only_img_hash_module .. && \
make -j $(nproc --all) && \
make install && \
cd ../.. && \
rm -r opencv-$BUILD_ARG_OPENCV_VERSION only_img_hash_module && \
wget -O img-hash-tools-master.zip https://github.com/th3or14/img-hash-tools/archive/master.zip && \
unzip img-hash-tools-master.zip && \
rm img-hash-tools-master.zip && \
mv img-hash-tools-master img-hash-tools && \
cd img-hash-tools && \
mkdir build && \
cd build && \
cmake .. '-GCodeBlocks - Unix Makefiles' -DCMAKE_CXX_COMPILER:STRING=$(which g++) -DCMAKE_C_COMPILER:STRING=$(which gcc) -DCMAKE_PREFIX_PATH:STRING=$(which gcc) && \
make -j $(nproc --all) && \
cd ../.. && \
ln -s img-hash-tools/build/similar-images-finder/similar-images-finder similar-images-finder && \
ln -s img-hash-tools/build/key-frames-extractor/key-frames-extractor key-frames-extractor
