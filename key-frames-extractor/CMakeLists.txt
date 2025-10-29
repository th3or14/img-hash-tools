project(key-frames-extractor)
find_package(Qt6 COMPONENTS Core REQUIRED)
add_executable(${PROJECT_NAME} key-frames-extractor.hpp key-frames-extractor.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} hash-handler Qt6::Core)
