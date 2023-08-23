#include "coc/arguments.hpp"
#include "coc/error_list.hpp"

namespace coc {
    void Arguments::run(std::string_view argv) {
        /*
            * in this step complete:
            * Analysis of argument
            */

        /*
             * after that
             * call none
             */
        COC_ERROR_INIT

        std::string_view key, value, str = argv.substr(2, argv.size() - 2);
        auto pos = str.find('=');
        if (pos != std::string::npos) {
            key = str.substr(0, pos);
            value = str.substr(pos + 1);
        }

        if (config->argument_need_extern) {
            auto p = this->arguments.find(key);
            if (p != this->arguments.end()) {
                this->arguments_u[key] = value;
            } else {
                error_list->add([=, this] { this->log->unidentifiedArgument(key); });
            }
        } else {
            this->arguments_u[key] = value;
        }
    }
    Arguments::Arguments() : config(nullptr), log(nullptr) {}
    Arguments::~Arguments() {
        for (auto &p: this->arguments) {
            delete p.second;
            p.second = nullptr;
        }
    }
}// namespace coc