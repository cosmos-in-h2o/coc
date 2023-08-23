#ifndef COC_TARGETS_HPP
#define COC_TARGETS_HPP
#include <vector>
#include <string_view>
#include <coc/option.hpp>

namespace coc {
    class Targets {
        friend class Options;

    private:
        struct Target {
            Option *option;
            std::vector<std::string_view> target_list;
            explicit Target(Option *opt)
                : option(opt) {}
            Target() : option(nullptr) {}
        };
        Target *first;
        std::vector<Target *> targets_list;
        inline void run(Option *opt) {
            this->targets_list.push_back(new Target(opt));
        }
        void run(std::string_view target);

    public:
        Targets();
        ~Targets();

        std::string_view at(int index, const std::string &_default);
        std::string_view atAbsoluteIndex(int index, const std::string &_default);
        std::string_view at(const std::string &option_name, int index, const std::string &_default);
        std::string_view atOutOfRange(const std::string &option_name, int index, const std::string &_default);

        inline size_t size() {
            size_t result = this->first->target_list.size();
            for (auto iter: this->targets_list) {
                result += iter->target_list.size();
            }
            return result;
        }
        inline size_t size_f() {
            return this->first->target_list.size();
        }
        inline size_t size(const std::string &option_name) {
            for (auto iter: this->targets_list) {
                if (iter->option->name == option_name) {
                    return iter->target_list.size();
                }
            }
            return 0;
        }
    };
}// namespace coc

#endif//COC_TARGETS_HPP
