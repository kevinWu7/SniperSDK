cmake_minimum_required(VERSION 3.1)
project(minizip)

# 添加zlib的源文件（.c）到编译中
file(GLOB MINIZLIB_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/unzip.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ioapi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/miniunz.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/zip.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/minizip.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mztools.c"
) 

# 创建一个静态库
add_library(${PROJECT_NAME}  STATIC ${MINIZLIB_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/external/zlib")

if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE zlib) #在windows-mingw32上，由于minizip依赖zlib，所以需要添加link,其他平台是ok的
endif()


