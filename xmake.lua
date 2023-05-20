add_rules("mode.debug", "mode.release")

target("coc")
    add_toolchains("clang")
    set_kind("binary")
    set_targetdir("target/")
    add_files("src/example.cc")
    add_files("src/coc.ixx")