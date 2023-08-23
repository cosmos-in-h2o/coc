#include "coc/parser_log.hpp"
#include <iostream>

namespace coc {
    void PrefabParserLog::unidentifiedArgument(std::string_view argument) {
        std::cout << "Error:Unidentified argument:" << argument << ".\n";
    }
    void PrefabParserLog::unidentifiedOption(std::string_view option) {
        std::cout << "Error:Unidentified option:--" << option << ".\n";
    }
    void PrefabParserLog::unidentifiedOption(char option) {
        std::cout << "Error:Unidentified option:-" << option << ".\n";
    }
    void PrefabParserLog::noValueEntered(std::string_view value) {
        std::cout << "Error:Value: " << value << " not assigned.\n";
    }
    void PrefabParserLog::notFoundAction(std::string_view action) {
        std::cout << "Error:Not found action:" << action << ".\n";
    }
    void PrefabParserLog::valueLog(std::string_view value_log, std::string_view default_value) {
        if (!default_value.empty())
            std::cout << value_log << "(default=" << default_value << "):\n";
        else
            std::cout << value_log << ":\n";
    }
    void PrefabParserLog::globalActionNotDoesNotExist() {
        std::cout << "Error:Global action doesn't exist.\n";
    }
}// namespace coc