//
// Created by dream on 4/16/23.
//
module;
#include <string>
export module cxc;

using namespace std;

namespace cxc {
    export typedef struct ParserConfig{
        bool is_help_logs=true;//if open help logs.
        bool is_version_logs=true;//if open version logs.
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
        Parser(int argc,char** argv){
            this->config = new ParserConfig;
            this->log = new Log;
        }
        Parser(int argc,char** argv,ParserConfig &p,Log* log):
            log(log){
            *(this->config) = p;
        }
        Parser(int argc,char** argv,ParserConfig *p,Log& log):
            config(p){
            *(this->log) = log;
        }
        Parser(int argc,char** argv,ParserConfig &p,Log& log){
            *(this->config) = p;
            *(this->log) = log;
        }
        Parser(int argc,char** argv,ParserConfig *p,Log* log):
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
        void addAction(string action_name,string describe,char abbreviation=NULL){}
        //add an option
        void addOption(string option_name,string describe,char abbreviation=NULL){

        }
        //add an argument
        void addArgument(string action_name,string argument_type,string describe){}
        //add value
        void addValue(string value_name,string log,string value_type,string describe){}
        void addValue(string value_name,string log,string value_type,string describe,string default_value){}
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