#include <coc/print_component.hpp>
#include "coc/help_func.hpp"

namespace coc {
    void PrefabHelpFunc::run(coc::Getter g) {
        PrintComponent::printBasicMessage(config);
        PrintComponent::printActions(parser);
        PrintComponent::printAction(parser->get_actions()->get_global_action());
        PrintComponent::printArguments(parser);
    }
    PrefabHelpFunc::PrefabHelpFunc(ParserConfig *config, Parser *parser) : IHelpFunc(config, parser){};
}// namespace coc
