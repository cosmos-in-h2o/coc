#include <iostream>
#include <coc/parser.hpp>
#include "coc/print_component.hpp"

namespace coc {
    void PrintComponent::printBasicMessage(coc::ParserConfig *_config) {

        std::cout << _config->logo_and_version << '\n';
        if (!_config->introduce.empty()) {
            std::cout << _config->introduce << '\n';
        }
        if (!_config->usage.empty()) {
            std::cout << _config->usage << '\n';
        }
    }
    void PrintComponent::printOptions(coc::Options *_options) {

        if (!_options->get_options().empty()) {
            std::cout << "Options(Global):\n";
            for (auto p: _options->get_options()) {
                if (p->short_name != COC_NULL_CHAR)
                    std::cout << "\t-" << p->short_name << ',';
                else
                    std::cout << '\t' << "   ";
                std::cout << "--" << p->name << '\t' << p->intro << '\n';
            }
        }
    }
    void PrintComponent::printValues(coc::Values *_values) {
        if (!_values->get_values().empty()) {
            std::cout << "Values(Global):\n";
            for (auto [k, v]: _values->get_values()) {
                std::cout << '\t' << k << "\tType:" << v->value_type << '\t' << v->intro;
                if (!v->default_value.empty()) {
                    std::cout << "\tDefault=" << v->default_value;
                }
                std::cout << '\n';
            }
        }
    }
    void PrintComponent::printAction(coc::IAction *_action) {
        PrintComponent::printOptions(_action->options);
        PrintComponent::printValues(_action->values);
    }
    void PrintComponent::printActions(coc::Parser *_parser) {
        if (!_parser->get_actions()->get_actions_map().empty()) {
            std::cout << "Actions:\n";
            for (auto &[k, v]: _parser->get_actions()->get_actions_map()) {
                if (v->short_cut != COC_NULL_CHAR)
                    std::cout << '\t' << v->short_cut << ',';
                else
                    std::cout << '\t' << "  ";
                std::cout << k << '\t' << v->get_intro() << '\n';
            }
        }
    }
    void PrintComponent::printArguments(coc::Parser *_parser) {
        if (!_parser->get_argument()->get_arguments_map().empty()) {
            std::cout << "Arguments:\n";
            for (auto &[k, v]: _parser->get_argument()->get_arguments_map()) {
                std::cout << '\t' << k << "\tType:" << v->argument_type << '\t' << v->intro << '\n';
            }
        }
    }
}// namespace coc