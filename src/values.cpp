#include "coc/values.hpp"
#include "coc/error_list.hpp"
#include <iostream>

namespace coc {
    decltype(Values::values_u.begin()) Values::at(std::string_view name) {
        auto iter = this->values_u.find(name);
        if (iter == this->values_u.end()) {
            this->run(name);
            auto iter2 = this->values_u.find(name);
            iter = iter2;
        }
        return iter;
    }
    void Values::run(std::string_view name) {
        /*
            * in this step complete:
            * Analysis of values
            */

        /*
             * after that
             * call none
             */
        auto iter = this->values.find(name);
        if (iter == this->values.end()) {
            return;
        }

        COC_ERROR_INIT

        std::string buff;

        log->valueLog(iter->second->value_log, iter->second->default_value);
        getline(std::cin, buff);
        if (buff.empty()) {
            if (iter->second->default_value.empty()) {
                error_list->add([=, this] { this->log->noValueEntered(iter->first); });
            } else {
                buff = iter->second->default_value;
            }
        }
        this->values_u[name] = buff;
    }
    Values::Values() : config(nullptr), log(nullptr) {}
    Values::~Values() {
        for (auto &p: values) {
            delete p.second;
            p.second = nullptr;
        }
    }
}// namespace coc