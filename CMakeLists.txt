cmake_minimum_required(VERSION 3.16)
project(VerletPlayground LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(VerletPlayground
    src/main.cpp
    src/World.cpp
    src/Renderer.cpp
    src/InputController.cpp
)

target_include_directories(VerletPlayground PRIVATE include)
target_link_libraries(VerletPlayground PRIVATE sfml-graphics sfml-window sfml-system)
