cmake_minimum_required(VERSION 3.25)
project(coc)

# 指定使用的编译器为Clang
set(CMAKE_CXX_COMPILER "clang++")

# 添加C++20标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-cache-path=./build/.gens/coc/linux/x86_64/release/rules/modules/cache/a5d32aaa/")

# 添加C++模块项目
add_executable(coc src/coc.ixx src/example.cc )