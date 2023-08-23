#include <coc/error_list.hpp>
#include "coc/actions.hpp"

namespace coc {
    bool Actions::havaAction(const std::string &_name) {
        if (_name.size() == 1) {
            bool result = false;
            for (auto &[k, v]: this->actions) {
                if (v->short_cut != COC_NULL_CHAR && v->short_cut == _name[0])
                    result = true;
            }
            return result;
        }
        if (this->actions.find(_name) == this->actions.end())
            return false;
        return true;
    }
    void Actions::run(const std::string &action_name, Arguments *arguments) {
        COC_ERROR_INIT
        if (action_name.size() == 1) {
            for (auto &[k, v]: this->actions) {
                //if it has shortcut and the action name equal to shortcut
                if (v->short_cut != COC_NULL_CHAR && action_name[0] == v->short_cut) {
                    return v->run(arguments);
                }
            }
        }
        auto p = this->actions.find(action_name);
        if (p == this->actions.end()) {
            return error_list->add([=, this] { this->log->notFoundAction(action_name); });
        }
        p->second->run(arguments);
    }
    void Actions::run(const std::string &action_name, std::list<std::string_view> &opt_tar, Arguments *arguments) {
        /*
            * in this step complete:
            * find the designated action
            */

        /*
             * after that
             * call action's run()
             */
        COC_ERROR_INIT
        //if it's only one letter ,then it maybe shortcut
        if (action_name.size() == 1) {
            for (auto &[k, v]: this->actions) {
                //if it has shortcut and the action name equal to shortcut
                if (v->short_cut != COC_NULL_CHAR && action_name[0] == v->short_cut)
                    return v->run(opt_tar, arguments);
            }
        }
        auto p = this->actions.find(action_name);
        if (p == this->actions.end()) {
            return error_list->add([=, this] { this->log->notFoundAction(action_name); });
        }
        return p->second->run(opt_tar, arguments);
    }
    Actions::Actions() : log(nullptr), config(nullptr), global(nullptr), actions(std::unordered_map<std::string_view, IAction *>()) {}
    Actions::~Actions() {
        for (auto &p: this->actions) {
            delete p.second;
            p.second = nullptr;
        }
        delete this->global;
        this->global = nullptr;
    }
}// namespace coc