module;
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>

export module coc;

using namespace std;

namespace coc {
    export class Options;
    export class Arguments;
    export class Values;

    //options,values,argv
    export typedef struct ParserConfig{
        bool is_help_logs=true;//if open help logs.
        bool is_version_logs=true;//if open version logs.
        bool intellisense=true;//not supported now
        string logo_and_version;//your app's version
        char argument_mark;//-[argument_mark] mark as argument
    } ParserConfig;
    export typedef void (*action_fun)(Options*,Arguments*,Values*,vector<string>&);

    export typedef struct Log{
        const char* not_found_argument="Error:Not found argument:%s.";
    } Log;

    class Options{
        friend class Action;
    private:
        typedef struct Option{
            string name;
            string describe;
            int number;
            char short_cut;
        } Option;
        vector<Option*> options;
        vector<int> numbers;
    public:
        inline int at(int index){
            if(index>=this->numbers.size()){
                return -1;
            }
            return this->numbers[index];
        }
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
        void addArgU(string key,string value){
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

    class Values{
        friend class Action;
    private:
        typedef struct Value{
            string value_name;
            string log;
            string value_type;
            string describe;
            string default_value;
            bool is_default;
        } Value;
        vector<Value*> values;
        map<string,string> values_u;
    public:

    };

    class Action{
        friend class Actions;
    private:
        string describe;
        action_fun af;
        char short_cut;
        Options* options;
        Values* values;

        int run(string& option,vector<string>& argv, Arguments *arguments){
            if(option[1] == '-'){
                for(auto p:this->options->options){
                    if(p->name == option.substr(2, option.size() - 2)){
                        this->options->numbers.push_back(p->number);
                        break;
                    }
                }
            }
            else{
                int i=0;
                for(auto p:this->options->options){
                    if(p->short_cut == option[i]) {
                        this->options->numbers.push_back(p->number);
                        i++;
                    }
                }
            }
            string buff;
            for(auto p:this->values->values){
                buff="";
                cout<<p->log;
                cin>>buff;
                if(buff==""){
                    if(p->is_default){
                        this->values->values_u[p->value_name]=p->default_value;
                    }
                    else{
                        //error
                        return -1;
                    }
                }
                else{
                    this->values->values_u[p->value_name]=buff;
                }
            }

            this->af(this->options,arguments,this->values,argv);
        }
    public:
        Action(string& describe,action_fun& af,char short_cut):
            describe(describe),af(af),short_cut(short_cut)
        {}

        inline Action* addOption(string name,string describe,int number,char short_cut=NULL){
            this->options->options.push_back(new Options::Option(name,describe,number,short_cut));
            return this;
        }

        inline Action* addValue(string value_name,string log,string value_type,string describe,string default_value){
            this->values->values.push_back(new Values::Value(value_name,log,value_type,describe,default_value,true));
            return this;
        }

        inline Action* addValue(string& value_name,string log,string value_type,string describe){
            this->values->values.push_back(new Values::Value(value_name,log,value_type,describe,"", false));
            return this;
        }
    };

    class Actions{
        friend class Parser;
    private:
        //an action with name,des,abb and af

        map<string, Action*> actions;
        //if error,the function will return false
        inline int run(string action_name,string& option,vector<string>& argv,Arguments *arguments) {
            auto p = this->actions.find(action_name);
            if (p == this->actions.end())
                return -1;
            return p->second->run(option,argv,arguments);
        }
    public:
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
        ParserConfig *config= nullptr;
        Log *log= nullptr;
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

        //get and set function
        inline ParserConfig *getConfig(){
            return this->config;
        }

        inline Log* getLog(){
            return this->log;
        }

        inline Actions* getActions(){
            return this->actions;
        }
        //set help function
        void setHelp(help_fun &hp){
            this->is_def_hp= false;
            this->hp=hp;
        }

        int run(int argc,char** argv) {
            vector<string> argv_v(10);
            string option;
            for(int i=1;i<argc;i++){
                if(argv[i][0]=='-'){
                    if(argv[i][1]==this->config->argument_mark)
                    {
                        string key="";
                        string value="";
                        int ptr=2;
                        for(; argv[i][ptr] != '='; ptr++){
                            key.push_back(argv[i][ptr]);
                        }
                        ++ptr;
                        for(;argv[i][ptr]!='\0';ptr++){
                            value.push_back(argv[i][ptr]);
                        }
                        this->arguments->addArgU(key,value);
                    }
                    else {
                        option=argv[i];
                    }
                    continue;
                }
                argv_v.emplace_back(argv[i]);
            }
            return this->actions->run(argv[1],option,argv_v,this->arguments);
        }
    };
}