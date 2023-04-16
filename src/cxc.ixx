//
// Created by dream on 4/16/23.
//
module;
#include <string>
export module cxc;

using namespace std;

namespace cxc {
    export typedef struct ParserConfig{
        bool is_help_logs;//if open help logs.
        bool is_version_logs;//if open version logs.
        string app_version;
    } ParserConfig;

    export typedef struct Log{

    } Log;

    export class Parser{
    public:
        typedef void(*help_fun)(Parser*);
    private:
        ParserConfig *config= nullptr;
        Log *log= nullptr;
        bool is_def_hp = true;//if open default function
        help_fun hp;
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
        }
        void defaultConfig(){
            if(this->config!= nullptr)
                delete this->config;
            if(this->log!= nullptr)
                delete this->log;
            //need add some code
        }
        //over config
        void loadConfig(ParserConfig &p){
            *(this->config)=p;
        }
        void loadConfig(ParserConfig *p){this->config=p;}
        //load config from file
        void loadConfig(string &path){}
        //export config file
        void exportConfig(string& path){}
        void exportConfig(string& path,string& name){}
        //add an action
        void add_action(string action_name,string describe,char abbreviation=NULL){}
        //add an option
        void add_option(string option_name,string describe,char abbreviation=NULL){

        }
        //add an argument
        void add_argument(string action_name,string argument_type,string describe){}
        //add value
        void add_value(string value_name,string log,string value_type,string describe){}
        void add_value(string value_name,string log,string value_type,string describe,string default_value){}
        //set help function
        void set_help(help_fun &hp){
            this->is_def_hp= false;
            this->hp=hp;
        }
        void help(){
            if(!this->is_def_hp){
                this->hp(this);
                return;
            }
        }

    };

}