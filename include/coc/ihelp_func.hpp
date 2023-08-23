#ifndef COC_IHELP_FUNC_HPP
#define COC_IHELP_FUNC_HPP
#include <coc/parser_config.hpp>

namespace coc {
    class Parser;
    class Getter;
    struct IHelpFunc {
        ParserConfig *config;
        Parser *parser;
        virtual void run(Getter g) = 0;
        virtual ~IHelpFunc() = default;
        IHelpFunc(ParserConfig *config, Parser *parser) : config(config), parser(parser) {}
    };
}// namespace coc

#endif//COC_IHELP_FUNC_HPP
