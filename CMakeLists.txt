cmake_minimum_required(VERSION 3.10)
project(compass)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_subdirectory(src/trayectory)
add_subdirectory(src/evolutive)
add_subdirectory(src/utils)


set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(compass src/main.cpp)

target_link_libraries(compass PRIVATE trayectory)
target_link_libraries(compass PRIVATE evolutive)
target_link_libraries(compass PRIVATE utils)
