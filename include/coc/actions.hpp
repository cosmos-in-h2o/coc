#ifndef COC_ACTIONS_HPP
#define COC_ACTIONS_HPP
#include <unordered_map>
#include <coc/action.hpp>

namespace coc {
    class Actions {
        friend class Parser;

    private:
        IParserLog *log;
        ParserConfig *config;
        GlobalAction *global;
        std::unordered_map<std::string_view, IAction *> actions;

        bool havaAction(const std::string &_name);
        void run(const std::string &action_name, coc::Arguments *arguments);
        void run(const std::string &action_name, std::list<std::string_view> &opt_tar, Arguments *arguments);

    public:
        Actions();
        ~Actions();

        inline IAction *addAction(std::string_view _name, std::string_view _intro, const ActionFun &_af, char _short_cut = COC_NULL_CHAR) {
            auto action = new Action(_intro, _af, _short_cut, this->config, this->log);
            this->actions[_name] = action;
            return action;
        }
        inline IAction *addHelpAction(std::string_view _name, std::string_view _intro, IHelpFunc *_hf, char _short_cut = COC_NULL_CHAR) {
            auto *action = new HelpAction(_intro, _hf, _short_cut, this->config, this->log);
            this->actions[_name] = action;
            return action;
        }
        inline std::unordered_map<std::string_view, IAction *> &get_actions_map() {
            return this->actions;
        }
        inline GlobalAction *get_global_action() {
            return this->global;
        }
    };
}// namespace coc

#endif//COC_ACTIONS_HPP