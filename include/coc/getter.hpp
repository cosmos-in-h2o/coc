#ifndef COC_GETTER_HPP
#define COC_GETTER_HPP
#include <functional>
#include <coc/arguments.hpp>
#include <coc/options.hpp>
#include <coc/values.hpp>

namespace coc {
    struct Getter {
    private:
        Options *opt;
        Values *val;
        Arguments *arg;

    public:
        Getter(Options *options, Values *values, Arguments *arguments, bool is_empty = false);
        bool is_empty;
        inline Options *get_opt() { return this->opt; }
        inline Values *get_val() { return this->val; }
        inline Arguments *get_arg() { return this->arg; }
        inline Targets *get_tar() { return this->opt->targets; }
    };
    //Action function pointer
    using ActionFun = std::function<void(Getter)>;

    extern ActionFun coc_empty;

}// namespace coc
#endif//COC_GETTER_HPP
