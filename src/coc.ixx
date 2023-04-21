module;
#include <string>
#include <vector>
#include <set>
#include <map>

export module cxc;
#define CXC_DEBUG
using namespace std;

namespace cxc {
    //options,values,argv
    export typedef void (*action_fun)(vector<int>&,vector<string>&,vector<string>&);
    export typedef struct ParserConfig{
        bool is_help_logs=true;//if open help logs.
        bool is_version_logs=true;//if open version logs.
        bool intellisense=true;//not supported now
        string logo_and_version;//your app's version
        char argument_mark;//-[argument_mark] mark as argument
    } ParserConfig;

    export typedef struct Log{
        const char* not_found_argument="Error:Not found argument:%s.";
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
    class Arguments{
        friend class Parser;
    private:
        class Argument{
            friend class Arguments;
        private:
            string argument_type;
            string describe;
            string default_value;
            bool is_default;
        public:
            Argument(string argument_type,string describe):
                    argument_type(argument_type),describe(describe),is_default(false)
            {}
            Argument(string argument_type,string describe,string default_value):
                    argument_type(argument_type),describe(describe),default_value(default_value),is_default(true)
            {}
        };
        Log* log;
        map<string,Argument*> arguments;
        map<string,string> usr_arguments;
        Arguments(Log* log){
            this->log=log;
        }
        inline void add(string argument_name,string argument_type,string describe){
            Argument* argument = new Argument(argument_type,describe);
            this->arguments[argument_name]=argument;
        }
        inline void add(string argument_name,string argument_type,string describe,string default_value){
            Argument* argument = new Argument(argument_type,describe,default_value);
            this->arguments[argument_name]=argument;
        }
        void addUsrArg(string key,string value){
            auto p=this->arguments.find(key);
            if(p!=this->arguments.end()){
                this->usr_arguments[key]=value;
            }
            else{
                printf(log->not_found_argument,key.c_str());
            }
        }
    public:
        char getChar(string argument_name){
            auto p=this->arguments.find(argument_name);
#ifdef CXC_DEBUG
            if(p==this->arguments.end()){
                printf("CodeError:Not found:%s.",argument_name.c_str());
                return 0;
            }
            if(p->second->argument_type!="char"){
                printf("CodeError:Is not char type.\tcxc.ixx:line 87.");
            }
#endif
            
            return '0';
        }
        string getString(string argument_name){
            return "0";
        }
        int getInt(string argument_name){
            return 0;
        }
        bool getBool(string argument_name){
            return 0;
        }
    };
    class ArgumentsGetter{
    private:
        map<string,string> arguments;
    public:
        char getChar(string argument_name){
            return '0';
        }
        string getString(string argument_name){
            return "0";
        }
        int getInt(string argument_name){
            return 0;
        }
        bool getBool(string argument_name){
            return 0;
        }
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
        action_fun af;
        vector<Option> options;
        vector<Value> values;
    public:
        Action(string name,string describe,action_fun af,char abbreviation):
            name(name),describe(describe),af(af),abbreviation(abbreviation)
        {}
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
        Arguments* arguments;
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
            if(this->config != nullptr){
                delete this->config;
                this->config = nullptr;
            }
            if(this->arguments!= nullptr){
                delete this->arguments;
                this->arguments= nullptr;
            }
            for(auto& e:this->actions){
                if(e!=nullptr){
                    delete e;
                    e= nullptr;
                }
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
        Action* addAction(string action_name,string describe,action_fun af,char abbreviation=NULL){
            Action *action =new Action(action_name,describe,af,abbreviation);
            this->actions.push_back(action);
            return action;
        }
        //add an argument
        Parser* addArgument(string argument_name,string argument_type,string describe){
            this->arguments->add(argument_name,argument_type,describe);
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