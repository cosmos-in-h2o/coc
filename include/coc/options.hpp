#ifndef COC_OPTIONS_HPP
#define COC_OPTIONS_HPP
#include <vector>
#include <list>
#include <algorithm>
#include <coc/parser_config.hpp>
#include <coc/parser_log.hpp>
#include <coc/targets.hpp>

namespace coc{
    class Options {
        friend struct Targets;
        friend class IAction;
        friend class AAction;
        friend class HelpAction;
        friend class Parser;
        friend struct PrefabHelpFunc;
        friend struct Getter;

    private:
        ParserConfig *config;
        IParserLog *log;
        std::vector<Option *> options;  //options list
        std::vector<Option *> options_u;//user options list
        Targets *targets;
        //add an option to options list
        inline void addOption(std::string_view name, std::string_view intro, int number, char short_name) {
            auto p = new Option{name, intro, number, short_name};
            this->options.push_back(p);
        }
        //add opt_tar which user input.
        void run(std::list<std::string_view> &opt_tar);

    public:
        Options();
        ~Options();
        inline std::vector<Option *> &get_options() {
            return this->options;
        }
        inline int at(int index) {
            if (index >= this->options_u.size()) {
                return -1;
            }
            return this->options_u[index]->number;
        }
        inline int operator[](int index) {
            return this->at(index);
        }
        inline bool getOption(const std::string &option) {
            return std::ranges::any_of(options_u, [&](auto iter) {
              return iter->name == option;
            });
        }
        inline bool getOption(char short_name) {
            return std::ranges::any_of(options_u, [&](auto iter) {
              return iter->short_name == short_name;
            });
        }
        bool isOnlyOpt(const std::string &option);
        bool isOnlyOpt(char short_name);
        inline std::vector<Option *> get_list() {
            return this->options_u;
        }
    };

}


#endif//COC_OPTIONS_HPP
