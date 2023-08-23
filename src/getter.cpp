#include "coc/getter.hpp"

namespace coc{
    Getter::Getter(Options *options, Values *values, Arguments *arguments,bool is_empty) : opt(options), val(values), arg(arguments) ,is_empty(is_empty){}
    ActionFun coc_empty = [](const Getter&) { return; };
}