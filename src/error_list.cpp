#include <iostream>
#include "coc/error_list.hpp"

namespace coc {
    ErrorList *ErrorList::get_single() {
        if (single == nullptr) {
            single = new ErrorList;
        }
        return single;
    }

    int ErrorList::invoke() {
        int result = -1;
        if (this->error_list.empty())
            result = 0;
        else if (this->error_list.size() == 1)
            std::cout << "\n\033[31mThere is 1 error:\033[0m\n";
        else
            std::cout << "\n\033[31mThere are " << this->error_list.size() << " errors:\033[0m\n";
        for (auto &iter: this->error_list) {
            iter();
        }
        delete ErrorList::single;
        ErrorList::single = nullptr;
        return result;
    }
    ErrorList *ErrorList::single = nullptr;
}