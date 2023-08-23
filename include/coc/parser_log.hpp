#ifndef COC_PARSER_LOG_HPP
#define COC_PARSER_LOG_HPP
#include <string_view>

namespace coc{

    struct IParserLog {
        virtual ~IParserLog() = default;
        virtual void unidentifiedArgument(std::string_view argument) = 0;
        virtual void unidentifiedOption(std::string_view option) = 0;
        virtual void unidentifiedOption(char option) = 0;
        virtual void noValueEntered(std::string_view value) = 0;
        virtual void notFoundAction(std::string_view action) = 0;
        virtual void valueLog(std::string_view value_log, std::string_view default_value) = 0;
        virtual void globalActionNotDoesNotExist() = 0;
    };

    struct PrefabParserLog : public IParserLog {
        ~PrefabParserLog() override = default;
        void unidentifiedArgument(std::string_view argument) override;
        void unidentifiedOption(std::string_view option) override;
        void unidentifiedOption(char option) override;
        void noValueEntered(std::string_view value) override;
        void notFoundAction(std::string_view action) override;
        void valueLog(std::string_view value_log, std::string_view default_value) override;
        void globalActionNotDoesNotExist() override;
    };
}

#endif//COC_PARSER_LOG_HPP