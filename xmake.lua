add_rules("mode.debug", "mode.release")
set_languages("c++23")

target("coc")
    set_kind("binary")
    add_files("src/main.cpp")
    add_files("src/*.ixx")