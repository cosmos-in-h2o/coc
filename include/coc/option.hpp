#ifndef COC_OPTION_HPP
#define COC_OPTION_HPP
#include <string_view>

namespace coc {
    struct Option {
        std::string_view name, intro;
        int number;
        char short_name;
    };

}// namespace coc

#endif//COC_OPTION_HPP
