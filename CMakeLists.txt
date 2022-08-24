cmake_minimum_required(VERSION 3.20)
project(bot)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC src/*.cpp)

add_subdirectory(thirdparty/libuv)
include_directories(thirdparty/libuv/include)

add_link_options(-Wl,-rpath -Wl,LIBDIR)

add_executable(bot ${SRC})
target_link_libraries(bot uv_a)