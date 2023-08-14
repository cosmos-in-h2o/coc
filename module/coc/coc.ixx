module;
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
export module coc;

#ifdef COC_NULL_CHAR
#undef COC_NULL_CHAR
#endif
#define COC_NULL_CHAR '\0'

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

    export struct ParserConfig {
        bool intellisense_mode = true;//not supported now
        bool argument_need_extern = true;
        std::string logo_and_version =
                "coc v1.1.0\n"
                "\t  ____ ___   ____ \n"
                "\t / ___/ _ \\ / ___|\n"
                "\t| |  | | | | |    \n"
                "\t| |__| |_| | |___ \n"
                "\t \\____\\___/ \\____|\n"
                "\t\t\tcoc by cosmos-in-h2o\n"
                "==> https://github.com/cosmos-in-h2o/coc";//your app's version
        std::string introduce;
        std::string usage;
        char argument_mark = 'D';//-[argument_mark] mark as argument
    };

    export struct IParserLog {
        virtual ~IParserLog() = default;
        virtual void unidentifiedArgument(std::string_view argument) = 0;
        virtual void unidentifiedOption(std::string_view option) = 0;
        virtual void unidentifiedOption(char option) = 0;
        virtual void noValueEntered(std::string_view value) = 0;
        virtual void notFoundAction(std::string_view action) = 0;
        virtual void valueLog(std::string_view value_log, std::string_view default_value) = 0;
        virtual void globalActionNotDoesNotExist() = 0;
    };

    export struct PrefabParserLog : public IParserLog {
        ~PrefabParserLog() override = default;
        inline void unidentifiedArgument(std::string_view argument) override {
            std::cout << "Error:Unidentified argument:" << argument << ".\n";
        }
        inline void unidentifiedOption(std::string_view option) override {
            std::cout << "Error:Unidentified option:--" << option << ".\n";
        }
        inline void unidentifiedOption(char option) override {
            std::cout << "Error:Unidentified option:-" << option << ".\n";
        }
        inline void noValueEntered(std::string_view value) override {
            std::cout << "Error:Value: " << value << " not assigned.\n";
        }
        inline void notFoundAction(std::string_view action) override {
            std::cout << "Error:Not found action:" << action << ".\n";
        }
        inline void valueLog(std::string_view value_log, std::string_view default_value) override {
            if (!default_value.empty())
                std::cout << value_log << "(default=" << default_value << "):\n";
            else
                std::cout << value_log << ":\n";
        }
        inline void globalActionNotDoesNotExist() override {
            std::cout << "Error:Global action doesn't exist.\n";
        }
    };

    struct Option {
        std::string_view name, intro;
        int number;
        char short_name;
    };

    class Targets {
        friend class Options;

    private:
        struct Target {
            Option *option;
            std::vector<std::string_view> target_list;
            explicit Target(Option *opt)
                : option(opt) {}
            Target() : option(nullptr) {}
        };
        Target *first;
        std::vector<Target *> targets_list;
        inline void run(Option *opt) {
            this->targets_list.push_back(new Target(opt));
        }
        void run(std::string_view target);

    public:
        Targets();
        ~Targets();

        std::string_view at(int index, const std::string &_default);
        std::string_view atAbsoluteIndex(int index, const std::string &_default);
        std::string_view at(const std::string &option_name, int index, const std::string &_default);
        std::string_view atOutOfRange(const std::string &option_name, int index, const std::string &_default);

        inline size_t size() {
            size_t result = this->first->target_list.size();
            for (auto iter: this->targets_list) {
                result += iter->target_list.size();
            }
            return result;
        }
        inline size_t size_f() {
            return this->first->target_list.size();
        }
        inline size_t size(const std::string &option_name) {
            for (auto iter: this->targets_list) {
                if (iter->option->name == option_name) {
                    return iter->target_list.size();
                }
            }
            return 0;
        }
    };

    class Options {
        friend struct Targets;
        friend class IAction;
        friend class AAction;
        friend class HelpAction;
        friend class Parser;
        friend struct PrefabHelpFunc;
        friend struct Getter;

    private:
        ParserConfig *config;
        IParserLog *log;
        std::vector<Option *> options;  //options list
        std::vector<Option *> options_u;//user options list
        Targets *targets;
        //add an option to options list
        inline void addOption(std::string_view name, std::string_view intro, int number, char short_name) {
            auto p = new Option{name, intro, number, short_name};
            this->options.push_back(p);
        }
        //add opt_tar which user input.
        void run(std::list<std::string_view> &opt_tar);

    public:
        Options();
        ~Options();
        inline std::vector<Option *> &get_options() {
            return this->options;
        }
        inline int at(int index) {
            if (index >= this->options_u.size()) {
                return -1;
            }
            return this->options_u[index]->number;
        }
        inline int operator[](int index) {
            return this->at(index);
        }
        inline bool getOption(const std::string &option) {
            return std::ranges::any_of(options_u, [&](auto iter) {
                return iter->name == option;
            });
        }
        inline bool getOption(char short_name) {
            return std::ranges::any_of(options_u, [&](auto iter) {
                return iter->short_name == short_name;
            });
        }
        bool isOnlyOpt(const std::string &option);
        bool isOnlyOpt(char short_name);
        inline std::vector<Option *> get_list() {
            return this->options_u;
        }
    };

    class Values {
        friend class IAction;
        friend class AAction;
        friend class HelpAction;

    private:
        struct Value {
            std::string_view value_name, value_log, value_type, intro, default_value;
        };
        std::vector<Value *> values;
        std::unordered_map<std::string_view, std::string> values_u;
        ParserConfig *config;
        IParserLog *log;

        //add a value to list
        inline void addValue(std::string_view name, std::string_view val_log, std::string_view type, std::string_view intro, std::string_view def_val) {
            this->values.push_back(new Values::Value{name, val_log, type, intro, def_val});
        }
        //put run and get value that user input
        void run();

    public:
        Values();
        ~Values();
        inline std::vector<Value *> &get_values() {
            return this->values;
        }

        template<typename T>
        inline T get(const std::string &name) {
            return T(this->values_u[name]);
        }
        //the first is value.if the second is false,it means that the value was not found
        template<>
        inline int get<int>(const std::string &name) {
            return stoi(this->values_u[name]);
        }
        template<>
        inline float get<float>(const std::string &name) {
            return stof(this->values_u[name]);
        }
        template<>
        inline double get<double>(const std::string &name) {
            return stod(this->values_u[name]);
        }
        template<>
        inline char get<char>(const std::string &name) {
            return this->values_u[name][0];
        }
        template<>
        inline bool get<bool>(const std::string &name) {
            std::string &buff = this->values_u[name];
            if (buff == "FALSE" || buff == "False" || buff == "false" || buff == "0")
                return false;
            else
                return true;
        }
        template<>
        inline std::string get<std::string>(const std::string &name) {
            return this->values_u[name];
        }
        template<>
        inline const char *get<const char *>(const std::string &name) {
            return this->values_u[name].c_str();
        }
    };

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

    export struct Getter {
    private:
        Options *opt;
        Values *val;
        Arguments *arg;

    public:
        explicit Getter(Values *values);
        Getter(Options *options, Values *values, Arguments *arguments);
        bool is_empty = false;
        inline Options *get_opt() { return this->opt; }
        inline Values *get_val() { return this->val; }
        inline Arguments *get_arg() { return this->arg; }
        inline Targets *get_tar() { return this->opt->targets; }
    };

    //Action function pointer
    export using ActionFun = std::function<void(Getter)>;

    export extern ActionFun coc_empty;

    //Action interface
    export struct IAction {
        friend class Actions;

    protected:
        virtual void run() = 0;
        virtual void run(std::list<std::string_view> &opt_tar, Arguments *arguments) = 0;

    public:
        char short_cut{};
        Options *options;
        Values *values;

        virtual std::string_view get_intro() = 0;

        inline IAction *addOption(std::string_view _name, std::string_view _intro, int _num, char _s_name = COC_NULL_CHAR) {
            this->options->addOption(_name, _intro, _num, _s_name);
            return this;
        }
        inline IAction *addValue(std::string_view _name, std::string_view _log, std::string_view _type, std::string_view _intro, std::string_view _def_val = "") {
            this->values->addValue(_name, _log, _type, _intro, _def_val);
            return this;
        }

        IAction(Options *options, Values *values, char short_cut, ParserConfig *config, IParserLog *log);
        IAction();
        virtual ~IAction() = default;
    };

    export struct IHelpFunc {
        ParserConfig *config;
        Parser *parser;
        virtual void run(Getter g) = 0;
        virtual ~IHelpFunc() = default;
        IHelpFunc(ParserConfig *config, Parser *parser);
    };

    class HelpAction : public IAction {
    private:
        std::string_view intro;
        IHelpFunc *hf;
        inline void run() override {
            this->values->run();
            this->hf->run(Getter(this->values));
        }
        inline void run(std::list<std::string_view> &opt_tar, coc::Arguments *arguments) override {
            this->options->run(opt_tar);
            this->values->run();
            this->hf->run(Getter(this->options, this->values, arguments));
        }

    public:
        HelpAction(std::string_view intro, IHelpFunc *hf, char short_cut, ParserConfig *config, IParserLog *log);
        ~HelpAction() override;
        inline std::string_view get_intro() override {
            return this->intro;
        }
    };

    class AAction : public IAction {
        friend class Actions;
        friend class Parser;

    protected:
        ActionFun af;

    private:
        inline void run() override {
            this->values->run();
            this->af(Getter(this->values));
        }
        inline void run(std::list<std::string_view> &opt_tar, Arguments *arguments) override {
            /*
            * in this step complete:
            * do nothing,only call function
            */

            /*
             * after that
             * call options' run()
             * call values' run()
             */

            this->options->run(opt_tar);
            this->values->run();
            this->af(Getter(this->options, this->values, arguments));
        }

    public:
        inline std::string_view get_intro() override {
            return std::string_view{"null"};
        }

        AAction(const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log);
        ~AAction() override;
    };

    using GlobalAction = AAction;

    class Action : public AAction {
        friend class Actions;
        friend class Parser;

    private:
        std::string_view intro;

    public:
        inline std::string_view get_intro() override {
            return this->intro;
        }

        Action(std::string_view intro, const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log);
    };

    export class Actions {
        friend class Parser;

    private:
        IParserLog *log;
        ParserConfig *config;
        GlobalAction *global;
        std::unordered_map<std::string_view, IAction *> actions;

        bool havaAction(const std::string &_name);
        void run(const std::string &action_name);
        void run(const std::string &action_name, std::list<std::string_view> &opt_tar, Arguments *arguments);

    public:
        Actions();
        ~Actions();

        inline IAction *addAction(std::string_view _name, std::string_view _intro, const ActionFun &_af, char _short_cut = COC_NULL_CHAR) {
            auto action = new Action(_intro, _af, _short_cut, this->config, this->log);
            this->actions[_name] = action;
            return action;
        }
        inline IAction *addHelpAction(std::string_view _name, std::string_view _intro, IHelpFunc *_hf, char _short_cut = COC_NULL_CHAR) {
            auto *action = new HelpAction(_intro, _hf, _short_cut, this->config, this->log);
            this->actions[_name] = action;
            return action;
        }
        inline std::unordered_map<std::string_view, IAction *> &get_actions_map() {
            return this->actions;
        }
        inline GlobalAction *get_global_action() {
            return this->global;
        }
    };

    export class Parser {
    private:
        ParserConfig *config;
        IParserLog *log;
        bool is_def_hp = true;//if open default function
        Actions *actions;
        Arguments *arguments;

    public:
        Parser(ParserConfig *config, IParserLog *log);
        ~Parser();

        void defaultConfig();
        //over log
        void loadLog(IParserLog *_log);
        //over config
        void loadConfig(ParserConfig *_config);

        inline IAction *addAction(std::string_view _name, std::string_view _intro, const ActionFun &_af, char _short_cut = COC_NULL_CHAR) {
            return this->actions->addAction(_name, _intro, _af, _short_cut);
        }
        inline IAction *addHelpAction(std::string_view _name, std::string_view _intro, IHelpFunc *_hf, char _short_cut = COC_NULL_CHAR) {
            return this->actions->addHelpAction(_name, _intro, _hf, _short_cut);
        }
        inline Parser *addArgument(std::string_view _name, std::string_view _type, std::string_view _intro) {
            this->arguments->addArgument(_name, _type, _intro);
            return this;
        }
        //get and set function
        inline ParserConfig *get_config() {
            return this->config;
        }
        inline IParserLog *get_log() {
            return this->log;
        }
        inline IAction *set_global_action(const ActionFun &_af) {
            if (this->actions->global == nullptr)
                this->actions->global = new GlobalAction(_af, COC_NULL_CHAR, this->config, this->log);
            return this->actions->global;
        }
        inline Arguments *get_argument() {
            return this->arguments;
        }
        inline Actions *get_actions() {
            return this->actions;
        }

        int run(int argc, char **argv);
    };

    export struct PrintComponent {
        static void printBasicMessage(ParserConfig *_config);
        static void printOptions(Options *_options);
        static void printValues(Values *_values);
        static void printAction(IAction *_action);
        static void printActions(Parser *_parser);
        static void printArguments(Parser *_parser);
    };

    export struct PrefabHelpFunc : public IHelpFunc {
        void run(Getter g) override;
        PrefabHelpFunc(ParserConfig *config, Parser *parser);
    };
}//namespace coc