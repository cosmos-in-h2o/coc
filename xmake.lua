add_rules("mode.debug", "mode.release")
add_languages("c++20")

target("coc")
    set_kind("binary")
    set_targetdir("target/")
    add_files("src/*.cc")
