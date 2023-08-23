#ifndef COC_ERROR_LIST_HPP
#define COC_ERROR_LIST_HPP
#include <functional>
#include <list>
#define COC_ERROR_INIT \
    ErrorList *error_list = ErrorList::get_single();

namespace coc {
    struct ErrorList {
    private:
        using error = std::function<void(void)>;
        std::list<error> error_list;

    public:
        static ErrorList *single;
        static ErrorList *get_single();
        inline void add(const error &_error) {
            this->error_list.push_back(_error);
        }
        int invoke();
        ErrorList() = default;
        ~ErrorList() = default;
    };
}// namespace coc

#endif//COC_ERROR_LIST_HPP