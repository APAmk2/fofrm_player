cmake_minimum_required(VERSION 3.7)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set (CMAKE_CXX_STANDARD 17)

project(fofrm_player)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(fofrm_player "src/fofrm_player.cpp" info.rc)
target_link_libraries(fofrm_player ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})