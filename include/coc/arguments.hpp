#ifndef COC_ARGUMENTS_HPP
#define COC_ARGUMENTS_HPP
#include <string_view>
#include <unordered_map>
#include <coc/parser_config.hpp>
#include <coc/parser_log.hpp>

namespace coc {
    class Arguments {
        friend class Parser;

    private:
        struct Argument {
            std::string_view argument_type, intro;
        };

        ParserConfig *config;
        IParserLog *log;
        std::unordered_map<std::string_view, Argument *> arguments;
        std::unordered_map<std::string_view, std::string> arguments_u;

        //add an argument to list
        inline void addArgument(std::string_view _name, std::string_view _type, std::string_view _intro) {
            this->arguments[_name] = new Argument{_type, _intro};
        }
        void run(std::string_view argv);

    public:
        Arguments();
        ~Arguments();

        inline std::unordered_map<std::string_view, Argument *> &get_arguments_map() {
            return this->arguments;
        }
        template<typename T>
        inline T get(const std::string &name, T default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return T(p->second);
        }
        //the first is value.if the second is false,it means that the value was not found

        template<>
        inline int get<int>(const std::string &name, int default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return stoi(p->second);
        }
        template<>
        inline float get<float>(const std::string &name, float default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return stof(p->second);
        }
        template<>
        inline double get<double>(const std::string &name, double default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return stod(p->second);
        }
        template<>
        inline char get<char>(const std::string &name, char default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return p->second[0];
        }
        template<>
        inline bool get<bool>(const std::string &name, bool default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            if (p->second == "FALSE" || p->second == "False" || p->second == "false" || p->second == "0")
                return false;
            else
                return true;
        }
        template<>
        inline std::string get<std::string>(const std::string &name, std::string default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return p->second;
        }
        template<>
        inline const char *get<const char *>(const std::string &name, const char *default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return p->second.c_str();
        }
    };
}// namespace coc

#endif//COC_ARGUMENTS_HPP
