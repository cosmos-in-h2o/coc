add_rules("mode.debug", "mode.release")
set_languages("c++23")

target("coc")
    add_toolchains("clang")
    set_kind("binary")
    add_files("src/example.cc")
    add_files("src/coc.ixx")