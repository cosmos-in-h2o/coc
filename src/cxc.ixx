//
// Created by dream on 4/16/23.
//
module;
#include <string>
#include <fmt/format.h>
#include <vector>
#include <set>

export module cxc;

using namespace std;

namespace cxc {
    export typedef struct ParserConfig{
        bool is_help_logs=true;//if open help logs.
        bool is_version_logs=true;//if open version logs.
        char argument_mark;//-[argument_mark] mark as argument
        string app_version;//your app's version
    } ParserConfig;

    export typedef struct Log{

    } Log;

    class Option{
    private:
        string name;
        string describe;
        char abbreviation;
        int number;
    public:
        Option(string name,string describe,int number,char abbreviation):
            name(name),describe(describe),number(number),abbreviation(abbreviation)
        {}
    };
    class Argument{
    private:
        string action_name;
        string argument_type;
        string describe;
    public:
        Argument(string action_name,string argument_type,string describe):
            action_name(action_name),argument_type(argument_type),describe(describe)
        {}
    };
    class Value{
    private:
        string value_name;
        string log;
        string value_type;
        string describe;
        string default_value;
        bool is_default;
    public:
        Value(string value_name,string log,string value_type,string describe):
                value_name(value_name),log(log),value_type(value_type),describe(describe),is_default(false)
        {}
        Value(string value_name,string log,string value_type,string describe,string default_value):
            value_name(value_name),log(log),value_type(value_type),describe(describe),default_value(default_value),is_default(true)
        {}
    };
    class Action{
    private:
        string name;
        string describe;
        char abbreviation;
        vector<Option> options;
        vector<Value> values;
    public:
        Action(string name,string describe,char abbreviation):
            name(name),describe(describe),abbreviation(abbreviation){}
        Action* addOption(string option_name,string describe,int number,char abbreviation=NULL){
            this->options.push_back(Option(option_name,describe,number,abbreviation));
            return this;
        }

        //add value
        Action* addValue(string value_name,string log,string value_type,string describe){
            this->values.push_back(Value(value_name,log,value_type,describe));
            return this;
        }
        Action* addValue(string value_name,string log,string value_type,string describe,string default_value){
            this->values.push_back(Value(value_name,log,value_type,describe,default_value));
            return this;
        }
    };

    export class Parser{
    public:
        typedef void(*help_fun)(Parser*);
    private:
        ParserConfig *config= nullptr;
        Log *log= nullptr;
        bool is_def_hp = true;//if open default function
        help_fun hp;
        vector<Action*> actions;
        set<Argument*> arguments;
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
            config(p),log(log){}

        ~Parser(){
            if(this->config != nullptr){
                delete this->config;
                this->config = nullptr;
            }
            for(auto& e:this->actions){
                if(e!=nullptr){
                    delete e;
                    e= nullptr;
                    auto a=10;
                }
            }
            if(!this->arguments.empty()){
                for(auto e:this->arguments){
                    delete e;
                }
                this->arguments.clear();
            }
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
        //load config from file
        void loadConfig(string &path){}
        //export config file
        void exportConfig(string& path){}
        void exportConfig(string& path,string& name){}
        //add an action
        Action* addAction(string action_name,string describe,char abbreviation=NULL){
            Action *action =new Action(action_name,describe,abbreviation);
            this->actions.push_back(action);
            return action;
        }
        //add an argument
        Parser* addArgument(string argument_name,string argument_type,string describe){
            Argument *argument =new Argument(argument_name,argument_type,describe);
            this->arguments.insert(argument);
            return this;
        }
        //get and set function

        //set help function
        void setHelp(help_fun &hp){
            this->is_def_hp= false;
            this->hp=hp;
        }
        void help(){
            if(!this->is_def_hp){
                this->hp(this);
                return;
            }
        }
        int run(int argc,char** argv) {
            return 0;
        }
    };

}