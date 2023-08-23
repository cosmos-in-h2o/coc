#ifndef COC_HELP_FUNC_HPP
#define COC_HELP_FUNC_HPP
#include <coc/parser.hpp>

namespace coc {
    struct PrefabHelpFunc : public IHelpFunc {
        void run(Getter g) override;
        PrefabHelpFunc(ParserConfig *config, Parser *parser);
    };
}// namespace coc

#endif//COC_HELP_FUNC_HPP
