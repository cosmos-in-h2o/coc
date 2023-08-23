#ifndef COC_PARSER_CONFIG_HPP
#define COC_PARSER_CONFIG_HPP
#include <string>

namespace coc {
    struct ParserConfig {
        bool intellisense_mode = true;//not supported now
        bool argument_need_extern = true;
        std::string logo_and_version =
                "coc v1.2.0\n"
                "\t  ____ ___   ____ \n"
                "\t / ___/ _ \\ / ___|\n"
                "\t| |  | | | | |    \n"
                "\t| |__| |_| | |___ \n"
                "\t \\____\\___/ \\____|\n"
                "\t\t\tcoc by cosmos-in-h2o\n"
                "==> https://github.com/cosmos-in-h2o/coc";//your app's version
        std::string introduce;
        std::string usage;
        char argument_mark = 'D';//-[argument_mark] mark as argument
    };
}// namespace coc

#endif//COC_PARSER_CONFIG_HPP