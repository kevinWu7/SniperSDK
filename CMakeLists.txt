cmake_minimum_required (VERSION 3.1) #最低要求的CMake版本
project(SniperSDK) # 项目名称

file(GLOB SRC_FILE 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/util/*.cpp"
) # 建立变量SRC_FILE为目录下cpp文件列表

add_definitions(-DASIO_STANDALONE) #设置asio的独立版，不需要boost

set(CMAKE_CXX_STANDARD 17) # 指定为C++11 版本
add_executable (${PROJECT_NAME} ${SRC_FILE}) # 要求编译可执行文件

# 为可执行文件目标设置头文件搜索路径
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/external/asio"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include/util"

)

#让cmake进入zlib和minizip目录去寻找对应的CMakeLists进行构建
add_subdirectory(external/zlib)
add_subdirectory(external/minizip)

target_link_libraries(${PROJECT_NAME} PRIVATE zlib)   # 链接zlib库
target_link_libraries(${PROJECT_NAME} PRIVATE minizip) # 链接minizip库

#因为asio跨平台需要用到各个平台的网络库，所以要加上相关链接
if (CMAKE_SYSTEM_NAME STREQUAL "Linux") 
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()
