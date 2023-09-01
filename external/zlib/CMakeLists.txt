cmake_minimum_required(VERSION 3.1)
project(zlib)

# 添加zlib的源文件（.c）到编译中
file(GLOB ZLIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c") 

# 创建一个静态库
add_library(${PROJECT_NAME}  STATIC ${ZLIB_SOURCES})
