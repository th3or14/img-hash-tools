FROM ubuntu:16.04

# install dependencies from packages
RUN apt-get update && apt-get install -y wget unzip cmake build-essential libfontconfig libdbus-1-3 '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libavcodec-dev libavformat-dev libswscale-dev libavresample-dev libgtk2.0-dev

# install qt
RUN wget http://ftp.acc.umu.se/mirror/qt.io/qtproject/archive/qt/5.10/5.10.1/qt-opensource-linux-x64-5.10.1.run && chmod +x qt-opensource-linux-x64-5.10.1.run
COPY qt-installer-noninteractive.qs /qt-installer-noninteractive.qs
RUN ./qt-opensource-linux-x64-5.10.1.run --script qt-installer-noninteractive.qs -platform minimal --verbose && rm qt-opensource-linux-x64-5.10.1.run qt-installer-noninteractive.qs
ENV QT_XKB_CONFIG_ROOT=/usr/share/X11/xkb

# install opencv
RUN wget -O opencv-3.4.1.tar.gz https://codeload.github.com/opencv/opencv/tar.gz/3.4.1 && tar -xf opencv-3.4.1.tar.gz && rm opencv-3.4.1.tar.gz
RUN wget -O opencv_contrib-3.4.1.tar.gz https://codeload.github.com/opencv/opencv_contrib/tar.gz/3.4.1 && tar -xf opencv_contrib-3.4.1.tar.gz && rm opencv_contrib-3.4.1.tar.gz
RUN cd opencv-3.4.1 && mkdir build && cd build && cmake -DENABLE_PRECOMPILED_HEADERS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.4.1/modules .. && make -j $(nproc --all) && make install && rm -r /opencv-3.4.1 /opencv_contrib-3.4.1

# build img-hash-tools
RUN wget -O img-hash-tools-master.zip https://github.com/th3or14/img-hash-tools/archive/master.zip && unzip img-hash-tools-master.zip && rm img-hash-tools-master.zip && mv img-hash-tools-master img-hash-tools
RUN cd img-hash-tools && mkdir build && cd build && cmake .. '-GCodeBlocks - Unix Makefiles' -DCMAKE_CXX_COMPILER:STRING=/usr/bin/g++ -DCMAKE_C_COMPILER:STRING=/usr/bin/gcc -DCMAKE_PREFIX_PATH:STRING=/Qt/5.10.1/gcc_64 -DQT_QMAKE_EXECUTABLE:STRING=/Qt/5.10.1/gcc_64/bin/qmake && make -j $(nproc --all)
RUN ln -s /img-hash-tools/build/similar-images-finder/similar-images-finder /similar-images-finder && ln -s /img-hash-tools/build/key-frames-extractor/key-frames-extractor /key-frames-extractor
