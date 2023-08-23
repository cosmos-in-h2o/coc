#ifndef COC_PARSER_HPP
#define COC_PARSER_HPP
#include <coc/actions.hpp>

namespace coc {
    class Parser {
    private:
        ParserConfig *config;
        IParserLog *log;
        bool is_def_hp = true;//if open default function
        Actions *actions;
        Arguments *arguments;
        void empty();

    public:
        Parser(ParserConfig *config, IParserLog *log);
        ~Parser();

        void defaultConfig();
        //over log
        void loadLog(IParserLog *_log);
        //over config
        void loadConfig(ParserConfig *_config);

        inline IAction *addAction(std::string_view _name, std::string_view _intro, const ActionFun &_af, char _short_cut = COC_NULL_CHAR) {
            return this->actions->addAction(_name, _intro, _af, _short_cut);
        }
        inline IAction *addHelpAction(std::string_view _name, std::string_view _intro, IHelpFunc *_hf, char _short_cut = COC_NULL_CHAR) {
            return this->actions->addHelpAction(_name, _intro, _hf, _short_cut);
        }
        inline Parser *addArgument(std::string_view _name, std::string_view _type, std::string_view _intro) {
            this->arguments->addArgument(_name, _type, _intro);
            return this;
        }
        //get and set function
        inline ParserConfig *get_config() {
            return this->config;
        }
        inline IParserLog *get_log() {
            return this->log;
        }
        inline IAction *set_global_action(const ActionFun &_af) {
            if (this->actions->global == nullptr)
                this->actions->global = new GlobalAction(_af, COC_NULL_CHAR, this->config, this->log);
            return this->actions->global;
        }
        inline Arguments *get_argument() {
            return this->arguments;
        }
        inline Actions *get_actions() {
            return this->actions;
        }

        int run(int argc, char **argv);
    };
}// namespace coc

#endif//COC_PARSER_HPP