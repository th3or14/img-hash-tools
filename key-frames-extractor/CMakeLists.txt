project(key-frames-extractor)
find_package(Qt5 COMPONENTS Core REQUIRED)
add_executable(${PROJECT_NAME} demo.hpp demo.cpp main.cpp)
target_link_libraries(${PROJECT_NAME} kernel Qt5::Core)
