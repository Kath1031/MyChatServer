cmake_minimum_required(VERSION 3.0)
project(main)

# set(CMAKE_BUILD_TYPE "Debug")#设置debug模式，如果没有这一行将不能调试设断点
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)


# 配置头文件搜索路径
# include_directories()

# 配置库文件搜索路径
# link_directories()

# 设置需要编译的源文件列表
set(SRC_LIST ./muduo_server.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(server ${SRC_LIST})

target_link_libraries(server muduo_net muduo_base pthread)
