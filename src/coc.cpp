#include "coc/coc.hpp"

#define COC_ERROR_INIT \
    ErrorList *error_list = ErrorList::get_single();

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

    void Options::run(std::list<std::string_view> &opt_tar) {
        /*
            * in this step complete:
            * Analysis of opt_tar
            */

        /*
             * after that
             * call none
             */
        COC_ERROR_INIT
        for (auto str: opt_tar) {
            if (str[0] == '-') {
                if (str[1] == '-') {
                    bool error = true;
                    std::string_view temp = str.substr(2, str.size() - 2);
                    //if the 2nd char is -
                    for (auto iter_opt: this->options) {
                        //find right option
                        if (iter_opt->name == temp) {
                            error = false;
                            this->options_u.push_back(iter_opt);//add option ptr to options_u
                            this->targets->run(iter_opt);
                        }
                    }
                    if (error)
                        error_list->add([=, this] { this->log->unidentifiedOption(temp.data()); });
                } else {
                    bool error;
                    std::string_view options_str = str.substr(1, str.size() - 1);
                    for (auto iter_str: options_str) {
                        error = true;
                        for (auto iter_opt: this->options) {
                            if (iter_opt->short_name != COC_NULL_CHAR && iter_str == iter_opt->short_name) {
                                this->options_u.push_back(iter_opt);//add option ptr to options_u
                                this->targets->run(iter_opt);
                                error = false;
                            }
                        }
                        if (error)
                            error_list->add([=, this] { this->log->unidentifiedOption(iter_str); });
                    }
                }
            } else {
                this->targets->run(str);
            }
        }
    }
    Options::Options() : config(nullptr), log(nullptr), targets(new Targets()) {}
    Options::~Options() {
        delete this->targets;
        this->targets = nullptr;
        for (auto &p: this->options) {
            delete p;
            p = nullptr;
        }
    }
    bool Options::isOnlyOpt(const std::string &option) {
        if (this->options_u.size() == 1) {
            for (auto iter: this->options_u) {
                if (iter->name == option)
                    return true;
            }
        }
        return false;
    }
    bool Options::isOnlyOpt(char short_name) {
        if (this->options_u.size() == 1) {
            for (auto iter: this->options_u) {
                if (iter->short_name == short_name)
                    return true;
            }
        }
        return false;
    }

    void Values::run() {
        /*
            * in this step complete:
            * Analysis of values
            */

        /*
             * after that
             * call none
             */
        COC_ERROR_INIT
        std::string buff;
        for (auto iter: this->values) {
            log->valueLog(iter->value_log, iter->default_value);
            getline(std::cin, buff);
            if (buff.empty()) {
                if (iter->default_value.empty()) {
                    error_list->add([=, this] { this->log->noValueEntered(iter->value_name); });
                } else {
                    this->values_u[iter->value_name] = iter->default_value;
                    continue;
                }
            }
            this->values_u[iter->value_name] = buff;
        }
    }
    Values::Values() : config(nullptr), log(nullptr) {}
    Values::~Values() {
        for (auto &p: values) {
            delete p;
            p = nullptr;
        }
    }

    void Arguments::run(std::string_view argv) {
        /*
            * in this step complete:
            * Analysis of argument
            */

        /*
             * after that
             * call none
             */
        COC_ERROR_INIT

        std::string_view key, value, str = argv.substr(2, argv.size() - 2);
        auto pos = str.find('=');
        if (pos != std::string::npos) {
            key = str.substr(0, pos);
            value = str.substr(pos + 1);
        }

        if (config->argument_need_extern) {
            auto p = this->arguments.find(key);
            if (p != this->arguments.end()) {
                this->arguments_u[key] = value;
            } else {
                error_list->add([=, this] { this->log->unidentifiedArgument(key); });
            }
        } else {
            this->arguments_u[key] = value;
        }
    }
    Arguments::Arguments() : config(nullptr), log(nullptr) {}
    Arguments::~Arguments() {
        for (auto &p: this->arguments) {
            delete p.second;
            p.second = nullptr;
        }
    }

    Getter::Getter(Values *values, Arguments *arguments) : is_empty(true), opt(nullptr), val(values), arg(arguments) {}
    Getter::Getter(Options *options, Values *values, Arguments *arguments) : opt(options), val(values), arg(arguments) {}

    ActionFun coc_empty = [](Getter) { return; };

    IAction::IAction(coc::Options *options, coc::Values *values, char short_cut, coc::ParserConfig *config, coc::IParserLog *log) : options(options), values(values), short_cut(short_cut) {
        this->options->config = config;
        this->options->log = log;
        this->values->config = config;
        this->values->log = log;
    }

    IHelpFunc::IHelpFunc(ParserConfig *config, Parser *parser) : config(config), parser(parser) {}

    HelpAction::HelpAction(std::string_view intro, coc::IHelpFunc *hf, char short_cut, coc::ParserConfig *config, coc::IParserLog *log) : IAction(new Options(), new Values(), short_cut, config, log) {
        this->intro = intro;
        this->hf = hf;
    }
    HelpAction::~HelpAction() {
        delete this->hf;
        hf = nullptr;
    }

    AAction::AAction(const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log) : IAction(new Options(), new Values(), short_cut, config, log) {
        this->af = af;
    }
    AAction::~AAction() {
        delete this->options;
        this->options = nullptr;
        delete this->values;
        this->values = nullptr;
    }

    Action::Action(std::string_view intro, const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log) : AAction(af, short_cut, config, log) {
        this->intro = intro;
    }

    bool Actions::havaAction(const std::string &_name) {
        if (_name.size() == 1) {
            bool result = false;
            for (auto &[k, v]: this->actions) {
                if (v->short_cut != COC_NULL_CHAR && v->short_cut == _name[0])
                    result = true;
            }
            return result;
        }
        if (this->actions.find(_name) == this->actions.end())
            return false;
        return true;
    }
    void Actions::run(const std::string &action_name, Arguments *arguments) {
        COC_ERROR_INIT
        if (action_name.size() == 1) {
            for (auto &[k, v]: this->actions) {
                //if it has shortcut and the action name equal to shortcut
                if (v->short_cut != COC_NULL_CHAR && action_name[0] == v->short_cut) {
                    return v->run(arguments);
                }
            }
        }
        auto p = this->actions.find(action_name);
        if (p == this->actions.end()) {
            return error_list->add([=, this] { this->log->notFoundAction(action_name); });
        }
        p->second->run(arguments);
    }
    void Actions::run(const std::string &action_name, std::list<std::string_view> &opt_tar, Arguments *arguments) {
        /*
            * in this step complete:
            * find the designated action
            */

        /*
             * after that
             * call action's run()
             */
        COC_ERROR_INIT
        //if it's only one letter ,then it maybe shortcut
        if (action_name.size() == 1) {
            for (auto &[k, v]: this->actions) {
                //if it has shortcut and the action name equal to shortcut
                if (v->short_cut != COC_NULL_CHAR && action_name[0] == v->short_cut)
                    return v->run(opt_tar, arguments);
            }
        }
        auto p = this->actions.find(action_name);
        if (p == this->actions.end()) {
            return error_list->add([=, this] { this->log->notFoundAction(action_name); });
        }
        return p->second->run(opt_tar, arguments);
    }
    Actions::Actions() : log(nullptr), config(nullptr), global(nullptr), actions(std::unordered_map<std::string_view, IAction *>()) {}
    Actions::~Actions() {
        for (auto &p: this->actions) {
            delete p.second;
            p.second = nullptr;
        }
        delete this->global;
        this->global = nullptr;
    }

    Parser::Parser(ParserConfig *config, IParserLog *log) : config(config), log(log) {
        this->arguments = new Arguments;
        this->actions = new Actions;
        this->arguments->config = config;
        this->arguments->log = log;
        this->actions->config = config;
        this->actions->log = log;
    }
    Parser::~Parser() {
        delete this->config;
        this->config = nullptr;
        delete this->log;
        this->log = nullptr;
        delete this->actions;
        this->actions = nullptr;
        delete this->arguments;
        this->arguments = nullptr;
    }
    void Parser::defaultConfig() {
        delete this->config;
        this->config = nullptr;
        delete this->log;
        this->log = nullptr;
    }
    void Parser::loadLog(coc::IParserLog *_log) {
        delete this->log;
        this->log = _log;
    }
    void Parser::loadConfig(coc::ParserConfig *_config) {
        delete this->config;
        this->config = _config;
    }
    int Parser::run(int argc, char **argv) {
        /*
             * in this step complete:
             * Analysis of argv except opt_tar and arguments
             * determine which is option
             * determine which is argument
             */

        /*
             * after that
             * call actions' run() or global_action's run()
             * call arguments' run()
             */
#define COC_IS_GLOBAL
#define COC_IS_COMMON
        bool is_common;
        COC_ERROR_INIT
        //optimize when empty
        if (this->actions->global != nullptr) {
            if (argc == 1) {
                COC_IS_GLOBAL
                this->actions->global->run(this->arguments);
                return error_list->invoke();
            } else {
                is_common = this->actions->havaAction(argv[1]);
            }
            if (argc == 2 && is_common) {
                COC_IS_COMMON
                this->actions->run(argv[1], this->arguments);
                return error_list->invoke();
            }
        } else {
            COC_IS_COMMON
            if (argc == 2) {
                this->actions->run(argv[1], this->arguments);
                return error_list->invoke();
            }
            is_common = true;
        }
        //analyse
        std::list<std::string_view> all_argv;//all cmd argv
        std::list<std::string_view> opt_tar;

        int i = 1;
        //if it isn't the global action, then read from index of 2
        if (is_common) i = 2;
        for (; i < argc; ++i) {
            all_argv.emplace_back(argv[i]);
        }
        for (auto &p: all_argv) {
            if (p[0] == '-') {
                if (p[1] == this->config->argument_mark) {
                    arguments->run(p);
                } else {
                    opt_tar.push_back(p);
                }
            } else {
                opt_tar.push_back(p);
            }
        }
        //execute
        if (!is_common) {
            if (this->actions->global == nullptr) {
                error_list->add([=, this] { this->log->globalActionNotDoesNotExist(); });
                return error_list->invoke();
            } else {
                this->actions->global->run(opt_tar, this->arguments);
                return error_list->invoke();
            }
        }
        this->actions->run(argv[1], opt_tar, this->arguments);
        return error_list->invoke();
    }

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
            for (auto p: _values->get_values()) {
                std::cout << '\t' << p->value_name << "\tType:" << p->value_type << '\t' << p->intro;
                if (!p->default_value.empty()) {
                    std::cout << "\tDefault=" << p->default_value;
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

    void PrefabHelpFunc::run(coc::Getter g) {

        PrintComponent::printBasicMessage(config);
        PrintComponent::printActions(parser);
        PrintComponent::printAction(parser->get_actions()->get_global_action());
        PrintComponent::printArguments(parser);
    }
    PrefabHelpFunc::PrefabHelpFunc(ParserConfig *config, Parser *parser) : IHelpFunc(config, parser){};
}// namespace coc