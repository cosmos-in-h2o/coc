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
//    export class Options;
//    export class Arguments;
//    export class Values;
    export typedef struct ParserConfig{
        bool is_help_logs=true;//if open help logs.
        bool is_version_logs=true;//if open version logs.
        bool intellisense=true;//not supported now
        bool is_global_action=true;
        string logo_and_version;//your app's version
        char argument_mark;//-[argument_mark] mark as argument
    } ParserConfig;

    export class Log{
    public:
        inline void notFoundArgument(const string& argument){
            fmt::print("Error:Not found argument:{}.",argument);
        }
    };

    export class Options{
        friend class Action;
    private:
        typedef struct Option{
            string name;
            string describe;
            int number;
            bool is_necessary;
            char short_cut;
        } Option;
        ParserConfig *config;
        Log *log;
        Options():
            config(nullptr),log(nullptr)
        {}

        Options(ParserConfig* config,Log* log):
            config(config),log(log)
        {}

        vector<Option*> options;
        vector<int> options_u;
        //add an option to options list
        inline void addOption(string& name,string& describe,int number,char short_cut){
            this->options.push_back(new Options::Option(name,describe,number,short_cut));
        }
        //add options which user input.
        void addOptionsU(string&str){
            if(str[1]=='-'){
                //if the 2nd char is -
                for(auto& iter_opt:this->options){
                    if(iter_opt->name == str.substr(2, str.size() - 2)){
                        this->options_u.push_back(iter_opt->number);
                        return;
                    }
                }
                //error at there
                //log->...

            }
            else {
                bool error=true;
                for (auto &iter_str: str){
                    for(auto &iter_opt:this->options){
                        if(iter_opt->short_cut!=NULL&&iter_str==iter_opt->short_cut){
                            this->options_u.push_back(iter_opt->number);
                            error= false;
                        }
                    }
                }
                if(error){
                    //error at there
                    //log->...


                }
            }
        }
        bool checkout(){
            for(auto&iter:this->options){
                if(iter->is_necessary) {
                    auto p= std::find(this->options_u.begin(), this->options_u.end(), iter->number);
                    if(p == options_u.end()){
                        //error
                        //log->...
                        return false;
                    }
                }
            }
            return true;
        }
    public:
        inline int at(int index){
            if(index>=this->options_u.size()){
                return -1;
            }
            return this->options_u[index];
        }
        inline int operator[](int index){
            return this->at(index);
        }
    };

    export class Arguments{
        friend class Parser;
    private:
        typedef struct Argument{
            string argument_type;
            string describe;
            string default_value;
        } Argument;

        ParserConfig* config;
        Log* log;
        map<string,Argument*> arguments;
        map<string,string> arguments_u;
        Arguments(Log* log){
            this->log=log;
        }
        //add an argument to list
        inline void addArgument(const string& argument_name,const string& argument_type,const string& describe,const string &default_value=""){
            this->arguments[argument_name]=new Argument(argument_type,describe,default_value);
        }
        bool addArgumentsU(const string& argv){
            string key;
            string value;
            string buff;

            buff=argv.substr(2,argv.size()-3);
            std::string long_str = "this_is_why_we_play";

            regex reg("=");
            sregex_token_iterator iter(buff.begin(),buff.end(),reg);
            try {
                key = *iter;
                iter++;
                value = *iter;
            }
            catch(regex_error& e){
                cout<<e.what()<<"\ncode"<<e.code();
                return false;
            }
            auto p=this->arguments.find(key);
            if(p!=this->arguments.end()){
                this->arguments_u[key]=value;
            }
            else{
                //error
                //log->...
                return false;
            }
            return true;
        }
    public:
        //the first is value.if the second is false,it means that the value was not found
        inline pair<int,bool> getInt(string name){}
        inline pair<float,bool> getFloat(string name){}
        inline pair<bool,bool> getBool(string name){}
        inline pair<char,bool> getChar(string name){}
        inline pair<string,bool> getString(string name){}
    };

    export class Values{
        friend class Action;
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
        //add a value to list
        inline void addValue(string& value_name,string& log,string& value_type,string& describe,string& default_value){
            this->values.push_back(new Values::Value(value_name,log,value_type,describe,default_value));
        }
        //put log and get value that user input
        bool log(){
            string buff;
            for(auto &iter:this->values){
                cout<<iter->log;
                cin>>buff;
                if(buff.empty()&&iter->default_value.empty()){
                    //error
                    //log->...
                    return false;
                }
                this->values_u[iter->value_name]=buff;
            }
        }
    public:
        //the first is value.if the second is false,it means that the value was not found
        inline pair<int,bool> getInt(string name){}
        inline pair<float,bool> getFloat(string name){}
        inline pair<bool,bool> getBool(string name){}
        inline pair<char,bool> getChar(string name){}
        inline pair<string,bool> getString(string name){}
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

        inline int run(vector<string>& option,vector<string>& argv, Arguments *arguments){
            for(auto&iter:option) {options->addOptionsU(iter);}
            if(!this->options->checkout()) return -1;
            if(!this->values->log()) return -1;

            this->af(this->options,arguments,this->values,argv);
        }
    public:
        Action(string& describe,action_fun& af,char short_cut):
            describe(describe),af(af),short_cut(short_cut)
        {}

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
            auto p = this->actions.find(action_name);
            if (p == this->actions.end()) {
                //error

                return -1;
            }
            return this->actions[action_name]->run(options,argv,arguments);
        }
    public:
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
        }
        Parser(ParserConfig &p,Log* log):
            log(log){
            *(this->config) = p;
        }
        Parser(ParserConfig *p,Log& log):
            config(p){
            *(this->log) = log;
        }
        Parser(ParserConfig &p,Log& log){
            *(this->config) = p;
            *(this->log) = log;
        }
        Parser(ParserConfig *p,Log* log):
            config(p),log(log)
        {}

        ~Parser(){

        }
        void defaultConfig(){
            if(this->config!= nullptr)
                delete this->config;
            if(this->log!= nullptr)
                delete this->log;
            //here need add some code
        }
        //over log
        void loadLog(Log& log){
            *(this->log)=log;
        }
        void loadLog(Log* log){
            if(this->log!= nullptr)
                delete this->log;
            this->log=log;
        }
        //over config
        void loadConfig(ParserConfig &p){
            *(this->config)=p;
        }
        void loadConfig(ParserConfig *p){
            if(this->config!= nullptr)
                delete this->config;
            this->config=p;
        }
        //only package with a layer
        inline Action* addAction(const string& action_name,const string& describe,action_fun af,char short_cut=NULL){
            return this->actions->addAction(action_name,describe,af,short_cut);
        }
        //only package with a layer
        inline Parser* addArgument(const string& argument_name,const string& argument_type,const string& describe,const string &default_value=""){
            this->arguments->addArgument(argument_type,describe,default_value,default_value);
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
            //determine global action
            vector<string> argv_vec(24);
            vector<string> options;
            vector<string> argv_v;
            int i=1;
            if(argv[1][0]!='-') i=2;
            for (; i < argc; ++i) {
                argv_vec.emplace_back(argv[i]);
            }

            for(auto&p:argv_vec){
                if(p[0]=='-')
                    if(p[1]==this->config->argument_mark)
                        arguments->addArgumentsU(p);
                    else
                        options.push_back(p);
                else
                    argv_v.push_back(p);
            }

            if(this->config->is_global_action&&argv[1][0]=='-'){
                return this->actions->get_global()->run(options,argv_v,this->arguments);
            }
            return this->actions->run(argv[1],options,argv_v,this->arguments);
        }
    };
}