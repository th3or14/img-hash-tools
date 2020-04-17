FROM ubuntu:16.04

# install dependencies from packages
RUN apt-get update && apt-get install -y wget unzip cmake build-essential libfontconfig libdbus-1-3 '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libavcodec-dev libavformat-dev libswscale-dev libavresample-dev libgtk2.0-dev qtbase5-dev qt5-qmake

# install opencv
RUN wget -O opencv-4.2.0.tar.gz https://codeload.github.com/opencv/opencv/tar.gz/4.2.0 && tar -xf opencv-4.2.0.tar.gz && rm opencv-4.2.0.tar.gz
RUN wget -O opencv_contrib-4.2.0.tar.gz https://codeload.github.com/opencv/opencv_contrib/tar.gz/4.2.0 && tar -xf opencv_contrib-4.2.0.tar.gz && rm opencv_contrib-4.2.0.tar.gz
RUN mkdir only_img_hash_module && mv opencv_contrib-4.2.0/modules/img_hash only_img_hash_module && rm -r opencv_contrib-4.2.0 && cd opencv-4.2.0 && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DOPENCV_EXTRA_MODULES_PATH=../../only_img_hash_module .. && make -j $(nproc --all) && make install && cd ../.. && rm -r opencv-4.2.0 only_img_hash_module

# build img-hash-tools
RUN wget -O img-hash-tools-master.zip https://github.com/th3or14/img-hash-tools/archive/master.zip && unzip img-hash-tools-master.zip && rm img-hash-tools-master.zip && mv img-hash-tools-master img-hash-tools
RUN cd img-hash-tools && mkdir build && cd build && cmake .. '-GCodeBlocks - Unix Makefiles' -DCMAKE_CXX_COMPILER:STRING=$(which g++) -DCMAKE_C_COMPILER:STRING=$(which gcc) -DCMAKE_PREFIX_PATH:STRING=$(which gcc) -DQT_QMAKE_EXECUTABLE:STRING=$(which qmake) && make -j $(nproc --all)
RUN ln -s /img-hash-tools/build/similar-images-finder/similar-images-finder /similar-images-finder && ln -s /img-hash-tools/build/key-frames-extractor/key-frames-extractor /key-frames-extractor
