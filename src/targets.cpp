#include "coc/targets.hpp"

namespace coc {
    void Targets::run(std::string_view target) {
        if (this->targets_list.empty())
            this->first->target_list.push_back(target);
        else
            this->targets_list.back()->target_list.push_back(target);
    }
    Targets::Targets() {
        this->first = new Target();
    }
    Targets::~Targets() {
        delete this->first;
        this->first = nullptr;
        for (auto &p: this->targets_list) {
            delete p;
            p = nullptr;
        }
    }
    std::string_view Targets::at(int index, const std::string &_default) {
        if (index < this->first->target_list.size()) {
            return this->first->target_list[index];
        }
        return _default;
    }
    std::string_view Targets::atAbsoluteIndex(int index, const std::string &_default) {
        if (index >= this->first->target_list.size()) {
            index -= static_cast<int>(this->first->target_list.size());
        } else {
            return this->first->target_list[index];
        }
        for (auto iter: this->targets_list) {
            if (index >= iter->target_list.size()) {
                index -= static_cast<int>(iter->target_list.size());
            } else {
                return iter->target_list[index];
            }
        }
        return _default;
    }
    std::string_view Targets::at(const std::string &option_name, int index, const std::string &_default) {
        for (auto iter: this->targets_list) {
            if (iter->option->name == option_name) {
                if (index < iter->target_list.size()) {
                    return iter->target_list[index];
                }
                break;
            }
        }
        return _default;
    }
    std::string_view Targets::atOutOfRange(const std::string &option_name, int index, const std::string &_default) {
        auto iter = this->targets_list.begin();
        for (; iter < this->targets_list.end(); ++iter) {
            if (iter.operator*()->option->name == option_name) {
                break;
            }
        }
        for (; iter < this->targets_list.end(); ++iter) {
            if (index >= iter.operator*()->target_list.size()) {
                index -= static_cast<int>(iter.operator*()->target_list.size());
            } else {
                return iter.operator*()->target_list[index];
            }
        }
        return _default;
    }
}// namespace coc