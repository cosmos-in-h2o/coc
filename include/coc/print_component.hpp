#ifndef COC_PRINT_COMPONENT_HPP
#define COC_PRINT_COMPONENT_HPP
#include <coc/action.hpp>

namespace coc {
    struct PrintComponent {
        static void printBasicMessage(ParserConfig *_config);
        static void printOptions(Options *_options);
        static void printValues(Values *_values);
        static void printAction(IAction *_action);
        static void printActions(Parser *_parser);
        static void printArguments(Parser *_parser);
    };
}// namespace coc

#endif//COC_PRINT_COMPONENT_HPP
