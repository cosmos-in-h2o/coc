cmake_minimum_required(VERSION 3.25)
project(coc)
set(CMAKE_CXX_STANDARD 20)

add_library(coc STATIC)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
add_executable(example)
target_link_libraries(example PRIVATE coc)
if(DEFINED MODULE_BUILD)
    target_sources(coc PUBLIC module/coc/coc.ixx PRIVATE module/coc/coc.cc)
else ()
    target_include_directories(coc PUBLIC include)
    target_sources(coc PRIVATE
            src/action.cpp
            src/actions.cpp
            src/arguments.cpp
            src/error_list.cpp
            src/getter.cpp
            src/help_func.cpp
            src/options.cpp
            src/parser.cpp
            src/parser_log.cpp
            src/print_component.cpp
            src/targets.cpp
            src/values.cpp
    )
endif ()

target_sources(example PRIVATE example/example.cc)