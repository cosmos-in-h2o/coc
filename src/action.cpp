#include "coc/action.hpp"

namespace coc {
    IAction::IAction(coc::Options *options, coc::Values *values, char short_cut, coc::ParserConfig *config, coc::IParserLog *log) : options(options), values(values), short_cut(short_cut) {
        this->options->config = config;
        this->options->log = log;
        this->values->config = config;
        this->values->log = log;
    }

    HelpAction::HelpAction(std::string_view intro, coc::IHelpFunc *hf, char short_cut, coc::ParserConfig *config, coc::IParserLog *log) : IAction(new Options(), new Values(), short_cut, config, log) {
        this->intro = intro;
        this->hf = hf;
    }
    HelpAction::~HelpAction() {
        delete this->hf;
        hf = nullptr;
    }

    AAction::AAction(const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log) : IAction(new Options(), new Values(), short_cut, config, log) {
        this->af = af;
    }
    AAction::~AAction() {
        delete this->options;
        this->options = nullptr;
        delete this->values;
        this->values = nullptr;
    }

    Action::Action(std::string_view intro, const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log) : AAction(af, short_cut, config, log) {
        this->intro = intro;
    }
}// namespace coc