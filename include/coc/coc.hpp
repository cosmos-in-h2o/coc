#include <functional>
#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

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
        void add(const error &_error);
        int invoke();
        ErrorList() = default;
        ~ErrorList() = default;
    };

    struct ParserConfig {
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
        void unidentifiedArgument(std::string_view argument) override;
        void unidentifiedOption(std::string_view option) override;
        void unidentifiedOption(char option) override;
        void noValueEntered(std::string_view value) override;
        void notFoundAction(std::string_view action) override;
        void valueLog(std::string_view value_log, std::string_view default_value) override;
        void globalActionNotDoesNotExist() override;
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
        void run(Option *opt);
        void run(std::string_view target);

    public:
        Targets();
        ~Targets();

        std::string_view at(int index, const std::string &_default);
        std::string_view atAbsoluteIndex(int index, const std::string &_default);
        std::string_view at(const std::string &option_name, int index, const std::string &_default);
        std::string_view atOutOfRange(const std::string &option_name, int index, const std::string &_default);

        size_t size();
        size_t size_f();
        size_t size(const std::string &option_name);
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
        void addOption(std::string_view name, std::string_view intro, int number, char short_name);
        //add opt_tar which user input.
        void run(std::list<std::string_view> &opt_tar);

    public:
        Options();
        ~Options();
        std::vector<Option *> &get_options();
        int at(int index);
        int operator[](int index);
        bool getOption(const std::string &option);
        bool getOption(char short_name);
        bool isOnlyOpt(const std::string &option);
        bool isOnlyOpt(char short_name);
        std::vector<Option *> get_list();
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
        void addValue(std::string_view name, std::string_view val_log, std::string_view type, std::string_view intro, std::string_view def_val);
        //put run and get value that user input
        void run();

    public:
        Values();
        ~Values();
        std::vector<Value *> &get_values();

        template<typename T>
        inline T get(const std::string &name) {
            return T(this->values_u[name]);
        }
        //the first is value.if the second is false,it means that the value was not found
        template<>
        int get<int>(const std::string &name);
        template<>
        float get<float>(const std::string &name);
        template<>
        double get<double>(const std::string &name);
        template<>
        char get<char>(const std::string &name);
        template<>
        inline bool get<bool>(const std::string &name);
        template<>
        std::string get<std::string>(const std::string &name);
        template<>
        const char *get<const char *>(const std::string &name);
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
        void addArgument(std::string_view _name, std::string_view _type, std::string_view _intro);
        void run(std::string_view argv);

    public:
        Arguments();
        ~Arguments();

        std::unordered_map<std::string_view, Argument *> &get_arguments_map();
        template<typename T>
        inline T get(const std::string &name, T default_) {
            auto p = this->arguments_u.find(name);
            if (p == this->arguments_u.end())
                return default_;
            return T(p->second);
        }
        //the first is value.if the second is false,it means that the value was not found
        template<>
        int get<int>(const std::string &name, int default_);
        template<>
        float get<float>(const std::string &name, float default_);
        template<>
        double get<double>(const std::string &name, double default_);
        template<>
        char get<char>(const std::string &name, char default_);
        template<>
        bool get<bool>(const std::string &name, bool default_);
        template<>
        std::string get<std::string>(const std::string &name, std::string default_);
        template<>
        const char *get<const char *>(const std::string &name, const char *default_);
    };

    struct Getter {
    private:
        Options *opt;
        Values *val;
        Arguments *arg;

    public:
        explicit Getter(Values *values);
        Getter(Options *options, Values *values, Arguments *arguments);
        bool is_empty = false;
        Options *get_opt();
        Values *get_val();
        Arguments *get_arg();
        Targets *get_tar();
    };

    //Action function pointer
    using ActionFun = std::function<void(Getter)>;

    extern ActionFun coc_empty;

    //Action interface
    struct IAction {
        friend class Actions;

    protected:
        virtual void run() = 0;
        virtual void run(std::list<std::string_view> &opt_tar, Arguments *arguments) = 0;

    public:
        char short_cut{};
        Options *options;
        Values *values;

        virtual std::string_view get_intro() = 0;

        IAction *addOption(std::string_view _name, std::string_view _intro, int _num, char _s_name = COC_NULL_CHAR);
        IAction *addValue(std::string_view _name, std::string_view _log, std::string_view _type, std::string_view _intro, std::string_view _def_val = "");

        IAction(Options *options, Values *values, char short_cut, ParserConfig *config, IParserLog *log);
        IAction();
        virtual ~IAction() = default;
    };

    struct IHelpFunc {
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
        void run() override;
        void run(std::list<std::string_view> &opt_tar, coc::Arguments *arguments) override;

    public:
        HelpAction(std::string_view intro, IHelpFunc *hf, char short_cut, ParserConfig *config, IParserLog *log);
        ~HelpAction() override;
        std::string_view get_intro() override;
    };

    class AAction : public IAction {
        friend class Actions;
        friend class Parser;

    protected:
        ActionFun af;

    private:
        void run() override;
        void run(std::list<std::string_view> &opt_tar, Arguments *arguments) override;

    public:
        std::string_view get_intro() override;

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
        std::string_view get_intro() override;
        Action(std::string_view intro, const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log);
    };

    class Actions {
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

        //add an action
        IAction *addAction(std::string_view _name, std::string_view _intro, const ActionFun &_af, char _short_cut = COC_NULL_CHAR);
        //add a help action
        IAction *addHelpAction(std::string_view _name, std::string_view _intro, IHelpFunc *_hf, char _short_cut = COC_NULL_CHAR);

        std::unordered_map<std::string_view, IAction *> &get_actions_map();
        GlobalAction *get_global_action();
    };

    class Parser {
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
        //only package with a layer
        IAction *addAction(std::string_view _name, std::string_view _intro, const ActionFun &_af = coc_empty, char _short_cut = COC_NULL_CHAR);
        //only package with a layer
        IAction *addHelpAction(std::string_view _name, std::string_view _intro, IHelpFunc *_hf, char _short_cut = COC_NULL_CHAR);
        //only package with a layer
        Parser *addArgument(std::string_view _name, std::string_view _type, std::string_view _intro);

        //get and set function
        ParserConfig *get_config();
        IParserLog *get_log();
        IAction *set_global_action(const ActionFun &_af);
        Arguments *get_argument();
        Actions *get_actions();

        int run(int argc, char **argv);
    };

    struct PrintComponent {
        static void printBasicMessage(ParserConfig *_config);
        static void printOptions(Options *_options);
        static void printValues(Values *_values);
        static void printAction(IAction *_action);
        static void printActions(Parser *_parser);
        static void printArguments(Parser *_parser);
    };

    struct PrefabHelpFunc : public IHelpFunc {
        void run(Getter g) override;
        PrefabHelpFunc(ParserConfig *config, Parser *parser);
    };
}//namespace coc