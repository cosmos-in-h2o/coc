#ifndef COC_HPP
#define COC_HPP
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <list>
#include <string_view>
#include <vector>


#undef COC_NULL_CHAR
#define COC_NULL_CHAR '\0'
namespace coc {
    class Parser;
    class Values;

    struct ParserConfig{
        bool intellisense_mode=true;//not supported now
        bool is_exit_if_not_found_option=true;
        bool argument_need_extern=true;
        std::string logo_and_version=
                "coc v1.0.0\n"
                "\t  ____ ___   ____ \n"
                "\t / ___/ _ \\ / ___|\n"
                "\t| |  | | | | |    \n"
                "\t| |__| |_| | |___ \n"
                "\t \\____\\___/ \\____|\n"
                "\t\t\tcoc by dream727\n"
                "==> https://github.com/dream727/coc";//your app's version
        std::string introduce;
        std::string usage;
        char argument_mark='D';//-[argument_mark] mark as argument
    };

    struct Log{
        virtual inline void unidentifiedArgument(const std::string& argument){
            printf("Error:Unidentified argument:%s.\n",argument.c_str());
        }
        virtual inline void unidentifiedOption(const std::string& option){
            printf("Error:Unidentified option:--%s.\n",option.c_str());
        }
        virtual inline void unidentifiedOption(char option){
            printf("Error:Unidentified option:-%c.\n",option);
        }
        virtual inline void noValueEntered(const std::string& value){
            printf("Error:Value:%s not assigned.\n",value.c_str());
        }
        virtual inline void notFoundAction(const std::string& action){
            printf("Error:Not found action:%s.\n",action.c_str());
        }
        virtual inline void valueLog(const std::string &value_log,const std::string& default_value){
            std::string temp;
            if(!default_value.empty())
                temp="(default="+default_value+")";
            printf("%s%s:\n",value_log.c_str(),temp.c_str());
        }
        virtual inline void globalActionNotDoesNotExist(){
            std::cout<<"Error:Global action doesn't exist.\n";
        }

    };

    class Options{
        friend struct Targets;
        friend class IAction;
        friend class AAction;
        friend class HelpAction;
        friend class Parser;
        friend struct HelpFunc;
    private:
        struct Option{
            std::string name, intro;
            int number;
            char short_name;
            Option(std::string&& name,std::string&&intro,int number,char short_name):
                                                                                 name(std::move(name)), intro(std::move(intro)),number(number), short_name(short_name)
            {}
        };
        ParserConfig *config;
        Log *log;
        std::vector<Option*> options;//options list
        std::vector<Option*> options_u;//user options list
        //add an option to options list
        inline void addOption(std::string&& name,std::string&&intro,int number,char short_name){
            auto p=new Option(std::move(name), std::move(intro),number, short_name);
            this->options.push_back(p);
        }
        //add opt_tar which user input.
        bool run(std::list<std::string_view>& opt_tar){
            /*
            * in this step complete:
            * Analysis of opt_tar
            */

            /*
             * after that
             * call none
             */
            for(auto str: opt_tar) {
                if (str[1] == '-') {
                    //if the 2nd char is -
                    for (auto iter_opt: this->options) {
                        //find right option
                        if (iter_opt->name == str.substr(2, str.size() - 2)) {
                            this->options_u.push_back(iter_opt);//add option ptr to options_u
                            return true;
                        }
                    }
                    log->unidentifiedOption(str.data());
                    return false;
                } else {
                    bool error = true;
                    std::string_view options_str=str.substr(1,str.size()-1);
                    for (auto iter_str: options_str) {
                        for (auto iter_opt: this->options) {
                            if (iter_opt->short_name != COC_NULL_CHAR && iter_str == iter_opt->short_name) {
                                this->options_u.push_back(iter_opt);//add option ptr to options_u
                                error = false;
                            }
                        }
                        if (error) {
                            log->unidentifiedOption(iter_str);
                            if(config->is_exit_if_not_found_option)
                            {
                                return false;
                            }
                        }
                    }
                    if(!config->is_exit_if_not_found_option){
                        return true;
                    }
                    return !error;
                }
            }
            return true;
        }
    public:
        Options():
                    config(nullptr),log(nullptr)
        {}

        ~Options(){
            for(auto& p:this->options){
                delete p;
                p=nullptr;
            }
        }
        inline int at(int index){
            if(index>=this->options_u.size()){
                return -1;
            }
            return this->options_u[index]->number;
        }
        inline int operator[](int index){
            return this->at(index);
        }
        inline bool getOption(const std::string& option){
            for (auto iter: this->options_u) {
                if (iter->name == option)
                    return true;
            }
            return false;
        }
        inline bool getOption(char short_name){
            for (auto iter: this->options_u) {
                if (iter->short_name == short_name)
                    return true;
            }
            return false;
        }
        inline bool isOnlyOpt(const std::string& option){
            if(this->options_u.size()==1) {
                for (auto iter: this->options_u) {
                    if (iter->name == option)
                        return true;
                }
            }
            return false;
        }
        inline bool isOnlyOpt(char short_name){
            if(this->options_u.size()==1) {
                for (auto iter: this->options_u) {
                    if (iter->short_name == short_name)
                        return true;
                }
            }
            return false;
        }
        inline std::vector<Option*> get_list(){
            return this->options_u;
        }
    };

    class Values{
        friend class IAction;
        friend class AAction;
        friend class HelpAction;
        friend class Parser;
        friend struct HelpFunc;

    private:
        struct Value{
            std::string value_name,value_log,value_type, intro,default_value;
            Value(std::string&&name,std::string&& val_log,std::string&& type,std::string&&intro,std::string&& def_val):
                                                                                                      value_name(std::move(name)),value_log(std::move(val_log)),value_type(std::move(type)), intro(std::move(intro)),default_value(std::move(def_val))
            {}
        };
        std::vector<Value*> values;
        std::map<std::string,std::string> values_u;
        ParserConfig* config;
        Log* log;

        //add a value to list
        inline void addValue(std::string&& name,std::string&& val_log,std::string&& type,std::string&& intro,std::string&& def_val){
            this->values.push_back(new Values::Value(std::move(name),std::move(val_log),std::move(type),std::move(intro),std::move(def_val)));
        }
        //put run and get value that user input
        bool run(){
            /*
            * in this step complete:
            * Analysis of values
            */

            /*
             * after that
             * call none
             */
            std::string buff;
            for(auto iter:this->values){
                log->valueLog(iter->value_log,iter->default_value);
                getline(std::cin,buff);
                if(buff.empty()){
                    if(iter->default_value.empty()){
                        log->noValueEntered(iter->value_name);
                        return false;
                    }
                    else{
                        this->values_u[iter->value_name]=iter->default_value;
                        continue;
                    }
                }
                this->values_u[iter->value_name]=buff;
            }
            return true;
        }
    public:
        Values():
                   config(nullptr),log(nullptr)
        {}
        ~Values(){
            for(auto&p:values){
                delete p;
                p= nullptr;
            }
        }
        //the first is value.if the second is false,it means that the value was not found
        inline int getInt(const std::string& name){
            return stoi(this->values_u[name]);
        }
        inline float getFloat(const std::string& name){
            return stof(this->values_u[name]);
        }
        inline char getChar(const std::string& name){
            return this->values_u[name][0];
        }
        inline bool getBool(const std::string& name){
            std::string& buff=this->values_u[name];
            if(buff=="FALSE"||buff=="False"||buff=="false"||buff=="0")
                return false;
            else
                return true;
        }
        inline std::string getString(const std::string& name){
            return this->values_u[name];
        }
    };

    class Targets{
        friend class Action;
        friend class Parser;
    private:
        struct Target{
            std::vector<std::string> option_name;
            std::vector<std::string>target_list;
        };
        std::vector<Target*>targets_list;
        int run(std::vector<std::vector<std::string>&>& targets,std::string){

        }
    public:
        const std::string& at(int index,const std::string &_default){
            return _default;
        }
        const std::string& at(const std::string&option_name,int index,const std::string&_default){
            return _default;
        }
        const std::string& atOutRange(const std::string&option_name,int index,const std::string&_default){
            return _default;
        }
        int size(const std::string& option_name){

        }
    };

    class Arguments{
        friend class Parser;
        friend struct HelpFunc;
    private:
        struct Argument{
            std::string argument_type,describe;

            Argument(std::string& argument_type,std::string& describe):
                                                                argument_type(argument_type),describe(describe)
            {}
        };

        ParserConfig* config;
        Log* log;
        std::map<std::string,Argument*> arguments;
        std::map<std::string,std::string> arguments_u;

        //add an argument to list
        inline void addArgument(std::string& argument_name,std::string& argument_type,std::string& describe){
            this->arguments[argument_name]=new Argument(argument_type,describe);
        }
        bool run(std::string_view argv){
            /*
            * in this step complete:
            * Analysis of argument
            */

            /*
             * after that
             * call none
             */
            std::string key;
            std::string value;
            std::string buff;

            buff = argv.substr(2, argv.size() - 2);
            for (char & i : buff) {
                if(i=='='){
                    i=' ';
                }
            }
            std::istringstream ss(buff);
            ss>>key>>value;

            if(config->argument_need_extern) {
                auto p = this->arguments.find(key);
                if (p != this->arguments.end()) {
                    this->arguments_u[key] = value;
                } else {
                    log->unidentifiedArgument(key);
                    return false;
                }
            }
            else{
                this->arguments_u[key]=value;
            }
            return true;
        }
    public:
        Arguments():
                      config(nullptr),log(nullptr)
        {}

        ~Arguments(){
            for(auto& p:this->arguments){
                delete p.second;
                p.second= nullptr;
            }
        }
        //the first is value.if the second is false,it means that the value was not found
        inline int getInt(const std::string& name,int default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return stoi(p->second);
        }
        inline float getFloat(const std::string& name,float default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return stof(p->second);
        }
        inline char getChar(const std::string& name,char default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return p->second[0];
        }
        inline bool getBool(const std::string& name,bool default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            if(p->second=="FALSE"||
                p->second=="False"||
                p->second=="false"||
                p->second=="0")
                return false;
            else
                return true;
        }
        inline std::string getString(const std::string& name,const std::string& default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return p->second;
        }
    };

    struct Getter{
    private:
        Options* opt;
        Values* val;
        Arguments* arg;

    public:
        explicit Getter(Values* values):
                                          is_empty(true),opt(nullptr),val(values),arg(nullptr)
        {}
        Getter(Options* options,Values* values,Arguments* arguments):
                                                                         opt(options),val(values),arg(arguments)
        {}
        bool is_empty=false;
        inline Options* get_opt(){return this->opt;}
        inline Values* get_val(){return this->val;}
        inline Arguments* get_arg(){return this->arg;}
    };

    //Action function pointer
    using ActionFun =std::function<void(Getter)>;

    ActionFun coc_empty=[](Getter){return;};

    //Action interface
    struct IAction{
        friend class Actions;
        friend struct HelpFunc;
    protected:
        virtual int run()=0;
        virtual int run(std::list<std::string_view>&opt_tar, Arguments *arguments)=0;
        char short_cut{};
        Options* options;
        Values* values;
    public:
        virtual const std::string& get_describe()=0;
        virtual IAction* addOption(std::string&&name,std::string &&intro,int num,char s_name =COC_NULL_CHAR) {
            this->options->addOption(std::move(name), std::move(intro),num, s_name);
            return this;
        }
        virtual IAction* addValue(std::string&&name,std::string&&val_log,std::string&&type,std::string&&intro,std::string&&def_val ="") {
            this->values->addValue(std::move(name),std::move(val_log), std::move(type),std::move(intro),std::move(def_val));
            return this;
        }
        IAction(Options*options,Values*values,char short_cut,ParserConfig*config,Log*log):
                                                                                                    options(options),values(values),short_cut(short_cut)
        {
            this->options->config=config;
            this->options->log=log;
            this->values->config=config;
            this->values->log=log;
        }
        virtual ~IAction()= default;
    };

    struct IHelpFunc{
        ParserConfig* config;
        Parser* parser;
        virtual void run(Getter g)=0;
        virtual ~IHelpFunc()= default;
        IHelpFunc(ParserConfig* config,Parser* parser):
                                                          config(config),parser(parser)
        {}
    };

    class HelpAction:public IAction{
    private:
        std::string describe;
        IHelpFunc* hf;
        inline int run() override{
            if (!this->values->run()) return -1;
            this->hf->run(Getter(this->values));
            return 0;
        }
        int run(std::list<std::string_view> &opt_tar, coc::Arguments *arguments) override{
            if(!this->options->run(opt_tar)) return -1;
            if(!this->values->run()) return -1;
            this->hf->run(Getter(this->options,this->values,arguments));
            return 0;
        }

    public:
        ~HelpAction() override{
            delete this->hf;
            hf=nullptr;
        }
        HelpAction(std::string& describe,IHelpFunc*hf,char short_cut,ParserConfig*config,Log*log):
                                                                                                      IAction(new Options(),new Values(),short_cut,config,log)
        {
            this->describe=describe;
            this->hf=hf;
        }

        inline const std::string& get_describe() override{
            return this->describe;
        }
    };

    class AAction: public IAction {
        friend class Actions;
        friend class Parser;
        friend struct HelpFunc;
    protected:
        ActionFun af;

    private:
        inline int run() override {
            if (!this->values->run()) return -1;
            this->af(Getter(this->values));
            return 0;
        }
        inline int run(std::list<std::string_view>&opt_tar, Arguments *arguments) override{
            /*
            * in this step complete:
            * do nothing,only call function
            */

            /*
             * after that
             * call options' run()
             * call values' run()
             */

            if(!this->options->run(opt_tar)) return -1;
            if(!this->values->run()) return -1;
            this->af(Getter(this->options,this->values,arguments));
            return 0;
        }
    public:
        inline const std::string & get_describe() override{return "null";}

        ~AAction() override{
            delete this->options;
            this->options=nullptr;
            delete this->values;
            this->values=nullptr;
        }
        AAction(ActionFun af,char short_cut,ParserConfig*config,Log*log):
                                                                                IAction(new Options(),new Values(),short_cut,config,log)
        {
            this->af=af;
        }
    };

    using GlobalAction=AAction;

    class Action:public AAction{
        friend class Actions;
        friend class Parser;
        friend struct HelpFunc;
    private:
        std::string describe;
    public:
        inline const std::string & get_describe() override{
            return this->describe;
        }
        Action(std::string& describe, ActionFun af,char short_cut,ParserConfig*config,Log*log):
                                                                                                 AAction(af,short_cut,config,log)
        {
            this->describe=describe;
        }
    };

    class Actions{
        friend class Parser;
        friend struct HelpFunc;
    private:
        Log* log;
        ParserConfig *config;
        GlobalAction* global;
        std::map<std::string, IAction*> actions;
        bool isHavaAction(const std::string& name){
            if (this->actions.find(name) == this->actions.end())
                return false;
            return true;
        }
        int run(const std::string& action_name){
            if(action_name.size()==1){
                for(auto &[k,v]: this->actions){
                    //if it has shortcut and the action name equal to shortcut
                    if(v->short_cut!= COC_NULL_CHAR &&action_name[0]==v->short_cut){
                        v->run();
                        return 0;
                    }
                }
            }
            auto p = this->actions.find(action_name);
            if (p == this->actions.end()) {

                log->notFoundAction(action_name);
                return -1;
            }
            p->second->run();
            return 0;
        }
        //if error,the function will return -1
        int run(const std::string& action_name,std::list<std::string_view>& opt_tar,Arguments *arguments) {
            /*
            * in this step complete:
            * find the designated action
            */

            /*
             * after that
             * call action's run()
             */

            //if it's only one letter ,then it maybe shortcut
            if(action_name.size()==1){
                for(auto &[k,v]: this->actions){
                    //if it has shortcut and the action name equal to shortcut
                    if(v->short_cut!= COC_NULL_CHAR &&action_name[0]==v->short_cut){
                        return v->run(opt_tar, arguments);
                    }
                }
            }
            auto p = this->actions.find(action_name);
            if (p == this->actions.end()) {

                log->notFoundAction(action_name);
                return -1;
            }
            return p->second->run(opt_tar, arguments);
        }

    public:
        Actions():
                    log(nullptr),config(nullptr),global(nullptr),actions(std::map<std::string, IAction*>())
        {}
        ~Actions(){
            for(auto&p:this->actions){
                delete p.second;
                p.second= nullptr;
            }
            delete this->global;
            this->global=nullptr;
        }

        //add an action
        inline IAction* addAction(std::string& action_name,std::string& describe, ActionFun & af,char short_cut= COC_NULL_CHAR){
            auto action =new Action(describe,af,short_cut,this->config,this->log);
            this->actions[action_name]=action;
            return action;
        }

        //add a help action
        inline IAction* addHelpAction(std::string& action_name,std::string& describe,IHelpFunc* hf,char short_cut= COC_NULL_CHAR){
            auto* action =new HelpAction(describe,hf,short_cut,this->config,this->log);
            this->actions[action_name]=action;
            return action;
        }
    };

    class Parser{
        friend struct HelpFunc;
    public:
        typedef void(*help_fun)(Parser*);
    private:
        ParserConfig *config;
        Log *log;
        bool is_def_hp = true;//if open default function
        help_fun hf;
        Actions *actions;
        Arguments *arguments;
        void help(){
            if(!this->is_def_hp){
                this->hf(this);
                return;
            }
        }
    public:
        Parser(ParserConfig *config,Log* log):
                                                 config(config),log(log), hf(nullptr)
        {
            this->arguments=new Arguments;
            this->actions=new Actions;
            this->arguments->config=config;
            this->arguments->log=log;
            this->actions->config=config;
            this->actions->log=log;
        }

        ~Parser(){
            delete this->config;
            this->config= nullptr;
            delete this->log;
            this->log=nullptr;
            delete this->actions;
            this->actions=nullptr;
            delete this->arguments;
            this->arguments=nullptr;
        }
        void defaultConfig(){
            delete this->config;
            this->config= nullptr;
            delete this->log;
            this->log= nullptr;
        }
        //over m_log
        void loadLog(Log*m_log){
            delete this->log;
            this->log= m_log;
        }
        //over config
        void loadConfig(ParserConfig *m_config){
            delete this->config;
            this->config= m_config;
        }
        //only package with a layer
        inline IAction* addAction(std::string&& action_name,std::string&& describe,ActionFun af,char short_cut= COC_NULL_CHAR){
            return this->actions->addAction(action_name,describe,af,short_cut);
        }
        //only package with a layer
        inline IAction* addHelpAction(std::string&& action_name,std::string&& describe,IHelpFunc*m_hf,char short_cut= COC_NULL_CHAR){
            return this->actions->addHelpAction(action_name,describe, m_hf,short_cut);
        }
        //only package with a layer
        inline Parser* addArgument(std::string&& argument_name,std::string&& argument_type,std::string&& describe){
            this->arguments->addArgument(argument_name,argument_type,describe);
            return this;
        }
        //get and set function
        inline ParserConfig *get_config(){
            return this->config;
        }

        inline Log* get_log(){
            return this->log;
        }

        inline IAction* set_global_actions(ActionFun af){
            if(this->actions->global==nullptr)
                this->actions->global=new GlobalAction(std::move(af),COC_NULL_CHAR,this->config,this->log);
            return this->actions->global;
        }

        inline Arguments* get_argument(){
            return this->arguments;
        }
        //set help function
        void set_help(help_fun &m_hf){
            this->is_def_hp= false;
            this->hf = m_hf;
        }

        int run(int argc,char** argv) {
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
            //optimize
            if(argc==1&&this->actions->global!=nullptr){
                COC_IS_GLOBAL
                return this->actions->global->run();
            }
            bool is_common=this->actions->isHavaAction(argv[1]);
            if(argc==2&&is_common){
                COC_IS_COMMON
                return this->actions->run(argv[1]);
            }

            //analyse
            std::list<std::string_view> all_argv;//all cmd argv
            std::list<std::string_view> opt_tar;

            int i=1;
            //if it isn't the global action, then read from index of 2
            if(is_common) i=2;
            for (; i < argc; ++i) {
                all_argv.emplace_back(argv[i]);
            }
            for(auto&p:all_argv){
                if(p[0]=='-') {
                    if (p[1] == this->config->argument_mark) {
                        if(!arguments->run(p))
                            return -1;
                    }
                }
                else {
                    opt_tar.push_back(p);
                }
            }
            //execute
            if(!is_common){
                if(this->actions->global==nullptr) {
                    this->log->globalActionNotDoesNotExist();
                    return -1;
                }
                else {
                    return this->actions->global->run(opt_tar, this->arguments);
                }
            }
            return this->actions->run(argv[1],opt_tar,this->arguments);

        }
    };

    struct HelpFunc:public IHelpFunc{
        void run(Getter g) override{
            this->printBasicMessage();
            this->printActions();
            this->printGlobalOptions();
            this->printGlobalValues();
            this->printArguments();
        }

        virtual inline void operator()(Getter g)final{
            this->run(g);
        }

        virtual void printBasicMessage(){
            std::cout<<config->logo_and_version<<'\n';
            if(!config->introduce.empty()){
                std::cout<<config->introduce<<'\n';
            }
            if(!config->usage.empty()){
                std::cout<<config->usage<<'\n';
            }
        }

        virtual void printActions(){
            if(!parser->actions->actions.empty()) {
                std::cout << "Actions:\n";
                for (auto &[k, v]: parser->actions->actions) {
                    if (v->short_cut != COC_NULL_CHAR)
                        std::cout << '\t' << v->short_cut << ',';
                    else
                        std::cout << '\t' << "  ";
                    std::cout << k << '\t' << v->get_describe() << '\n';
                }
            }
        }

        virtual inline void printGlobalOptions(){
            HelpFunc::printOptions(this->parser->actions->global->options);
        }

        virtual inline void printGlobalValues(){
            HelpFunc::printValues(this->parser->actions->global->values);
        }

        virtual inline void printArguments(){
            if(!parser->arguments->arguments.empty()){
                std::cout<<"Arguments:\n";
                for (auto &[k,v]: parser->arguments->arguments) {
                    std::cout<<'\t' <<k<<"\tType:"<<v->argument_type<<'\t'<<v->describe <<'\n';
                }
            }
        }

        static void printOptions(Options*options){
            if(!options->options.empty()) {
                std::cout << "Options(Global):\n";
                for (auto p: options->options) {
                    if (p->short_name != COC_NULL_CHAR)
                        std::cout << "\t-" << p->short_name << ',';
                    else
                        std::cout << '\t' << "   ";
                    std::cout << "--" << p->name << '\t' << p->intro << '\n';
                }
            }
        }

        static void printValues(Values*values){
            if(!values->values.empty()){
                std::cout<<"Values(Global):\n";
                for (auto p: values->values) {
                    std::cout<<'\t' <<p->value_name<<"\tType:"<<p->value_type<<'\t'<< p->intro;
                    if(!p->default_value.empty()){
                        std::cout<<"\tDefault="<<p->default_value;
                    }
                    std::cout<<'\n';
                }
            }
        }

        static void printAction(IAction* action){
            HelpFunc::printOptions(action->options);
            HelpFunc::printValues(action->values);
        }

        HelpFunc(ParserConfig*config,Parser*parser):
                                                         IHelpFunc(config,parser)
                                                                 {};
    };
}//namespace coc
#endif//!COC_HPP