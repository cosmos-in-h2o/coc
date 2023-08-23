#include <coc/error_list.hpp>
#include "coc/parser.hpp"

namespace coc {
    Parser::Parser(ParserConfig *config, IParserLog *log) : config(config), log(log) {
        this->arguments = new Arguments;
        this->actions = new Actions;
        this->arguments->config = config;
        this->arguments->log = log;
        this->actions->config = config;
        this->actions->log = log;
    }
    Parser::~Parser() {
        delete this->config;
        this->config = nullptr;
        delete this->log;
        this->log = nullptr;
        delete this->actions;
        this->actions = nullptr;
        delete this->arguments;
        this->arguments = nullptr;
    }
    void Parser::defaultConfig() {
        delete this->config;
        this->config = nullptr;
        delete this->log;
        this->log = nullptr;
    }
    void Parser::loadLog(coc::IParserLog *_log) {
        delete this->log;
        this->log = _log;
    }
    void Parser::loadConfig(coc::ParserConfig *_config) {
        delete this->config;
        this->config = _config;
    }
    int Parser::run(int argc, char **argv) {
        /*
             * in this step complete:
             * Analysis of argv except opt_tar and arguments
             * determine which is option
             * determine which is argument
             */

        /*
             * after that
             * call actions' run() or global_action's run()
             * call arguments' run()
             */
#define COC_IS_GLOBAL
#define COC_IS_COMMON
        bool is_common;
        COC_ERROR_INIT
        //optimize when empty
        if (this->actions->global != nullptr) {
            if (argc == 1) {
                COC_IS_GLOBAL
                this->actions->global->run(this->arguments);
                return error_list->invoke();
            } else {
                is_common = this->actions->havaAction(argv[1]);
            }
            if (argc == 2 && is_common) {
                COC_IS_COMMON
                this->actions->run(argv[1], this->arguments);
                return error_list->invoke();
            }
        } else {
            COC_IS_COMMON
            if (argc == 2) {
                this->actions->run(argv[1], this->arguments);
                return error_list->invoke();
            }
            is_common = true;
        }
        //analyse
        std::list<std::string_view> all_argv;//all cmd argv
        std::list<std::string_view> opt_tar;

        int i = 1;
        //if it isn't the global action, then read from index of 2
        if (is_common) i = 2;
        for (; i < argc; ++i) {
            all_argv.emplace_back(argv[i]);
        }
        for (auto &p: all_argv) {
            if (p[0] == '-') {
                if (p[1] == this->config->argument_mark) {
                    arguments->run(p);
                } else {
                    opt_tar.push_back(p);
                }
            } else {
                opt_tar.push_back(p);
            }
        }
        //execute
        if (!is_common) {
            if (this->actions->global == nullptr) {
                error_list->add([=, this] { this->log->globalActionNotDoesNotExist(); });
                return error_list->invoke();
            } else {
                this->actions->global->run(opt_tar, this->arguments);
                return error_list->invoke();
            }
        }
        this->actions->run(argv[1], opt_tar, this->arguments);
        return error_list->invoke();
    }
}// namespace coc