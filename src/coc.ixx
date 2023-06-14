module;
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
export module coc;

using namespace std;
#undef COC_NULL_CHAR
#define COC_NULL_CHAR '\0'
namespace coc {
    export class Parser;
    class Values;
    class Option;

    export struct ParserConfig{
        bool intellisense_mode=true;//not supported now
        bool is_exit_if_not_found_option=true;
        bool argument_need_extern=true;
        string logo_and_version=
                "coc v1.0.0\n"
                "\t  ____ ___   ____ \n"
                "\t / ___/ _ \\ / ___|\n"
                "\t| |  | | | | |    \n"
                "\t| |__| |_| | |___ \n"
                "\t \\____\\___/ \\____|\n"
                "\t\t\tcoc by dream727\n"
                "==> https://github.com/dream727/coc";//your app's version
        string introduce;
        string usage;
        char argument_mark='D';//-[argument_mark] mark as argument
    };

    export struct Log{
        virtual inline void unidentifiedArgument(const string& argument){
            printf("Error:Unidentified argument:%s.\n",argument.c_str());
        }
        virtual inline void unidentifiedOption(const string& option){
            printf("Error:Unidentified option:--%s.\n",option.c_str());
        }
        virtual inline void unidentifiedOption(char option){
            printf("Error:Unidentified option:-%c.\n",option);
        }
        virtual inline void noValueEntered(const string& value){
            printf("Error:Value:%s not assigned.\n",value.c_str());
        }
        virtual inline void notFoundAction(const string& action){
            printf("Error:Not found action:%s.\n",action.c_str());
        }
        virtual inline void valueLog(const string &value_log,const string& default_value){
            string temp;
            if(!default_value.empty())
                temp="(default="+default_value+")";
            printf("%s%s:\n",value_log.c_str(),temp.c_str());
        }
        virtual inline void globalActionNotDoesNotExist(){
            cout<<"Error:Global action doesn't exist.\n";
        }

    };

    struct Option{
        string name, intro;
        int number;
        char short_name;
        Option(string&& name,string&&intro,int number,char short_name):
                                                                             name(std::move(name)), intro(std::move(intro)),number(number), short_name(short_name)
        {}
    };

    class Targets{
        friend class Options;
        friend class Action;
        friend class Parser;
    private:
        struct Target{
            Option* option;
            vector<string_view>target_list;
            explicit Target(Option*opt)
                :option(opt)
            {}
        };
        vector<Target*>targets_list;
        inline void run(Option*opt){
            this->targets_list.push_back(new Target(opt));
        }
        inline void run(string_view target){
            this->targets_list.rbegin().operator*()->target_list.push_back(target);
        }
    public:
        Targets()=default;
        ~Targets(){
            for(auto&p:this->targets_list){
                delete p;
                p=nullptr;
            }
        }
        const char* at(int index,const string &_default){
            if(index<this->targets_list[0]->target_list.size()){
                return this->targets_list[0]->target_list[index].data();
            }
            return _default.data();
        }
        const char* at(const string&option_name,int index,const string&_default){
            for(auto iter:this->targets_list){
                if(iter->option->name==option_name){
                    if(index<iter->target_list.size()){
                        return iter->target_list[index].data();
                    }
                    break;
                }
            }
            return _default.data();
        }
        const char* atOutOfRange(const string&option_name,int index,const string&_default){
            auto iter = targets_list.begin();
            for (; iter < this->targets_list.end(); ++iter) {
                if(iter.operator*()->option->name==option_name){
                    break;
                }
            }
            for (;iter < this->targets_list.end(); ++iter) {
                if(index-iter.operator*()->target_list.size()> -1){
                    index-=static_cast<int>(iter.operator*()->target_list.size());
                }
                else{
                    return iter.operator*()->target_list[index].data();
                }
            }
            return _default.data();
        }
        const char* atAbsoluteIndex(int index,const string&_default){
            for(auto iter:this->targets_list){
                if(index-iter->target_list.size()> -1){
                    index-=static_cast<int>(iter->target_list.size());
                }
                else{
                    return iter->target_list[index].data();
                }
            }
            return _default.data();
        }
        size_t size(){
            size_t result=0;
            for(auto iter:this->targets_list){
                result+=static_cast<int>(iter->target_list.size());
            }
            return result;
        }
        size_t size(const string& option_name){
            for(auto iter:this->targets_list){
                if(iter->option->name==option_name){
                    return iter->target_list.size();
                }
            }
            return 0;
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

        ParserConfig *config;
        Log *log;
        vector<Option*> options;//options list
        vector<Option*> options_u;//user options list
        //add an option to options list
        inline void addOption(string&& name,string&&intro,int number,char short_name){
            auto p=new Option(std::move(name), std::move(intro),number, short_name);
            this->options.push_back(p);
        }
        //add opt_tar which user input.
        bool run(list<string_view>& opt_tar){
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
                    string_view options_str=str.substr(1,str.size()-1);
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
        inline bool getOption(const string& option){
            return std::ranges::any_of(options_u, [&](auto iter) {
                return iter->name == option;
            });
        }
        inline bool getOption(char short_name){
            return std::ranges::any_of(options_u, [&](auto iter) {
                return iter->short_name == short_name;
            });
        }
        inline bool isOnlyOpt(const string& option){
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
        inline vector<Option*> get_list(){
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
            string value_name,value_log,value_type, intro,default_value;
            Value(string&&name,string&& val_log,string&& type,string&&intro,string&& def_val):
                                                                                                      value_name(std::move(name)),value_log(std::move(val_log)),value_type(std::move(type)), intro(std::move(intro)),default_value(std::move(def_val))
            {}
        };
        vector<Value*> values;
        map<string,string> values_u;
        ParserConfig* config;
        Log* log;

        //add a value to list
        inline void addValue(string&& name,string&& val_log,string&& type,string&& intro,string&& def_val){
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
            string buff;
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
        inline int getInt(const string& name){
            return stoi(this->values_u[name]);
        }
        inline float getFloat(const string& name){
            return stof(this->values_u[name]);
        }
        inline char getChar(const string& name){
            return this->values_u[name][0];
        }
        inline bool getBool(const string& name){
            string& buff=this->values_u[name];
            if(buff=="FALSE"||buff=="False"||buff=="false"||buff=="0")
                return false;
            else
                return true;
        }
        inline string getString(const string& name){
            return this->values_u[name];
        }
    };

    class Arguments{
        friend class Parser;
        friend struct HelpFunc;
    private:
        struct Argument{
            string argument_type,describe;

            Argument(string& argument_type,string& describe):
                                                                argument_type(argument_type),describe(describe)
            {}
        };

        ParserConfig* config;
        Log* log;
        map<string,Argument*> arguments;
        map<string,string> arguments_u;

        //add an argument to list
        inline void addArgument(string& argument_name,string& argument_type,string& describe){
            this->arguments[argument_name]=new Argument(argument_type,describe);
        }
        bool run(string_view argv){
            /*
            * in this step complete:
            * Analysis of argument
            */

            /*
             * after that
             * call none
             */
            string key;
            string value;
            string buff;

            buff = argv.substr(2, argv.size() - 2);
            for (char & i : buff) {
                if(i=='='){
                    i=' ';
                }
            }
            istringstream ss(buff);
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
        inline int getInt(const string& name,int default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return stoi(p->second);
        }
        inline float getFloat(const string& name,float default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return stof(p->second);
        }
        inline char getChar(const string& name,char default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return p->second[0];
        }
        inline bool getBool(const string& name,bool default_){
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
        inline string getString(const string& name,const string& default_){
            auto p=this->arguments_u.find(name);
            if(p==this->arguments_u.end())
                return default_;
            return p->second;
        }
    };

    export struct Getter{
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
    using ActionFun =function<void(Getter)>;

    ActionFun coc_empty=[](Getter){return;};

    //Action interface
    export struct IAction{
        friend class Actions;
        friend struct HelpFunc;
    protected:
        virtual int run()=0;
        virtual int run(list<string_view>&opt_tar, Arguments *arguments)=0;
        char short_cut{};
        Options* options;
        Values* values;
    public:
        virtual const string& get_describe()=0;
        virtual IAction* addOption(string&&name,string &&intro,int num,char s_name =COC_NULL_CHAR) {
            this->options->addOption(std::move(name), std::move(intro),num, s_name);
            return this;
        }
        virtual IAction* addValue(string&&name,string&&val_log,string&&type,string&&intro,string&&def_val ="") {
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

    export struct IHelpFunc{
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
        string describe;
        IHelpFunc* hf;
        inline int run() override{
            if (!this->values->run()) return -1;
            this->hf->run(Getter(this->values));
            return 0;
        }
        int run(list<string_view> &opt_tar, coc::Arguments *arguments) override{
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
        HelpAction(string& describe,IHelpFunc*hf,char short_cut,ParserConfig*config,Log*log):
                                                                                                      IAction(new Options(),new Values(),short_cut,config,log)
        {
            this->describe=describe;
            this->hf=hf;
        }

        inline const string& get_describe() override{
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
        inline int run(list<string_view>&opt_tar, Arguments *arguments) override{
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
        inline const string & get_describe() override{return "null"s;}

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
        string describe;
    public:
        inline const string & get_describe() override{
            return this->describe;
        }
        Action(string& describe, ActionFun af,char short_cut,ParserConfig*config,Log*log):
                                                                                                 AAction(af,short_cut,config,log)
        {
            this->describe=describe;
        }
    };

    export class Actions{
        friend class Parser;
        friend struct HelpFunc;
    private:
        Log* log;
        ParserConfig *config;
        GlobalAction* global;
        map<string, IAction*> actions;
        bool isHavaAction(const string& name){
            if(name.size()==1){
                bool result=false;
                for(auto&[k,v]:this->actions){
                    if(v->short_cut!=COC_NULL_CHAR&&v->short_cut==name[0])
                        result=true;
                }
                return result;
            }
            if (this->actions.find(name) == this->actions.end())
                return false;
            return true;
        }
        int run(const string& action_name){
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
        int run(const string& action_name,list<string_view>& opt_tar,Arguments *arguments) {
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
                    log(nullptr),config(nullptr),global(nullptr),actions(map<string, IAction*>())
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
        inline IAction* addAction(string& action_name,string& describe, ActionFun & af,char short_cut= COC_NULL_CHAR){
            auto action =new Action(describe,af,short_cut,this->config,this->log);
            this->actions[action_name]=action;
            return action;
        }

        //add a help action
        inline IAction* addHelpAction(string& action_name,string& describe,IHelpFunc* hf,char short_cut= COC_NULL_CHAR){
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
        inline IAction* addAction(string&& action_name,string&& describe,ActionFun af,char short_cut= COC_NULL_CHAR){
            return this->actions->addAction(action_name,describe,af,short_cut);
        }
        //only package with a layer
        inline IAction* addHelpAction(string&& action_name,string&& describe,IHelpFunc*m_hf,char short_cut= COC_NULL_CHAR){
            return this->actions->addHelpAction(action_name,describe, m_hf,short_cut);
        }
        //only package with a layer
        inline Parser* addArgument(string&& argument_name,string&& argument_type,string&& describe){
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
            bool is_common;
            //optimize
            if(this->actions->global!=nullptr){
                is_common=this->actions->isHavaAction(argv[1]);
                if(argc==1&&!is_common) {
                    COC_IS_GLOBAL
                    return this->actions->global->run();
                }
                else if(argc==2&&is_common){
                   COC_IS_COMMON
                   return this->actions->run(argv[1]);
                }
            }
            else{
                COC_IS_COMMON
                if(argc==2)
                    return this->actions->run(argv[1]);
                is_common=true;
            }

            //analyse
            list<string_view> all_argv;//all cmd argv
            list<string_view> opt_tar;

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

    export struct HelpFunc:public IHelpFunc{
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
            cout<<config->logo_and_version<<'\n';
            if(!config->introduce.empty()){
                cout<<config->introduce<<'\n';
            }
            if(!config->usage.empty()){
                cout<<config->usage<<'\n';
            }
        }

        virtual void printActions(){
            if(!parser->actions->actions.empty()) {
                cout << "Actions:\n";
                for (auto &[k, v]: parser->actions->actions) {
                    if (v->short_cut != COC_NULL_CHAR)
                        cout << '\t' << v->short_cut << ',';
                    else
                        cout << '\t' << "  ";
                    cout << k << '\t' << v->get_describe() << '\n';
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
                cout<<"Arguments:\n";
                for (auto &[k,v]: parser->arguments->arguments) {
                    cout<<'\t' <<k<<"\tType:"<<v->argument_type<<'\t'<<v->describe <<'\n';
                }
            }
        }

        static void printOptions(Options*options){
            if(!options->options.empty()) {
                cout << "Options(Global):\n";
                for (auto p: options->options) {
                    if (p->short_name != COC_NULL_CHAR)
                        cout << "\t-" << p->short_name << ',';
                    else
                        cout << '\t' << "   ";
                    cout << "--" << p->name << '\t' << p->intro << '\n';
                }
            }
        }

        static void printValues(Values*values){
            if(!values->values.empty()){
                cout<<"Values(Global):\n";
                for (auto p: values->values) {
                    cout<<'\t' <<p->value_name<<"\tType:"<<p->value_type<<'\t'<< p->intro;
                    if(!p->default_value.empty()){
                        cout<<"\tDefault="<<p->default_value;
                    }
                    cout<<'\n';
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