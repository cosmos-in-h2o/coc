module;
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <regex>
#include <fmt/format.h>
export module coc;

using namespace std;

namespace coc {
    // export class Options;
    // export class Arguments;
    // export class Values;
    export typedef struct ParserConfig{
        bool is_help_logs=true;//if open help logs.
        bool is_version_logs=true;//if open version logs.
        bool intellisense=true;//not supported now
        bool is_global_action=true;
        bool is_exit_if_not_found_option=true;
        bool is_unix=true;
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
        char argument_mark='D';//-[argument_mark] mark as argument
    } ParserConfig;

    export struct Log{
        virtual inline void unidentifiedArgument(const string& argument){
            fmt::print("Error:Unidentified argument:{}.",argument);
        }
        virtual inline void unidentifiedOption(const string& option){
            fmt::print("Error:Unidentified option:--{}",option);
        }
        virtual inline void unidentifiedOption(char option){
            fmt::print("Error:Unidentified option:-{}",option);
        }
        virtual inline void noValueEntered(const string& value){
            fmt::print("Error:Value:{} not assigned",value);
        }
        virtual inline void notFoundAction(const string& action){
            fmt::print("Error:Not found action:{}",action);
        }
    };

    export class Options{
        friend class Action;
        friend class Parser;
    private:
        typedef struct Option{
            string name;
            string describe;
            int number;
            char short_cut;
        } Option;
        ParserConfig *config;
        Log *log;
        Options():
               config(nullptr),log(nullptr)
        {}

        ~Options(){
            for(auto p:this->options){
                delete p;
                p=nullptr;
            }
        }

        vector<Option*> options;//options list
        vector<Option*> options_u;//user options list
        //add an option to options list
        inline void addOption(string& name,string& describe,int number,char short_cut){
            this->options.push_back(new Options::Option(name,describe,number,short_cut));
        }
        //add options_argv which user input.
        bool run(vector<string>& options_argv){
            /*
            * in this step complete:
            * Analysis of options_argv
            */

            /*
             * after that
             * call none
             */
            for(auto &str:options_argv) {
                if (str[1] == '-') {
                    //if the 2nd char is -
                    for (auto iter_opt: this->options) {
                        //find right option
                        if (iter_opt->name == str.substr(2, str.size() - 2)) {
                            this->options_u.push_back(iter_opt);//add option ptr to options_u
                            return true;
                        }
                    }
                    log->unidentifiedOption(str);
                    return false;
                } else {
                    bool error = true;
                    for (auto iter_str: str) {
                        for (auto iter_opt: this->options) {
                            if (iter_opt->short_cut != NULL && iter_str == iter_opt->short_cut) {
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
        }
    public:
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
            for(auto iter:this->options_u){
                if(iter->name==option)
                    return true;
            }
            return false;
        }
        inline bool getOption(char short_cut){
            for(auto iter:this->options_u){
                if(iter->short_cut==short_cut)
                    return true;
            }
            return false;
        }
        inline vector<Option*> get_list(){
            return this->options_u;
        }
    };

    export class Arguments{
        friend class Parser;
    private:
        typedef struct Argument{
            string argument_type;
            string describe;
        } Argument;

        ParserConfig* config;
        Log* log;
        map<string,Argument*> arguments;
        map<string,string> arguments_u;

        ~Arguments(){
            for(auto& p:this->arguments){
                delete p.second;
                p.second= nullptr;
            }
        }
        //add an argument to list
        inline void addArgument(const string& argument_name,const string& argument_type,const string& describe){
            this->arguments[argument_name]=new Argument(argument_type,describe);
        }
        bool run(const string& argv){
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
            regex reg("=");
            sregex_token_iterator iter(buff.begin(), buff.end(), reg);
            try {
                key = *iter;
                iter++;
                value = *iter;
            }
            catch (regex_error &e) {
                cout << e.what() << "\ncode" << e.code();
                return false;
            }
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

    export class Values{
        friend class Action;
        friend class Parser;
    private:
        typedef struct Value{
            string value_name;
            string log;
            string value_type;
            string describe;
            string default_value;
        } Value;
        vector<Value*> values;
        map<string,string> values_u;
        ParserConfig* config;
        Log* log;
        ~Values(){
            for(auto&p:values){
                delete p;
                p= nullptr;
            }
        }

        //add a value to list
        inline void addValue(string& value_name,string& log,string& value_type,string& describe,string& default_value){
            this->values.push_back(new Values::Value(value_name,log,value_type,describe,default_value));
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
                cout<<iter->log;
                cin>>buff;
                if(buff.empty()){
                    if(iter->default_value.empty()){
                        log->noValueEntered(values_u[iter->value_name]);
                        return false;
                    }
                    else{
                        this->values_u[iter->value_name]=iter->default_value;
                        return true;
                    }
                }
                this->values_u[iter->value_name]=buff;
                return true;
            }
        }
    public:
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

    //action function pointer
    typedef void (*action_fun)(Options*,Arguments*,Values*,vector<string>&);

    class Action{
        friend class Actions;
        friend class Parser;
    private:
        string describe;
        action_fun af;
        char short_cut;
        Options* options;
        Values* values;
        //pass down
        ParserConfig *config;
        Log *log;
        inline int run(vector<string>& options_argv,vector<string>& argv, Arguments *arguments){
            /*
            * in this step complete:
            * do nothing,only call function
            */

            /*
             * after that
             * call options' run()
             * call values' run()
             */

            if(!this->options->run(options_argv)) return -1;
            if(!this->values->run()) return -1;
            this->af(this->options,arguments,this->values,argv);
        }
    public:
        Action(string& describe,action_fun& af,char short_cut):
              describe(describe),af(af),short_cut(short_cut)
        {
            this->values->config=this->config;
            this->values->log=this->log;
            this->options->config=this->config;
            this->options->log=this->log;
        }
        ~Action(){
            delete this->options;
            this->options=nullptr;
            delete this->values;
            this->values=nullptr;
        }
        inline Action* addOption(string& name,string &describe,int number,char short_cut=NULL){
            this->options->addOption(name,describe,number,short_cut);
            return this;
        }

        inline Action* addValue(string value_name,string log,string value_type,string describe,string default_value=""){
            this->values->addValue(value_name,log,value_type,describe,default_value);
            return this;
        }
    };

    export class Actions{
        friend class Parser;
    private:
        Log* log;
        ParserConfig *config;
        Action* global;
        map<string, Action*> actions;

        //if error,the function will return false
        inline int run(string action_name,vector<string>& options,vector<string>& argv,Arguments *arguments) {
            /*
            * in this step complete:
            * find the designated action
            */

            /*
             * after that
             * call action's run()
             */
            auto p = this->actions.find(action_name);
            if (p == this->actions.end()) {
                log->notFoundAction(action_name);
                return -1;
            }
            return this->actions[action_name]->run(options,argv,arguments);
        }
    public:
        ~Actions(){
            for(auto&p:this->actions){
                delete p.second;
                p.second= nullptr;
            }
            delete this->global;
            this->global=nullptr;
        }
        inline Action* get_global(){
            if(this->config->is_global_action)
                return this->global;
            return nullptr;
        }
        //add an action
        inline Action* addAction(string action_name,string describe,action_fun af,char short_cut=NULL){
            auto p =new Action(describe,af,short_cut);
            this->actions[action_name]=p;
            return p;
        }
    };

    export class Parser{
    public:
        typedef void(*help_fun)(Parser*);
    private:
        ParserConfig *config;
        Log *log;
        bool is_def_hp = true;//if open default function
        help_fun hp;
        Actions *actions;
        Arguments *arguments;
        void help(){
            if(!this->is_def_hp){
                this->hp(this);
                return;
            }
        }
    public:
        Parser(){
            this->config = new ParserConfig;
            this->log = new Log;
            this->arguments=new Arguments;
            this->actions=new Actions;
            this->arguments->config=this->config;
            this->arguments->log=this->log;
            this->actions->config=this->config;
            this->actions->log=this->log;
        }
        Parser(ParserConfig *p,Log* log):
               config(p),log(log)
        {
            this->arguments=new Arguments;
            this->actions=new Actions;
            this->arguments->config=this->config;
            this->arguments->log=this->log;
            this->actions->config=this->config;
            this->actions->log=this->log;
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
        //over log
        void loadLog(Log* log){
            delete this->log;
            this->log=log;
        }
        //over config
        void loadConfig(ParserConfig *p){
            delete this->config;
            this->config=p;
        }
        //only package with a layer
        inline Action* addAction(const string& action_name,const string& describe,action_fun af,char short_cut=NULL){
            return this->actions->addAction(action_name,describe,af,short_cut);
        }
        //only package with a layer
        inline Parser* addArgument(const string& argument_name,const string& argument_type,const string& describe){
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

        inline Actions* get_actions(){
            return this->actions;
        }

        inline Arguments* get_argument(){
            return this->arguments;
        }
        //set help function
        void set_help(help_fun &hp){
            this->is_def_hp= false;
            this->hp=hp;
        }

        int run(int argc,char** argv) {
            /*
             * in this step complete:
             * Analysis of argv except options and arguments
             * determine which is option
             * determine which is argument
             */

            /*
             * after that
             * call actions' run() or global_action's run()
             * call arguments' run()
             */

            //determine is the global action
            vector<string> all_argv(24);//all cmd argv
            vector<string> options;
            vector<string> argv_vector;//argv except options and arguments
            int i=1;
            //if it isn't the global action, then read from index of 2
            if(argv[1][0]!='-') i=2;
            for (; i < argc; ++i) {
                all_argv.emplace_back(argv[i]);
            }

            for(auto&p:all_argv){
                if(p[0]=='-')
                    if(p[1]==this->config->argument_mark)
                        if(!arguments->run(p))
                            return -1;
                        else
                            options.push_back(p);
                    else
                        argv_vector.push_back(p);
            }
            //run global
            if(this->config->is_global_action&&argv[1][0]=='-'){
                return this->actions->get_global()->run(options, argv_vector, this->arguments);
            }
            return this->actions->run(argv[1], options, argv_vector, this->arguments);
        }
    };
}//namespace coc