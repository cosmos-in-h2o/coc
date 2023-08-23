#include <coc/error_list.hpp>
#include <coc/def.hpp>
#include "coc/options.hpp"

namespace coc {
    void Options::run(std::list<std::string_view> &opt_tar) {
        /*
            * in this step complete:
            * Analysis of opt_tar
            */

        /*
             * after that
             * call none
             */
        COC_ERROR_INIT
        for (auto str: opt_tar) {
            if (str[0] == '-') {
                if (str[1] == '-') {
                    bool error = true;
                    std::string_view temp = str.substr(2, str.size() - 2);
                    //if the 2nd char is -
                    for (auto iter_opt: this->options) {
                        //find right option
                        if (iter_opt->name == temp) {
                            error = false;
                            this->options_u.push_back(iter_opt);//add option ptr to options_u
                            this->targets->run(iter_opt);
                        }
                    }
                    if (error)
                        error_list->add([=, this] { this->log->unidentifiedOption(temp.data()); });
                } else {
                    bool error;
                    std::string_view options_str = str.substr(1, str.size() - 1);
                    for (auto iter_str: options_str) {
                        error = true;
                        for (auto iter_opt: this->options) {
                            if (iter_opt->short_name != COC_NULL_CHAR && iter_str == iter_opt->short_name) {
                                this->options_u.push_back(iter_opt);//add option ptr to options_u
                                this->targets->run(iter_opt);
                                error = false;
                            }
                        }
                        if (error)
                            error_list->add([=, this] { this->log->unidentifiedOption(iter_str); });
                    }
                }
            } else {
                this->targets->run(str);
            }
        }
    }
    Options::Options() : config(nullptr), log(nullptr), targets(new Targets()) {}
    Options::~Options() {
        delete this->targets;
        this->targets = nullptr;
        for (auto &p: this->options) {
            delete p;
            p = nullptr;
        }
    }
    bool Options::isOnlyOpt(const std::string &option) {
        if (this->options_u.size() == 1) {
            for (auto iter: this->options_u) {
                if (iter->name == option)
                    return true;
            }
        }
        return false;
    }
    bool Options::isOnlyOpt(char short_name) {
        if (this->options_u.size() == 1) {
            for (auto iter: this->options_u) {
                if (iter->short_name == short_name)
                    return true;
            }
        }
        return false;
    }
}// namespace coc