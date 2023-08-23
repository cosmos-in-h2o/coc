#ifndef COC_VALUES_HPP
#define COC_VALUES_HPP
#include <unordered_map>
#include <string_view>
#include <coc/parser_config.hpp>
#include <coc/parser_log.hpp>

namespace coc {
    class Values {
        friend class IAction;
        friend class AAction;
        friend class HelpAction;

    private:
        struct Value {
            std::string_view value_name, value_log, value_type, intro, default_value;
        };
        std::unordered_map<std::string_view, Value *> values;
        std::unordered_map<std::string_view, std::string> values_u;
        ParserConfig *config;
        IParserLog *log;

        //add a value to list
        inline void addValue(std::string_view name, std::string_view val_log, std::string_view type, std::string_view intro, std::string_view def_val) {
            this->values[name] = new Values::Value{name, val_log, type, intro, def_val};
        }
        //put run and get value that user input
        void run(std::string_view name);
        decltype(values_u.begin()) at(std::string_view name);

    public:
        Values();
        ~Values();
        inline std::unordered_map<std::string_view, Value *> &get_values() {
            return this->values;
        }

        template<typename T>
        inline T get(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return T{};
            }
            return T(iter->second);
        }
        //the first is value.if the second is false,it means that the value was not found
        template<>
        inline int get<int>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return int{};
            }
            return stoi(iter->second);
        }
        template<>
        inline float get<float>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return float{};
            }
            return stof(iter->second);
        }
        template<>
        inline double get<double>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return double{};
            }
            return stod(iter->second);
        }
        template<>
        inline char get<char>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return char{};
            }
            return iter->second[0];
        }
        template<>
        inline bool get<bool>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return bool{};
            }
            std::string_view buff = iter->second;
            if (buff == "FALSE" || buff == "False" || buff == "false" || buff == "0")
                return false;
            else
                return true;
        }
        template<>
        inline std::string get<std::string>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return std::string{};
            }
            return iter->second;
        }
        template<>
        inline const char *get<const char *>(const std::string &name) {
            auto iter = this->at(name);
            if (iter == this->values_u.end()) {
                return nullptr;
            }
            return iter->second.c_str();
        }
    };
}// namespace coc

#endif//COC_VALUES_HPP
